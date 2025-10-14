#include <SPI.h>
#include <mcp2515.h>
#include "current-limits.h"
#include "bydcan.h"
#include "ui.h"
#include "buttons.h"

struct can_frame can_msg_readbuf;

struct can_frame bytes35E_manuf_info {
  .can_id = 0x35E,
    .can_dlc = 8,
};
struct can_frame bytes382_product_info {
  .can_id = 0x382,
   .can_dlc = 8,
};
struct can_frame bytes35F_battery_info {
  .can_id = 0x35F,
   .can_dlc = 8,
};
struct can_frame bytes35A_warnings {
  .can_id = 0x35A,
   .can_dlc = 8,
};
struct can_frame bytes35B_events_unused {
  .can_id = 0x35B,
   .can_dlc = 8,
};
struct can_frame bytes351_limits {
  .can_id = 0x351,
   .can_dlc = 8,
};
struct can_frame bytes355_soc_soh {
  .can_id = 0x355,
   .can_dlc = 8,
};
struct can_frame bytes356_major_measurements {
  .can_id = 0x356,
   .can_dlc = 8,
};
struct can_frame bytes360_unknown0 {
  .can_id = 0x360,
   .can_dlc = 8,
};
struct can_frame bytes372_bank_info {
  .can_id = 0x372,
   .can_dlc = 8,
};
struct can_frame bytes373_cell_info {
  .can_id = 0x373,
   .can_dlc = 8,
};
struct can_frame bytes374_cell_volt_min_id {
  .can_id = 0x374,
   .can_dlc = 8,
};
struct can_frame bytes375_cell_volt_max_id {
  .can_id = 0x375,
   .can_dlc = 8,
};
struct can_frame bytes376_cell_temp_min_id {
  .can_id = 0x376,
   .can_dlc = 8,
};
struct can_frame bytes377_cell_temp_max_id {
  .can_id = 0x377,
   .can_dlc = 8,
};
struct can_frame bytes378_history {
  .can_id = 0x378,
   .can_dlc = 8,
};
struct can_frame bytes379_battery_size {
  .can_id = 0x356,
   .can_dlc = 8,
};



struct {
  char manuf_string[8];
} *struct35E_manuf_info = (typeof(struct35E_manuf_info)) bytes35E_manuf_info.data;

struct {
  char product_string[8];
} *struct382_product_info = (typeof(struct382_product_info)) bytes382_product_info.data;

struct {
  char product_code[2];
  uint16_t fw_version;
  uint16_t ah_available;
  uint8_t padding[2];
} *struct35F_battery_info = (typeof(struct35F_battery_info)) bytes35F_battery_info.data;

struct {
  uint8_t flags[8];
} *struct35A_warnings = (typeof(struct35A_warnings)) bytes35A_warnings.data;

struct {
  uint8_t padding[8];
} *struct35B_events_unused = (typeof(struct35B_events_unused)) bytes35B_events_unused.data;

struct {
  uint16_t pack_decivolts_hi;
  uint16_t charge_limit_deciamps;
  uint16_t discharge_limit_deciamps;
  uint16_t pack_decivolts_lo;
} *struct351_limits = (typeof(struct351_limits)) bytes351_limits.data;

struct {
  uint16_t soc_percent;
  uint16_t soh_percent;
  uint8_t padding[4];
} *struct355_soc_soh = (typeof(struct355_soc_soh)) bytes355_soc_soh.data;

struct {
  uint16_t pack_centivolts;
  uint16_t pack_deciamps;
  uint16_t pack_temp_dc;
  uint8_t padding[2];
} *struct356_major_measurements = (typeof(struct356_major_measurements)) bytes356_major_measurements.data;

struct {
  uint8_t padding[8];
} *struct360_unknown0 = (typeof(struct360_unknown0)) bytes360_unknown0.data;

struct {
  uint16_t batteries_online;
  uint16_t batteries_block_charge;
  uint16_t batteries_block_discharge;
  uint16_t batteries_offline;
} *struct372_bank_info = (typeof(struct372_bank_info)) bytes372_bank_info.data;

