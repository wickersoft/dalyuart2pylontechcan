#include "arduino.h"

#include "buttons.h"
#include "current-limits.h"
#include "daly-bms-uart.h"
#include "ui.h"


const uint8_t customCharmV[] = {
  0B11100,
  0B11010,
  0B10010,
  0B00000,
  0B00101,
  0B00101,
  0B00010,
  0B00000
};

const uint8_t customCharDotdot[] = {
  0B00000,
  0B00000,
  0B00000,
  0B00000,
  0B00000,
  0B00000,
  0B01010,
  0B00000
};

const uint8_t customCharDelta[] = {
  0B00000,
  0B00000,
  0B00100,
  0B01010,
  0B01010,
  0B10001,
  0B11111,
  0B00000
};

const uint8_t customCharCelsius[] = {
  0B11000,
  0B11000,
  0B00111,
  0B01000,
  0B01000,
  0B01000,
  0B00111,
  0B00000
};

uint8_t customCharHalfBar[] = {
  0B11111,
  0B11111,
  0B11111,
  0B11111,
  0B11111,
  0B11111,
  0B11111,
  0B11111,
};

//LiquidCrystal_I2C lcd(0x3F, 20, 4); // I2C address 0x27, 16 column and 2 rows
LiquidCrystal_I2C lcd(0x27, 20, 4); // I2C address 0x27, 16 column and 2 rows

void lcd_display_init() {
  lcd.init(); // initialize the lcd
  lcd.backlight();
  lcd.createChar(0, customCharmV);
  lcd.createChar(1, customCharDotdot);
  lcd.createChar(3, customCharDelta);
  lcd.createChar(4, customCharCelsius);
}


void print_int_right_adjusted(int16_t value, uint8_t digits, uint8_t sign) {
  uint16_t absval = value >= 0 ? value : -value;
  if (value >= 0 && sign) {
    lcd.print(" ");
  }
  if (sign) {
    digits--;
  }
  if (absval < 10000 && digits >= 5) {
    lcd.print(" ");
  }
  if (absval < 1000 && digits >= 4) {
    lcd.print(" ");
  }
  if (absval < 100 && digits >= 3) {
    lcd.print(" ");
  }
  if (absval < 10 && digits >= 2) {
    lcd.print(" ");
  }
  lcd.print(value);
}

void print_float_right_adjusted(float value, uint8_t digits, uint8_t sign) {
  float absval = value >= 0 ? value : -value;
  if (value >= 0 && sign) {
    lcd.print(" ");
  }
  if (sign) {
    digits--;
  }
  if (absval < 1000 && digits >= 7) {
    lcd.print(" ");
  }
  if (absval < 100 && digits >= 6) {
    lcd.print(" ");
  }
  if (absval < 10 && digits >= 5) {
    lcd.print(" ");
  }
  lcd.print(value);
}

void print_battery_state_lcd() {
  //lcd.clear();                 // clear display

  uint8_t bat_div = bms.get.packSOC / 50;
  uint8_t bat_mod = (bms.get.packSOC / 10) % 5;

  for (uint8_t i = 0; i < 8; i++) {
    customCharHalfBar[i] = 0B11111 << (5 - bat_mod);
  }
  lcd.createChar(5, customCharHalfBar);

  lcd.setCursor(0, 0);         // move cursor to   (0, 0)
  uint16_t display_soc = bms.get.packSOC;
  if(button_cancel_force_charge) {
    display_soc = 1000;
  }
  if(button_request_force_charge) {
    display_soc = 90;
  }

  print_int_right_adjusted(display_soc / 10, 3, 0);
  lcd.print(".");
  lcd.print(display_soc % 10);
  lcd.print("% ");

  print_int_right_adjusted(bms.get.packVoltage / 10, 2, 0);
  lcd.print(".");
  lcd.print(bms.get.packVoltage % 10);
  lcd.print("V ");

  //lcd.setCursor(0, 0);         // move cursor to   (0, 0)
  print_int_right_adjusted(bms.get.packCurrent / 10, 4, 1);
  lcd.print(".");
  lcd.print(((bms.get.packCurrent % 10) + 10) % 10);
  lcd.print("A");

  lcd.setCursor(0, 1);         // move cursor to   (0, 0)

  for (uint8_t i = 0; i < bat_div; i++) {
    lcd.print((char) 0xFF);
  }
  lcd.print((char) 0x05);
  for (uint8_t i = bat_div + 1; i < 20; i++) {
    lcd.print(' ');
  }

  if(bms_offline_indicator) {
    lcd.setCursor(6, 1);         // move cursor to   (0, 0)
    lcd.print("OFFLINE!");  
  }


  //lcd.print(F("                    "));

  lcd.setCursor(0, 2);         // move cursor to   (0, 0)
  print_int_right_adjusted(bms.get.minCellmV, 4, 0);
  lcd.print((char) 0x01);
  print_int_right_adjusted(bms.get.maxCellmV, 4, 0);
  lcd.print((char) 0x00);
  lcd.print(" ");
  lcd.print((char) 0x03);
  print_int_right_adjusted(bms.get.maxCellmV - bms.get.minCellmV, 3, 0);
  lcd.print((char) 0x00);
  lcd.print(" ");

  print_int_right_adjusted(bms.get.tempAverage, 2, 0);
  lcd.print((char) 0x04);

  lcd.setCursor(0, 3);         // move cursor to   (0, 0)
  print_int_right_adjusted(bms.get.minCellVNum - 1, 3, 0);
  lcd.print("^ ");
  print_int_right_adjusted(bms.get.maxCellVNum - 1, 3, 0);
  lcd.print("^  L");

  uint16_t chg_amps = get_charge_limit_deciamps(bms.get.maxCellmV, bms.get.packSOC, bms.get.tempAverage, bms.get.packCurrent) / 10;
  uint16_t dis_amps = get_discharge_limit_deciamps(bms.get.minCellmV, bms.get.packSOC, bms.get.tempAverage, bms.get.packCurrent) / 10;

  print_int_right_adjusted(chg_amps, 3, 0);
  lcd.print("/");
  print_int_right_adjusted(dis_amps, 3, 0);
  lcd.print("A");
}
