#include "buttons.h"
#include "current-limits.h"
#include "ui.h"

// Default values: 150A both directions
const uint16_t current_limit_charge = 1750;
const uint16_t current_limit_discharge = 1750;

uint8_t previously_full = 1;
uint8_t previously_empty = 1;

uint16_t get_charge_limit_deciamps(uint16_t max_cell_mv, uint16_t bat_soc_permille, uint16_t temp_celsius, int16_t current_deciamps) {
  if(bms_offline_indicator) {
    //return 100;
  }

  if(bat_soc_permille > 990) {
    previously_full = 1;
  } else if(bat_soc_permille < 970) {
    previously_full = 0;
  }
  
  int16_t limit = current_deciamps + 9 * (3505u - max_cell_mv); 

  if(temp_celsius < 10 && limit > 750) {
    limit = 750;
  }

  if(limit < 30) {
    limit = 30;
  }
  
  if(limit > current_limit_charge) {
    limit = current_limit_charge;
  }
  return limit;
}

uint16_t get_discharge_limit_deciamps(uint16_t min_cell_mv, uint16_t bat_soc_permille, uint16_t temp_celsius, int16_t current_deciamps) {
  if(bms_offline_indicator) {
    //return 100;
  }
 
  if(bat_soc_permille < 180) {
    previously_empty = 1;
  } else if(bat_soc_permille > 200) {
    previously_empty = 0;
  }

  int16_t limit = -current_deciamps + 9 * (min_cell_mv - 3205u); 

  if(temp_celsius < 10 && limit > 500) {
    limit = 500;
  }

  if(limit < 0) {
    limit = 0;
  }

  if(limit > current_limit_discharge) {
    return current_limit_discharge;
  }

  return limit;
}
