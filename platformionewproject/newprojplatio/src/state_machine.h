#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <ArduinoJson.h>
#include <stdint.h>

typedef enum {
  STATE_UNOCC = 0,
  STATE_OCC   = 1,
  STATE_MAINT = 2
} state_e;

// Configure LED pins + brightness (0..255)
void state_machine_init(int red_pin, int green_pin, uint8_t duty_on);

// Step state machine using JSON input. Updates LEDs only on state change.
state_e state_machine_step(const JsonDocument& data, state_e current_state);

#endif
