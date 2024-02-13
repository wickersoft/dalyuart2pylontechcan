#include <SoftwareSerial.h>
#include <SPI.h>
#include <mcp2515.h>
#include "daly-bms-uart.h" // This is where the library gets pulled in
#include "pylontech-can.h"

#define BMS_SERIAL sSerial // Set the serial port for communication with the Daly BMS

SoftwareSerial sSerial(3, 5);
MCP2515 mcp2515(8);
Daly_BMS_UART bms(BMS_SERIAL); // Construct the BMS driver and passing in the Serial interface (which pins to use)

void setup()
{
  bms.Init(); // This call sets up the driver

  // Used for debug printing. We initialize it after the bms object to override the baudrate
  Serial.begin(115200); // Serial interface for the Arduino Serial Monitor
}

void loop()
{
  // Print a message and wait for input from the user
  Serial.println("Press any key and hit enter to query data from the BMS...");
  while (Serial.available() == 0)
  {
  }
  // TODO: Could these both be reduced to a single flush()?
  // Right now there's a bug where if you send more than one character it will read multiple times
  Serial.read(); // Discard the character sent
  Serial.read(); // Discard the new line

  // This .update() call populates the entire get struct. If you only need certain values (like
  // SOC & Voltage) you could use other public APIs, like getPackMeasurements(), which only query
  // specific values from the BMS instead of all.

  uint32_t ms = millis();
  bms.update();


  // And print them out!
  Serial.print("BMS update in ");
  Serial.print(millis() - ms);
  Serial.println("ms");
  Serial.println(F("Basic BMS Data:              "));
  Serial.print(bms.get.packVoltage);
  Serial.print("V ");
  Serial.print(bms.get.packCurrent);
  Serial.print("A ");
  Serial.print(bms.get.packSOC);
  Serial.println("\% ");
  Serial.print(F("Package Temperature (C):     "));
  Serial.println(bms.get.tempAverage);
  Serial.print(F("Highest Cell Voltage:        #"));
  Serial.print(bms.get.maxCellVNum);
  Serial.print(F(" with voltage "));
  Serial.println((bms.get.maxCellmV / 1000));
  Serial.print(F("Lowest Cell Voltage:         #"));
  Serial.print(bms.get.minCellVNum);
  Serial.print(F(" with voltage "));
  Serial.println((bms.get.minCellmV / 1000));
  Serial.print(F("Number of Cells:             "));
  Serial.println(bms.get.numberOfCells);
  Serial.print(F("Number of Temp Sensors:      "));
  Serial.println(bms.get.numOfTempSensors);
  Serial.print(F("BMS Chrg / Dischrg Cycles:   "));
  Serial.println(bms.get.bmsCycles);
  Serial.print(F("BMS Heartbeat:               "));
  Serial.println(bms.get.bmsHeartBeat); // cycle 0-255
  Serial.print(F("Discharge MOSFet Status:     "));
  Serial.println(bms.get.disChargeFetState);
  Serial.print(F("Charge MOSFet Status:        "));
  Serial.println(bms.get.chargeFetState);
  Serial.print(F("Remaining Capacity mAh:      "));
  Serial.println(bms.get.resCapacitymAh);
  //... any many many more data

  for (size_t i = 0; i < size_t(bms.get.numberOfCells); i++)
  {
    Serial.print(F("Cell voltage "));
    Serial.print(i);
    Serial.print(": ");
    Serial.println(bms.get.cellVmV[i]);
  }

  // Alarm flags
  // These are boolean flags that the BMS will set to indicate various issues.
  // For all flags see the alarm struct in daly-bms-uart.h and refer to the datasheet
  Serial.print(F("Level one Cell V to High:    "));
  Serial.println(bms.alarm.levelOneCellVoltageTooHigh);

  /**
     Advanced functions:
     bms.setBmsReset(); //Reseting the BMS, after reboot the MOS Gates are enabled!
     bms.setDischargeMOS(true); Switches on the discharge Gate
     bms.setDischargeMOS(false); Switches off thedischarge Gate
     bms.setChargeMOS(true); Switches on the charge Gate
     bms.setChargeMOS(false); Switches off the charge Gate
  */
}