struct {
  uint16_t lowest_cell_mv;
  uint16_t highest_cell_mv;
  uint16_t lowest_cell_temp_k;
  uint16_t highest_cell_temp_k;
} *struct373_cell_info = (typeof(struct373_cell_info)) bytes373_cell_info.data;

struct {
  char cell_id[8];
} *struct374_cell_volt_min_id = (typeof(struct374_cell_volt_min_id)) bytes374_cell_volt_min_id.data;

struct {
  char cell_id[8];
} *struct375_cell_volt_max_id = (typeof(struct375_cell_volt_max_id)) bytes375_cell_volt_max_id.data;

struct {
  char cell_id[8];
} *struct376_cell_temp_min_id = (typeof(struct376_cell_temp_min_id)) bytes376_cell_temp_min_id.data;

struct {
  char cell_id[8];
} *struct377_cell_temp_max_id = (typeof(struct377_cell_temp_max_id)) bytes377_cell_temp_max_id.data;

struct {
  uint32_t charged_hWh;
  uint32_t discharged_hWh;
} *struct378_history = (typeof(struct378_history)) bytes378_history.data;
  
struct {
  uint16_t installed_ah;
  uint8_t padding[6];
} *struct379_battery_size = (typeof(struct379_battery_size)) bytes379_battery_size.data;



#define M35A_B0_A_CELL_OVERTEMP (1 << 6)
#define M35A_B0_A_CELL_UNDERVOLT (1 << 4)
#define M35A_B0_A_CELL_OVERVOLT (1 << 2)
#define M35A_B0_A_UNUSED (1 << 0)

#define M35A_B1_A_DISCH_OVERCURRENT (1 << 6)
#define M35A_B1_A_CHARGE_UNDERTEMP (1 << 4)
#define M35A_B1_A_CHARGE_OVERTEMP (1 << 2)
#define M35A_B1_A_UNDERTEMP (1 << 0)

#define M35A_B2_A_SYSTEM_ERROR (1 << 6)
#define M35A_B2_A_UNUSED (1 << 4) || (1 << 2)
#define M35A_B2_A_CHARGE_OVERCURRENT (1 << 0)

#define M35A_B3_A_UNUSED (1 << 6) || (1 << 4) || (1 << 2)
#define M35A_B3_A_CELL_IMBAANCE (1 << 0)

#define M35A_B4_W_CELL_OVERTEMP (1 << 6)
#define M35A_B4_W_CELL_UNDERVOLT (1 << 4)
#define M35A_B4_W_CELL_OVERVOLT (1 << 2)
#define M35A_B4_W_UNUSED (1 << 0)

#define M35A_B5_W_DISCH_OVERCURRENT (1 << 6)
#define M35A_B5_W_CHARGE_UNDERTEMP (1 << 4)
#define M35A_B5_W_CHARGE_OVERTEMP (1 << 2)
#define M35A_B5_W_UNDERTEMP (1 << 0)

#define M35A_B6_W_SYSTEM_ERROR (1 << 6)
#define M35A_B6_W_UNUSED (1 << 4) || (1 << 2)
#define M35A_B6_W_CHARGE_OVERCURRENT (1 << 0)

#define M35A_B7_W_UNUSED (1 << 6) || (1 << 4) || (1 << 2)
#define M35A_B7_W_CELL_IMBAANCE (1 << 0)


#define M35C_B0_T5_CHARGE_ENABLE (1 << 7)
#define M35C_B0_T5_DISCH_ENABLE (1 << 6)
#define M35C_B0_T5_REQUEST_FORCE_CHARGE_1 (1 << 5)
#define M35C_B0_T5_REQUEST_FORCE_CHARGE_2 (1 << 4)
#define M35C_B0_T5_REQUEST_FULL_CHARGE (1 << 3)


