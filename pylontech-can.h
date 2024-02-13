#ifndef PYLONTECH_CAN_H
#define PYLONTECH_CAN_H

#include "daly-bms-uart.h"

extern MCP2515 mcp2515;
extern struct can_frame canMsg;

uint8_t is_can_frame_received();
void can_data_init();
void can_data_update(Daly_BMS_UART *bms);
void can_data_transmit();


#endif
