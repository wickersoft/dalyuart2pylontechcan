#include "current-limits.h"
#include "ui.h"

uint8_t previously_full = 1;
uint8_t previously_empty = 1;

uint16_t get_charge_limit_deciamps(uint16_t max_cell_mv, uint16_t bat_soc_permille, uint16_t temp_celsius, int16_t current_deciamps) {
  if(bat_soc_permille > 960) {
    previously_full = 1;
  } else if(bat_soc_permille < 950) {
    previously_full = 0;
  }
  
  if(max_cell_mv > 3550) {
    return 25;
  }

  int16_t limit = current_deciamps + 9 * (3500u - max_cell_mv); 
  if (previously_full) {
    limit /= 2;
  }
  if(limit < 25) {
    return 25;
  }
  if(limit > ui_max_charge_current) {
    return ui_max_charge_current;
  }
  return limit;
}

uint16_t get_discharge_limit_deciamps(uint16_t min_cell_mv, uint16_t bat_soc_permille, uint16_t temp_celsius, int16_t current_deciamps) {
  if(bat_soc_permille < 100) {
    previously_empty = 1;
  } else if(bat_soc_permille > 200) {
    previously_empty = 0;
  }

  if(bat_soc_permille < 110) {
    return 0;
  }
  
  if(min_cell_mv < 3100) {
    return 500;
  }

  return 1500;
}