uint8_t is_can_frame_received() {
  //if(digitalReadFast(PIN_PB2)) {
  //  return;
  //}
  return mcp2515.readMessage(&can_msg_readbuf) == MCP2515::ERROR_OK;
}

void can_data_print_request() {
  Serial.print(can_msg_readbuf.can_id, HEX); // print ID
  Serial.print(" ");
  Serial.print(can_msg_readbuf.can_dlc, HEX); // print DLC
  Serial.print(" ");

  for (int i = 0; i < can_msg_readbuf.can_dlc; i++)  { // print the data
    Serial.print(can_msg_readbuf.data[i], HEX);
    Serial.print(" ");
  }

  Serial.println();
}

void can_data_init() {
  struct35E_manuf_info->manuf_string[0] = 'B';
  struct35E_manuf_info->manuf_string[1] = 'Y';
  struct35E_manuf_info->manuf_string[2] = 'D';
  struct35E_manuf_info->manuf_string[3] = 0;
  struct35E_manuf_info->manuf_string[4] = 0;
  struct35E_manuf_info->manuf_string[5] = 0;
  struct35E_manuf_info->manuf_string[6] = 0;
  struct35E_manuf_info->manuf_string[7] = 0;

  struct382_product_info->product_string[0] = 'P';
  struct382_product_info->product_string[1] = 'R';
  struct382_product_info->product_string[2] = 'E';
  struct382_product_info->product_string[3] = 'M';
  struct382_product_info->product_string[4] = 'I';
  struct382_product_info->product_string[5] = 'U';
  struct382_product_info->product_string[6] = 'M';
  struct382_product_info->product_string[7] = 0;
  
  struct35F_battery_info->product_code[0] = 'L';
  struct35F_battery_info->product_code[0] = 'i';
  struct35F_battery_info->fw_version = 0x1701; // 01 17 = v01->17
  struct35F_battery_info->ah_available = 100;
  struct35F_battery_info->padding[0] = 0;
  struct35F_battery_info->padding[1] = 0;
  
  struct35A_warnings->flags[0] = 0xAA;
  struct35A_warnings->flags[1] = 0xAA;
  struct35A_warnings->flags[2] = 0xAA;
  struct35A_warnings->flags[3] = 0xAA;
  struct35A_warnings->flags[4] = 0xAA;
  struct35A_warnings->flags[5] = 0xAA;
  struct35A_warnings->flags[6] = 0xAA;
  struct35A_warnings->flags[7] = 0xAA;

  struct35B_events_unused->padding[0] = 0;
  struct35B_events_unused->padding[1] = 0;
  struct35B_events_unused->padding[2] = 0;
  struct35B_events_unused->padding[3] = 0;
  struct35B_events_unused->padding[4] = 0;
  struct35B_events_unused->padding[5] = 0;
  struct35B_events_unused->padding[6] = 0;
  struct35B_events_unused->padding[7] = 0;

  struct351_limits->pack_decivolts_hi = 525;
  struct351_limits->charge_limit_deciamps = 50;
  struct351_limits->discharge_limit_deciamps = 50;
  struct351_limits->pack_decivolts_lo = 470;

  struct355_soc_soh->soc_percent = 50;
  struct355_soc_soh->soh_percent = 99;
  struct355_soc_soh->padding[0] = 0;
  struct355_soc_soh->padding[1] = 0;
  struct355_soc_soh->padding[2] = 0;
  struct355_soc_soh->padding[3] = 0;

  struct356_major_measurements->pack_centivolts = 4800;
  struct356_major_measurements->pack_deciamps = 0;
  struct356_major_measurements->pack_temp_dc = 200;
  struct356_major_measurements->padding[0] = 0;
  struct356_major_measurements->padding[1] = 0;

  struct360_unknown0->padding[0] = 0;
  struct360_unknown0->padding[1] = 0;
  struct360_unknown0->padding[2] = 0;
  struct360_unknown0->padding[3] = 0;
  struct360_unknown0->padding[4] = 0;
  struct360_unknown0->padding[5] = 0;
  struct360_unknown0->padding[6] = 0;
  struct360_unknown0->padding[7] = 0;

  struct372_bank_info->batteries_online = 4;
  struct372_bank_info->batteries_block_charge = 0;
  struct372_bank_info->batteries_block_discharge = 0;
  struct372_bank_info->batteries_offline = 0;

  struct373_cell_info->lowest_cell_mv = 3200;
  struct373_cell_info->highest_cell_mv = 3200;
  struct373_cell_info->lowest_cell_temp_k = 293;
  struct373_cell_info->lowest_cell_temp_k = 293;

  struct374_cell_volt_min_id->cell_id[0] = '0';
  struct374_cell_volt_min_id->cell_id[1] = 0;
  struct374_cell_volt_min_id->cell_id[2] = 0;
  struct374_cell_volt_min_id->cell_id[3] = 0;
  struct374_cell_volt_min_id->cell_id[4] = 0;
  struct374_cell_volt_min_id->cell_id[5] = 0;
  struct374_cell_volt_min_id->cell_id[6] = 0;
  struct374_cell_volt_min_id->cell_id[7] = 0;
  
  struct375_cell_volt_max_id->cell_id[0] = '0';
  struct375_cell_volt_max_id->cell_id[1] = 0;
  struct375_cell_volt_max_id->cell_id[2] = 0;
  struct375_cell_volt_max_id->cell_id[3] = 0;
  struct375_cell_volt_max_id->cell_id[4] = 0;
  struct375_cell_volt_max_id->cell_id[5] = 0;
  struct375_cell_volt_max_id->cell_id[6] = 0;
  struct375_cell_volt_max_id->cell_id[7] = 0;
  
  struct376_cell_temp_min_id->cell_id[0] = '0';
  struct376_cell_temp_min_id->cell_id[1] = 0;
  struct376_cell_temp_min_id->cell_id[2] = 0;
  struct376_cell_temp_min_id->cell_id[3] = 0;
  struct376_cell_temp_min_id->cell_id[4] = 0;
  struct376_cell_temp_min_id->cell_id[5] = 0;
  struct376_cell_temp_min_id->cell_id[6] = 0;
  struct376_cell_temp_min_id->cell_id[7] = 0;

  struct377_cell_temp_max_id->cell_id[0] = '0';
  struct377_cell_temp_max_id->cell_id[1] = 0;
  struct377_cell_temp_max_id->cell_id[2] = 0;
  struct377_cell_temp_max_id->cell_id[3] = 0;
  struct377_cell_temp_max_id->cell_id[4] = 0;
  struct377_cell_temp_max_id->cell_id[5] = 0;
  struct377_cell_temp_max_id->cell_id[6] = 0;
  struct377_cell_temp_max_id->cell_id[7] = 0;

  struct378_history->charged_hWh = 0;
  struct378_history->discharged_hWh = 0;

  struct379_battery_size->installed_ah = 560;
  struct379_battery_size->padding[0] = 0;
  struct379_battery_size->padding[1] = 0;
  struct379_battery_size->padding[2] = 0;
  struct379_battery_size->padding[3] = 0;
  struct379_battery_size->padding[4] = 0;
  struct379_battery_size->padding[5] = 0;
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_1000KBPS);
  mcp2515.setNormalMode();
}


