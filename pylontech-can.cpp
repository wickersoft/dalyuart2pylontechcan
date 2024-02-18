#include <SPI.h>
#include <mcp2515.h>
#include "pylontech-can.h"

struct can_frame canMsg;
struct can_frame canMsg359 {
  .can_id = 0x359,
  .can_dlc = 8,
};
struct can_frame canMsg351 {
  .can_id = 0x351,
  .can_dlc = 8,
};
struct can_frame canMsg355 {
  .can_id = 0x355,
  .can_dlc = 8,
};
struct can_frame canMsg356 {
  .can_id = 0x356,
  .can_dlc = 8,
};
struct can_frame canMsg35C {
  .can_id = 0x35C,
  .can_dlc = 1,
};
struct can_frame canMsg35E {
  .can_id = 0x35E,
  .can_dlc = 8,
};

struct {
  uint8_t protection1;
  uint8_t protection2;
  uint8_t alarm1;
  uint8_t alarm2;
  uint8_t num_modules;
  char magic_string[3];
} *status_flags = (typeof(status_flags)) canMsg359.data;

struct {
  uint16_t pack_decivolts;
  uint16_t charge_limit_deciamps;
  uint16_t discharge_limit_deciamps;
  uint8_t padding[2];
} *battery_voltage = (typeof(battery_voltage)) canMsg351.data;

struct {
  uint16_t soc_percent;
  uint16_t soh_percent;
  uint8_t padding[4];
} *battery_health = (typeof(battery_health)) canMsg355.data;

struct {
  uint16_t pack_centivolts;
  uint16_t pack_current_2cpl;
  uint16_t pack_temp_dc;
  uint8_t padding[2];
} *simple_measurements = (typeof(simple_measurements)) canMsg356.data;

struct {
  uint8_t flags;
} *requests = (typeof(requests)) canMsg35C.data;

struct {
  char magic_string[8];
} *manufacturer_name = (typeof(manufacturer_name)) canMsg35E.data;


#define M359_B0_T1_DISCH_OVERCURRENT (1 << 7)
#define M359_B0_T1_CELL_UNDERTEMP (1 << 4)
#define M359_B0_T1_CELL_OVERTEMP (1 << 3)
#define M359_B0_T1_CELL_UNDERVOLT (1 << 2)
#define M359_B0_T1_CELL_OVERVOLT (1 << 1)

#define M359_B1_T2_SYSTEM_ERROR (1 << 3)
#define M359_B1_T2_CHARGE_OVERCURRENT (1 << 0)

#define M359_B2_T3_DISCH_HIGH_CURRENT (1 << 7)
#define M359_B2_T3_CELL_LOW_TEMP (1 << 4)
#define M359_B2_T3_CELL_HIGH_TEMP (1 << 3)
#define M359_B2_T3_CELL_LOW_VOLT (1 << 2)
#define M359_B2_T3_CELL_HIGH_VOLT (1 << 1)

#define M359_B3_T4_INTERNAL_COMM_ERROR (1 << 3)
#define M359_B3_T4_CHARGE_HIGH_CURRENT (1 << 0)

