#include "sensors.h"
#include <Arduino.h>

static int g_pirPin  = -1;
static int g_echoPin = -1;
static int g_trigPin = -1;

static const float SPEED_OF_SOUND_CM_PER_US = 0.0343f;
static const float OCCUPIED_DISTANCE_CM = 800.0f;
static const unsigned long PULSE_TIMEOUT_US = 30000; // 30ms

void sensors_init(int pir_pin, int echo_pin, int trig_pin) {
  g_pirPin  = pir_pin;
  g_echoPin = echo_pin;
  g_trigPin = trig_pin;

  pinMode(g_pirPin, INPUT);
  pinMode(g_echoPin, INPUT);
  pinMode(g_trigPin, OUTPUT);

  digitalWrite(g_trigPin, LOW);
}

static bool read_ultrasonic(float& distance_cm_out) {
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

  distance_cm_out = (duration * SPEED_OF_SOUND_CM_PER_US) / 2.0f;
  return true;
}

void get_sensor_data(JsonDocument& doc) {
  doc.clear();
  doc["id"] = "001";

  bool pirOcc = (digitalRead(g_pirPin) == HIGH);
  doc["PIR"] = pirOcc ? "occupied" : "unoccupied";

  float dist = 0.0f;
  bool usValid = read_ultrasonic(dist);
  bool usOcc = usValid && (dist < OCCUPIED_DISTANCE_CM);

  doc["US"] = usOcc ? "occupied" : "unoccupied";
  doc["distance_cm"] = dist;

  doc["overall"] = (pirOcc || usOcc) ? "occupied" : "unoccupied";
}