#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <ArduinoJson.h>
#include <stdint.h>
#include <Arduino.h>
#include <string.h>

typedef enum {
  STATE_UNOCC = 0,
  STATE_OCC   = 1,
  STATE_MAINT = 2
} state_e;




static void pwm_write(int channel, uint8_t duty);

static void state_enter(state_e to);

// Configure LED pins + brightness (0..255)
void state_machine_init(int red_pin, int green_pin, uint8_t duty_on);

// Step state machine using JSON input. Updates LEDs only on state change.
void state_machine_step(const JsonDocument& data);

#endif