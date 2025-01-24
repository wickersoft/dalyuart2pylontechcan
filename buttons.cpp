#include "arduino.h"

#include "buttons.h"

uint8_t button_request_force_charge = 0;
uint8_t button_cancel_force_charge  = 0;


void buttons_init() {
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
}

void buttons_update_physical() {
  if (!digitalRead(A0)) {
    button_cancel_force_charge = 60;
    button_request_force_charge = 0;
  }
  if (!digitalRead(A1)) {
    button_request_force_charge = 60;
    button_cancel_force_charge = 0;
  }
}

void buttons_update_serial() {
  uint8_t c = 0;

  if (!Serial.available()) {
    return;
  }

  button_request_force_charge = 0;
  button_cancel_force_charge  = 0;

  while (Serial.available()) {
    c = Serial.read();
  }

  switch (c) {
    case '1':
    case '\n':
      button_cancel_force_charge = 60;
      button_request_force_charge = 0;
      break;
    case '0':
      button_request_force_charge = 60;
      button_cancel_force_charge = 0;
      break;
  }
}

void buttons_update() {
  if (button_request_force_charge) {
    button_request_force_charge--;
  }
  if (button_cancel_force_charge) {
    button_cancel_force_charge--;
  }

  buttons_update_serial();
  buttons_update_physical();
}
