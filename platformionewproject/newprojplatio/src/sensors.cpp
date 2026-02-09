#include "sensors.h"
#include <Arduino.h>




static int g_pirPin  = -1;
static int g_echoPin = -1;
static int g_trigPin = -1;

static const float SPEED_OF_SOUND_CM_PER_US = 0.0343f;
static const float OCCUPIED_DISTANCE_CM = 800.0f;     // your threshold
static const unsigned long PULSE_TIMEOUT_US = 30000;  // 30ms timeout

void sensors_init(int pir_pin, int echo_pin, int trig_pin) {
  g_pirPin  = pir_pin;
  g_echoPin = echo_pin;
  g_trigPin = trig_pin;

  pinMode(g_pirPin, INPUT);
  pinMode(g_echoPin, INPUT);
  pinMode(g_trigPin, OUTPUT);

  digitalWrite(g_trigPin, LOW);
}

static bool read_ultrasonic_occupied(float& distance_cm_out) {
  // Trigger
  digitalWrite(g_trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(g_trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(g_trigPin, LOW);

  unsigned long duration = pulseIn(g_echoPin, HIGH, PULSE_TIMEOUT_US);
  if (duration == 0) {
    distance_cm_out = 99999.0f;
    return false;
  }

  float distance_cm = (duration * SPEED_OF_SOUND_CM_PER_US) / 2.0f;
  distance_cm_out = distance_cm;

  return (distance_cm < OCCUPIED_DISTANCE_CM);
}

void get_sensor_data(JsonDocument& doc) {
  doc.clear();
  doc["id"] = "001";

  bool pir = (digitalRead(g_pirPin) == HIGH);

  float distance_cm = 0.0f;
  bool us = read_ultrasonic_occupied(distance_cm);

  doc["PIR"] = pir ? "occupied" : "unoccupied";
  doc["US"]  = us  ? "occupied" : "unoccupied";
  doc["distance_cm"] = distance_cm;

  // Overall = PIR OR US
  doc["overall"] = (pir || us) ? "occupied" : "unoccupied";
}
