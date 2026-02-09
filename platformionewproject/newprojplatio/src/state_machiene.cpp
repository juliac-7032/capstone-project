#include "state_machine.h"
#include <Arduino.h>
#include <string.h>

// ESP32-C3 LEDC PWM config
static int g_redPin   = -1;
static int g_greenPin = -1;

static const int g_pwmFreq = 5000; // 5 kHz
static const int g_pwmRes  = 8;    // 8-bit => 0..255
static const int g_chRed   = 0;
static const int g_chGreen = 1;

static uint8_t g_dutyOn = 255;

static void pwm_write(int channel, uint8_t duty) {
  ledcWrite(channel, duty);
}

static void state_enter(state_e to) {
  switch (to) {
    case STATE_UNOCC:
      pwm_write(g_chGreen, g_dutyOn);
      pwm_write(g_chRed,   0);
      break;

    case STATE_OCC:
      pwm_write(g_chRed,   g_dutyOn);
      pwm_write(g_chGreen, 0);
      break;

    case STATE_MAINT:
      // Yellow = both on
      pwm_write(g_chRed,   g_dutyOn);
      pwm_write(g_chGreen, g_dutyOn);
      break;
  }
}

void state_machine_init(int red_pin, int green_pin, uint8_t duty_on) {
  g_redPin = red_pin;
  g_greenPin = green_pin;
  g_dutyOn = duty_on;

  pinMode(g_redPin, OUTPUT);
  pinMode(g_greenPin, OUTPUT);

  // Setup PWM channels
  ledcSetup(g_chRed,   g_pwmFreq, g_pwmRes);
  ledcSetup(g_chGreen, g_pwmFreq, g_pwmRes);

  ledcAttachPin(g_redPin,   g_chRed);
  ledcAttachPin(g_greenPin, g_chGreen);

  // Default boot state
  state_enter(STATE_UNOCC);
}

state_e state_machine_step(const JsonDocument& data, state_e current_state) {
  const char* overall = data["overall"] | "";

  state_e next = current_state;

  if (strcmp(overall, "occupied") == 0) {
    next = STATE_OCC;
  } else if (strcmp(overall, "unoccupied") == 0) {
    next = STATE_UNOCC;
  } else if (strcmp(overall, "maintenance") == 0) {
    next = STATE_MAINT;
  } else {
    // Fail-safe
    next = STATE_MAINT;
  }

  if (next != current_state) {
    state_enter(next);
  }

  return next;
}
