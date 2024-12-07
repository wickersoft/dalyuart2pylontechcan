#include <avr/wdt.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <mcp2515.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "buttons.h"
#include "current-limits.h"
#include "daly-bms-uart.h" // This is where the library gets pulled in
#include "pylontech-can.h"
#include "ui.h"

uint8_t bullshit_requested = 0;
uint8_t bms_offline_indicator = 0;

MCP2515 mcp2515(7);

void setup()
{
  watchdogSetup();
  //pinMode(PIN_PB2, INPUT_PULLUP);

  // Used for debug printing. We initialize it after the bms object to override the baudrate
  Serial.begin(115200); // Serial interface for the Arduino Serial Monitor
  Serial2.swap(1);
  Wire.begin();

  for (byte i = 8; i < 120; i++)
  {
    Wire.beginTransmission (i);
    if (Wire.endTransmission () == 0)
    {
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);
      Serial.println (")");
      delay (1);  // maybe unneeded?
    } else {
      Serial.print(".");
    }
  } // end of for loop
  Serial.println("\n");

  buttons_init();
  lcd_display_init();
  bms.Init(); // This call sets up the driver
  can_data_init();

  // Print a message and wait for input from the user
  Serial.println("(Press any key and hit enter to query data from the BMS...)");
  bms.update();
  can_data_update(&bms);
  print_battery_state_lcd();
}

void loop()
{
  // The inverter only sends keepalive messages with no data. Configured for Pylontech US5000, the inverter sends can messages 0x305, 0x306 and 0x307.
  // We simply respond after the last one.
  if (is_can_frame_received()) {
    if (canMsg.can_id == 0x307) {
      can_data_transmit(); // I have seen CAN messages rejected if sent spontaneously, so let's play it safe and transmit as fast as possible, at the cost of only knowing a 1-second old battery state.
      do {
        // This .update() call populates the entire get struct. If you only need certain values (like
        // SOC & Voltage) you could use other public APIs, like getPackMeasurements(), which only query
        // specific values from the BMS instead of all.
        bms_offline_indicator = !bms.update();
      } while (bms.get.packSOC > 1000); // Reject any implausible updates, this will go away when we have a hardware serial for the BMS
      can_data_update(&bms);
      buttons_update();
      can_data_apply_overrides();
      print_battery_state_lcd();
      print_battery_state_serial();
      wdt_reset(); // reset the WDT timer
    }

    /*
        Serial.print("CAN < ");
        Serial.print(canMsg.can_id, HEX);
        Serial.print(": ");
        for(int i = 0; i < canMsg.can_dlc; i++) {
            Serial.print(canMsg.data[i], HEX);
            Serial.print(" ");
        }
        Serial.print("\n");
    */
  }
}

