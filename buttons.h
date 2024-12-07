#ifndef BUTTONS_H
#define BUTTONS_H

#include "stdint.h"

extern uint8_t bms_offline_indicator;

extern uint8_t button_request_force_charge;
extern uint8_t button_cancel_force_charge;

void buttons_init();
void buttons_update();
void buttons_can_data_override();



#endif
