#include "current-limits.h"
#include "ui.h"

uint8_t previously_full = 1;
uint8_t previously_empty = 1;

uint16_t get_charge_limit_deciamps(uint16_t max_cell_mv, uint16_t bat_soc_permille, uint16_t temp_celsius, int16_t current_deciamps) {
  //if(bms_offline_indicator) {
    //return 100;
  //}

  if(bat_soc_permille > 990) {
    previously_full = 1;
  } else if(bat_soc_permille < 970) {
    previously_full = 0;
  }
  
  int16_t limit = current_deciamps + 9 * (SLOPE_HIGH_END_MV - max_cell_mv); 

  if(temp_celsius < 10 && limit > 1000) {
    limit = 1000;
  }

  if(limit < 30) {
    limit = 30;
  }
  
  if(limit > ABS_LIMIT_CHARGE) {
    limit = ABS_LIMIT_CHARGE;
  }
  return limit;
}

uint16_t get_discharge_limit_deciamps(uint16_t min_cell_mv, uint16_t bat_soc_permille, uint16_t temp_celsius, int16_t current_deciamps) {
  //if(bms_offline_indicator) {
    //return 100;
  //}
 
  if(bat_soc_permille < 180) {
    previously_empty = 1;
  } else if(bat_soc_permille > 200) {
    previously_empty = 0;
  }

  int16_t limit = -current_deciamps + 9 * (min_cell_mv - SLOPE_LOW_END_MV); 

  if(temp_celsius < 10 && limit > 1000) {
    limit = 1000;
  }

  if(limit < 0) {
    limit = 0;
  }

  if(limit > ABS_LIMIT_DISCHARGE) {
    return ABS_LIMIT_DISCHARGE;
  }

  return limit;
}
