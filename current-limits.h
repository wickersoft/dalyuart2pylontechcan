#ifndef CURRENT_LIMITS_H
#define CURRENT_LIMITS_H

#include "stdint.h"

#define ABS_LIMIT_DISCHARGE 1750
#define ABS_LIMIT_CHARGE 1600
#define SLOPE_HIGH_END_MV 3505
#define SLOPE_LOW_END_MV 3105


uint16_t get_charge_limit_deciamps(uint16_t max_cell_mv, uint16_t bat_soc_permille, uint16_t temp_celsius, int16_t current_deciamps);

uint16_t get_discharge_limit_deciamps(uint16_t max_cell_mv, uint16_t bat_soc_permille, uint16_t temp_celsius, int16_t current_deciamps);

#endif