void can_data_update(Daly_BMS_UART *bms) {
  uint8_t i = 0;

  // MESSAGE 359 STATUS FLAGS

  // Byte 0 Protection/Critical 1
  i |= (bms->alarm.levelTwoDischargeTempTooHigh)     ? M35A_B0_A_CELL_OVERTEMP       : M35A_B0_A_CELL_OVERTEMP      << 1;
  i |= (bms->alarm.levelTwoCellVoltageTooLow
     || bms->alarm.levelTwoPackVoltageTooLow)        ? M35A_B0_A_CELL_UNDERVOLT      : M35A_B0_A_CELL_UNDERVOLT     << 1;
  i |= (bms->alarm.levelTwoCellVoltageTooHigh
     || bms->alarm.levelTwoPackVoltageTooHigh)       ? M35A_B0_A_CELL_OVERVOLT       : M35A_B0_A_CELL_OVERVOLT      << 1;
  i |= M35A_B0_A_UNUSED << 1;
  struct35A_warnings->flags[0] = i;

  i = 0;
  // Byte 1 Protection/Critical 2
  i |= (bms->alarm.levelTwoDischargeCurrentTooHigh)  ? M35A_B1_A_DISCH_OVERCURRENT   : M35A_B1_A_DISCH_OVERCURRENT  << 1;
  i |= (bms->alarm.levelTwoChargeTempTooLow)         ? M35A_B1_A_CHARGE_UNDERTEMP    : M35A_B1_A_CHARGE_UNDERTEMP   << 1;
  i |= (bms->alarm.levelTwoChargeTempTooHigh)        ? M35A_B1_A_CHARGE_OVERTEMP     : M35A_B1_A_CHARGE_OVERTEMP    << 1;
  i |= (bms->alarm.levelTwoDischargeTempTooLow)      ? M35A_B1_A_UNDERTEMP           : M35A_B1_A_UNDERTEMP          << 1;
  struct35A_warnings->flags[1] = i;

  i = 0;
  // Byte 2
  i |= (bms->alarm.levelTwoChargeCurrentTooHigh)     ? M35A_B2_A_CHARGE_OVERCURRENT  : M35A_B2_A_CHARGE_OVERCURRENT << 1;
  i |= M35A_B2_A_SYSTEM_ERROR << 1;
  i |= M35A_B2_A_UNUSED << 1;
  struct35A_warnings->flags[2] = i;

  i = 0;
  i |= (bms->alarm.levelTwoCellVoltageDifferenceTooHigh) ? M35A_B3_A_CELL_IMBAANCE   : M35A_B3_A_CELL_IMBAANCE      << 1;
  i |= M35A_B3_A_UNUSED << 1; 
  struct35A_warnings->flags[3] = i;

  i = 0;
  // Byte 4 Warning 1
  i |= (bms->alarm.levelOneDischargeTempTooHigh)     ? M35A_B4_W_CELL_OVERTEMP       : M35A_B4_W_CELL_OVERTEMP      << 1;
  i |= (bms->alarm.levelOneCellVoltageTooLow
     || bms->alarm.levelOnePackVoltageTooLow)        ? M35A_B4_W_CELL_UNDERVOLT      : M35A_B4_W_CELL_UNDERVOLT     << 1;
  i |= (bms->alarm.levelOneCellVoltageTooHigh
     || bms->alarm.levelOnePackVoltageTooHigh)       ? M35A_B4_W_CELL_OVERVOLT       : M35A_B4_W_CELL_OVERVOLT      << 1;
  i |= M35A_B4_W_UNUSED << 1;
  struct35A_warnings->flags[4] = i;

  i = 0;
  // Byte 5 Warning 2
  i |= (bms->alarm.levelOneDischargeCurrentTooHigh)  ? M35A_B5_W_DISCH_OVERCURRENT   : M35A_B5_W_DISCH_OVERCURRENT  << 1;
  i |= (bms->alarm.levelOneChargeTempTooLow)         ? M35A_B5_W_CHARGE_UNDERTEMP    : M35A_B5_W_CHARGE_UNDERTEMP   << 1;
  i |= (bms->alarm.levelOneChargeTempTooHigh)        ? M35A_B5_W_CHARGE_OVERTEMP     : M35A_B5_W_CHARGE_OVERTEMP    << 1;
  i |= (bms->alarm.levelOneDischargeTempTooLow)      ? M35A_B5_W_UNDERTEMP           : M35A_B5_W_UNDERTEMP          << 1;
  struct35A_warnings->flags[5] = i;

  i = 0;
  // Byte 6
  i |= (bms->alarm.levelOneChargeCurrentTooHigh)     ? M35A_B6_W_CHARGE_OVERCURRENT  : M35A_B6_W_CHARGE_OVERCURRENT << 1;
  i |= M35A_B6_W_SYSTEM_ERROR << 1;
  i |= M35A_B6_W_UNUSED << 1;
  struct35A_warnings->flags[6] = i;

  i = 0;
  i |= (bms->alarm.levelOneCellVoltageDifferenceTooHigh) ? M35A_B7_W_CELL_IMBAANCE   : M35A_B7_W_CELL_IMBAANCE      << 1;
  i |= M35A_B7_W_UNUSED << 1; 
  struct35A_warnings->flags[7] = i;
  

  // MESSAGE 355 BATTERY HEALTH
  struct355_soc_soh->soc_percent = bms->get.packSOC / 10;
  if (struct355_soc_soh->soc_percent < 11) {
    struct355_soc_soh->soc_percent = 11;
  }

  // MESSAGE 356 MEASUREMENTS
  struct356_major_measurements->pack_centivolts = 10 * bms->get.packVoltage;
  struct356_major_measurements->pack_deciamps = -bms->get.packCurrent;
  struct356_major_measurements->pack_temp_dc = bms->get.tempAverage * 10;
  struct356_major_measurements->padding[0] = 0;
  struct356_major_measurements->padding[1] = 0;


  struct351_limits->charge_limit_deciamps = get_charge_limit_deciamps(bms->get.maxCellmV, bms->get.packSOC, bms->get.tempAverage, bms->get.packCurrent);
  struct351_limits->discharge_limit_deciamps = get_discharge_limit_deciamps(bms->get.minCellmV, bms->get.packSOC, bms->get.tempAverage, bms->get.packCurrent);


//  373-378 remain to be populated
//  Maybe we can skip them?


// Charge requests and FET states are not sent??
/*
  i = 0;
  // MESSAGE 35C REQUESTS
  if (bms->get.chargeFetState) {
    i |= M35C_B0_T5_CHARGE_ENABLE;
  }
  if (bms->get.disChargeFetState) {
    i |= M35C_B0_T5_DISCH_ENABLE;
  }
  // We don't request force charges
  requests->flags = i;
  requests->padding = 0;
 */ 
}


