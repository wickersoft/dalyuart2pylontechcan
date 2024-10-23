#ifndef UI_H
#define UI_H

#include <LiquidCrystal_I2C.h>
#include "stdint.h"

extern LiquidCrystal_I2C lcd;
extern uint16_t ui_max_charge_current;
extern uint16_t ui_max_discharge_current;

void lcd_display_init();
void print_battery_state_serial();
void print_battery_state_lcd();

#endif
