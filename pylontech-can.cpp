#include <SPI.h>
#include <mcp2515.h>
#include "pylontech-can.h"

struct can_frame canMsg;
struct can_frame canMsg359;
struct can_frame canMsg351;
struct can_frame canMsg355;
struct can_frame canMsg356;
struct can_frame canMsg35C;
struct can_frame canMsg35E;


uint8_t is_can_frame_received() {
  return mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK;
}

void can_data_print_request() {
  Serial.print(canMsg.can_id, HEX); // print ID
  Serial.print(" ");
  Serial.print(canMsg.can_dlc, HEX); // print DLC
  Serial.print(" ");

  for (int i = 0; i < canMsg.can_dlc; i++)  { // print the data
    Serial.print(canMsg.data[i], HEX);
    Serial.print(" ");
  }

  Serial.println();
}

void can_data_init() {
  canMsg359.can_id  = 0x359;
  canMsg359.can_dlc = 8;
  canMsg359.data[0] = 0;
  canMsg359.data[1] = 0;
  canMsg359.data[2] = 0;
  canMsg359.data[3] = 0;
  canMsg359.data[4] = 1;
  canMsg359.data[5] = 'P';
  canMsg359.data[6] = 'N';
  canMsg359.data[7] = 0;

  canMsg351.can_id  = 0x351;
  canMsg351.can_dlc = 8;
  canMsg351.data[0] = 0xE0;
  canMsg351.data[1] = 0x01;
  canMsg351.data[2] = 0x80;
  canMsg351.data[3] = 0x00;
  canMsg351.data[4] = 0x80;
  canMsg351.data[5] = 0x00;
  canMsg351.data[6] = 0;
  canMsg351.data[7] = 0;

  canMsg355.can_id  = 0x355;
  canMsg355.can_dlc = 8;
  canMsg355.data[0] = 0x50;
  canMsg355.data[1] = 0x00;
  canMsg355.data[2] = 0x50;
  canMsg355.data[3] = 0x00;
  canMsg355.data[4] = 0;
  canMsg355.data[5] = 0;
  canMsg355.data[6] = 0;
  canMsg355.data[7] = 0;

  canMsg356.can_id  = 0x356;
  canMsg356.can_dlc = 8;
  canMsg356.data[0] = 0x50;
  canMsg356.data[1] = 0x01;
  canMsg356.data[2] = 0x50;
  canMsg356.data[3] = 0x01;
  canMsg356.data[4] = 0xFF;
  canMsg356.data[5] = 0;
  canMsg356.data[6] = 0;
  canMsg356.data[7] = 0;

  canMsg35C.can_id  = 0x35C;
  canMsg35C.can_dlc = 1;
  canMsg35C.data[0] = 0;
  canMsg35C.data[1] = 0;
  canMsg35C.data[2] = 0;
  canMsg35C.data[3] = 0;
  canMsg35C.data[4] = 0;
  canMsg35C.data[5] = 0;
  canMsg35C.data[6] = 0;
  canMsg35C.data[7] = 0;

  canMsg35E.can_id  = 0x35E;
  canMsg35E.can_dlc = 5;
  canMsg35E.data[0] = 'P';
  canMsg35E.data[1] = 'Y';
  canMsg35E.data[2] = 'L';
  canMsg35E.data[3] = 'O';
  canMsg35E.data[4] = 'N';
  canMsg35E.data[5] = 0;
  canMsg35E.data[6] = 0;
  canMsg35E.data[7] = 0;

  mcp2515.reset();
  mcp2515.setBitrate(CAN_1000KBPS);
  mcp2515.setNormalMode();
}

void can_data_update(Daly_BMS_UART *bms) {




}

void can_data_transmit() {
  mcp2515.sendMessage(&canMsg359);
  mcp2515.sendMessage(&canMsg351);
  mcp2515.sendMessage(&canMsg355);
  mcp2515.sendMessage(&canMsg356);
  mcp2515.sendMessage(&canMsg35C);
  mcp2515.sendMessage(&canMsg35E);
  Serial.println("Messages sent");
}