void can_data_apply_overrides() {
    if(button_cancel_force_charge) {
      struct355_soc_soh->soc_percent = 100;
    }

    if(button_request_force_charge) {
      struct355_soc_soh->soc_percent = 9;
      //requests->flags |= M35C_B0_T5_REQUEST_FORCE_CHARGE_2;
    }
}


void can_debug(can_frame *fr) {
  Serial.print("CAN > ");
  Serial.print(fr->can_id, HEX);
  Serial.print(": ");
  for (int i = 0; i < fr->can_dlc; i++) {
    Serial.print(fr->data[i], HEX);
    Serial.print(" ");
  }
  Serial.print("\n");
}

void *can_message_transmit_sequence[17] =  {
  &bytes35E_manuf_info,
  &bytes382_product_info,
  &bytes35F_battery_info,
  &bytes35A_warnings,
  &bytes35B_events_unused,
  &bytes351_limits,
  &bytes355_soc_soh,
  &bytes356_major_measurements,
  &bytes360_unknown0,
  &bytes372_bank_info,
  &bytes373_cell_info,
  &bytes374_cell_volt_min_id,
  &bytes375_cell_volt_max_id,
  &bytes376_cell_temp_min_id,
  &bytes377_cell_temp_max_id,
  &bytes378_history,
  &bytes379_battery_size
};
const uint8_t num_canbus_elements = sizeof(can_message_transmit_sequence) / sizeof(void*);

void can_data_transmit() {
  for(int i = 0; i < num_canbus_elements; i++) {
    mcp2515.sendMessage((struct can_frame*) can_message_transmit_sequence[i]);
    //can_debug(&canMsg35C);
    //Serial.println("");
  }
  //Serial.println("Messages sent");
}
