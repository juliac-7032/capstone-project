#include "state_machine.h"

// ESP32-C3 LEDC PWM config
static int g_redPin   = -1;
static int g_greenPin = -1;

static const int g_pwmFreq = 5000; // 5 kHz
static const int g_pwmRes  = 8;    // 8-bit => 0..255
static const int g_chRed   = 0;
static const int g_chGreen = 1;

static uint8_t g_dutyOn = 100;

static void pwm_write(int channel, uint8_t duty) {
  ledcWrite(channel, duty);
}

static void state_enter(state_e to) {
  switch (to) {
    case STATE_UNOCC:
      pwm_write(g_chGreen, g_dutyOn);
      pwm_write(g_chRed, 0);
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

void state_machine_step(const JsonDocument& data) {
  const char* overall = data["overall"] | "";
  state_e curr = data["current"];

  state_e next = curr;

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

  if (next != curr) {
    state_enter(next);
  }

}