#define M35C_B0_T5_CHARGE_ENABLE (1 << 7)
#define M35C_B0_T5_DISCH_ENABLE (1 << 6)
#define M35C_B0_T5_REQUEST_FORCE_CHARGE_1 (1 << 5)
#define M35C_B0_T5_REQUEST_FORCE_CHARGE_2 (1 << 4)
#define M35C_B0_T5_REQUEST_FULL_CHARGE (1 << 3)


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
  canMsg359.data[0] = 0;
  canMsg359.data[1] = 0;
  canMsg359.data[2] = 0;
  canMsg359.data[3] = 0;
  canMsg359.data[4] = 1;
  canMsg359.data[5] = 'P';
  canMsg359.data[6] = 'N';
  canMsg359.data[7] = 0;

  canMsg351.data[0] = 0xE0;
  canMsg351.data[1] = 0x01;
  canMsg351.data[2] = 0x80;
  canMsg351.data[3] = 0x00;
  canMsg351.data[4] = 0x80;
  canMsg351.data[5] = 0x00;
  canMsg351.data[6] = 0;
  canMsg351.data[7] = 0;

  canMsg355.data[0] = 0x50;
  canMsg355.data[1] = 0x00;
  canMsg355.data[2] = 0x50;
  canMsg355.data[3] = 0x00;
  canMsg355.data[4] = 0;
  canMsg355.data[5] = 0;
  canMsg355.data[6] = 0;
  canMsg355.data[7] = 0;

  canMsg356.data[0] = 0x50;
  canMsg356.data[1] = 0x01;
  canMsg356.data[2] = 0x50;
  canMsg356.data[3] = 0x01;
  canMsg356.data[4] = 0xFF;
  canMsg356.data[5] = 0;
  canMsg356.data[6] = 0;
  canMsg356.data[7] = 0;

  canMsg35C.data[0] = 0;
  canMsg35C.data[1] = 0;
  canMsg35C.data[2] = 0;
  canMsg35C.data[3] = 0;
  canMsg35C.data[4] = 0;
  canMsg35C.data[5] = 0;
  canMsg35C.data[6] = 0;
  canMsg35C.data[7] = 0;

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
  uint8_t i = 0;

  // MESSAGE 359 STATUS FLAGS
  // Byte 0 Protection/Critical 1
  if(bms->alarm.levelTwoDischargeCurrentTooHigh) {
    i |= M359_B0_T1_DISCH_OVERCURRENT;
  }
  if(bms->alarm.levelTwoChargeTempTooLow
  || bms->alarm.levelTwoDischargeTempTooLow ) {
    i |= M359_B0_T1_CELL_UNDERTEMP;
  }
  if(bms->alarm.levelTwoChargeTempTooHigh
  || bms->alarm.levelTwoDischargeTempTooHigh ) {
    i |= M359_B0_T1_CELL_OVERTEMP;
  }
  if(bms->alarm.levelTwoCellVoltageTooLow
  || bms->alarm.levelTwoPackVoltageTooLow ) {
    i |= M359_B0_T1_CELL_UNDERVOLT;
  }
  if(bms->alarm.levelTwoCellVoltageTooHigh
  || bms->alarm.levelTwoPackVoltageTooHigh ) {
    i |= M359_B0_T1_CELL_OVERVOLT;
  }
  status_flags->protection1 = i;
  i = 0;

  // Byte 1 Protection/Critical 2
  if(bms->alarm.levelTwoChargeCurrentTooHigh) {
    i |= M359_B1_T2_CHARGE_OVERCURRENT;
  }
//  if() {  // No condition currently known that should set this bit
//    i |= M359_B1_T2_SYSTEM_ERROR;
//  }
  status_flags->protection2 = i;
  i = 0;


  // Byte 2 Alarm/Warning 1 
  if(bms->alarm.levelOneDischargeCurrentTooHigh) {
    i |= M359_B2_T3_DISCH_HIGH_CURRENT;
  }
  if(bms->alarm.levelOneChargeTempTooLow
  || bms->alarm.levelOneDischargeTempTooLow ) {
    i |= M359_B2_T3_CELL_LOW_TEMP;
  }
  if(bms->alarm.levelOneChargeTempTooHigh
  || bms->alarm.levelOneDischargeTempTooHigh ) {
    i |= M359_B2_T3_CELL_HIGH_TEMP;
  }
  if(bms->alarm.levelOneCellVoltageTooLow
  || bms->alarm.levelOnePackVoltageTooLow ) {
    i |= M359_B2_T3_CELL_LOW_VOLT;
  }
  if(bms->alarm.levelOneCellVoltageTooHigh
  || bms->alarm.levelOnePackVoltageTooHigh ) {
    i |= M359_B2_T3_CELL_HIGH_VOLT;
  }
  status_flags->alarm1 = i;  
  i = 0;


  // Byte 3 Alarm/Warning 2
  if(bms->alarm.levelOneChargeCurrentTooHigh) {
    i |= M359_B3_T4_CHARGE_HIGH_CURRENT;
  }
//  if() {  // No condition currently known that should set this bit
//    i |= M359_B3_T4_INTERNAL_COMM_ERROR;
//  }
  status_flags->protection2 = i;
  
  status_flags->num_modules = 1;
  status_flags->magic_string[0] = 'P';
  status_flags->magic_string[1] = 'N';
  status_flags->magic_string[2] = 0;
  
  i = 0;

void can_data_transmit() {
  mcp2515.sendMessage(&canMsg359);
  mcp2515.sendMessage(&canMsg351);
  mcp2515.sendMessage(&canMsg355);
  mcp2515.sendMessage(&canMsg356);
  mcp2515.sendMessage(&canMsg35C);
  mcp2515.sendMessage(&canMsg35E);
  Serial.println("Messages sent");
}