void print_battery_state_serial() {
  // TODO: Could these both be reduced to a single flush()?
  // Right now there's a bug where if you send more than one character it will read multiple times
  while (Serial.available()) {
    Serial.read(); // Discard the character sent
  }

  // And print them out!
  Serial.println("\033[1A\033[K");

  uint16_t display_soc = bms.get.packSOC;
  if (button_cancel_force_charge) {
    display_soc = 1000;
    Serial.print(F("Cancel Charge: "));
    Serial.println(button_cancel_force_charge);
  }
  if (button_request_force_charge) {
    display_soc = 90;
    Serial.print(F("Force Charge: "));
    Serial.println(button_request_force_charge);
  }
  
  Serial.print("|");

  if (display_soc < 1000) {
    Serial.print(" ");
  }
  Serial.print(display_soc / 10);
  Serial.print('.');
  Serial.print(display_soc % 10);
  Serial.print("%  ");
  Serial.print(0.1 * bms.get.packVoltage);
  Serial.print("V  ");

  if (bms.get.packCurrent >= 0) {
    Serial.print(" ");
  }
  if (abs(bms.get.packCurrent) < 1000) {
    Serial.print(" ");
  }
  if (abs(bms.get.packCurrent) < 100) {
    Serial.print(" ");
  }
  Serial.print(0.1 * bms.get.packCurrent);
  Serial.println("A|");

  Serial.print(F("|       "));
  if (bms.get.resCapacitymAh < 100000) {
    Serial.print(" ");
  }
  if (bms.get.resCapacitymAh < 10000) {
    Serial.print(" ");
  }
  Serial.print(bms.get.resCapacitymAh * 0.001);
  Serial.print("Ah");;
  if (bms_offline_indicator) {
    Serial.print(" OFFLINE");
  } else {
    Serial.print("         ");
  }

  Serial.print("|\r\n|");
  Serial.print(bms.get.minCellmV);
  Serial.print("..");
  Serial.print(bms.get.maxCellmV);
  Serial.print(F("mV Δ"));
  if (bms.get.maxCellmV - bms.get.minCellmV < 100) {
    Serial.print(" ");
  }
  if (bms.get.maxCellmV - bms.get.minCellmV < 10) {
    Serial.print(" ");
  }
  Serial.print(bms.get.maxCellmV - bms.get.minCellmV);
  Serial.print("mV ");
  Serial.print(bms.get.tempAverage);
  Serial.println("°C|");

  Serial.print("|");
  if (bms.get.minCellVNum - 1 < 10) {
    Serial.print("  ");
  } else {
    Serial.print(" ");
  }
  Serial.print(bms.get.minCellVNum - 1);
  Serial.print("^  ");

  if (bms.get.maxCellVNum - 1 < 10) {
    Serial.print(" ");
  }
  Serial.print(bms.get.maxCellVNum - 1);
  Serial.print("^   L:  ");


  uint16_t charge_limit_deciamps = get_charge_limit_deciamps( bms.get.maxCellmV,  bms.get.packSOC,  bms.get.tempAverage,  bms.get.packCurrent);
  uint16_t discharge_limit_deciamps = get_discharge_limit_deciamps( bms.get.minCellmV,  bms.get.packSOC,  bms.get.tempAverage,  bms.get.packCurrent);

  if (charge_limit_deciamps < 1000) {
    Serial.print(" ");
  }
  if (charge_limit_deciamps < 100) {
    Serial.print(" ");
  }
  Serial.print(charge_limit_deciamps / 10);
  Serial.print("/");
  if (discharge_limit_deciamps < 1000) {
    Serial.print(" ");
  }
  if (discharge_limit_deciamps < 100) {
    Serial.print(" ");
  }
  Serial.print(discharge_limit_deciamps / 10);
  Serial.println("A|");

  Serial.print(F(" FETs: "));
  Serial.print(bms.get.chargeFetState);
  Serial.print("/");
  Serial.print(bms.get.disChargeFetState);

  // Alarm flags
  // These are boolean flags that the BMS will set to indicate various issues.
  // For all flags see the alarm struct in daly-bms-uart.h and refer to the datasheet

  Serial.print(F("Cells: "));
  if (bms.alarm.levelTwoCellVoltageTooLow) {
    Serial.print("LO  ");
  } else if (bms.alarm.levelOneCellVoltageTooLow) {
    Serial.print("lo  ");
  } else if (bms.alarm.levelOneCellVoltageTooHigh) {
    Serial.print("hi  ");
  } else if (bms.alarm.levelTwoCellVoltageTooHigh) {
    Serial.print("HI  ");
  } else {
    Serial.print("0   ");
  }

  Serial.print(F("Pack: "));
  if (bms.alarm.levelTwoPackVoltageTooLow) {
    Serial.print("LO ");
  } else if (bms.alarm.levelOnePackVoltageTooLow) {
    Serial.print("lo ");
  } else if (bms.alarm.levelOnePackVoltageTooHigh) {
    Serial.print("hi ");
  } else if (bms.alarm.levelTwoPackVoltageTooHigh) {
    Serial.print("HI ");
  } else {
    Serial.print("0   ");
  }

  Serial.print(F("C/D Cycles: "));
  Serial.print(bms.get.bmsCycles);
  Serial.print(F(" HB "));
  Serial.println(bms.get.bmsHeartBeat); // cycle 0-255


  for (uint8_t i = 0; i < 15; i++)
  {
    if (i < 10) {
      Serial.print("  ");
    } else {
      Serial.print(" ");
    }
    Serial.print(i);
    if (bms.get.cellBalanceState[i]) {
      Serial.print(":#");
    } else {
      Serial.print(": ");
    }    Serial.print(bms.get.cellVmV[i]);
    Serial.print(" ");
    if (i == 7 || i == 14) {
      Serial.println("");
    }
  }

  /**
     Advanced functions:
     bms.setBmsReset(); //Reseting the BMS, after reboot the MOS Gates are enabled!
     bms.setDischargeMOS(true); Switches on the discharge Gate
     bms.setDischargeMOS(false); Switches off thedischarge Gate
     bms.setChargeMOS(true); Switches on the charge Gate
     bms.setChargeMOS(false); Switches off the charge Gate
  */
}

void watchdogSetup(void)
{
  cli(); // disable all interrupts
  wdt_reset(); // reset the WDT timer
  wdt_enable(11);
  sei();
}
