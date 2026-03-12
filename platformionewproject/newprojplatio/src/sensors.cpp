#include "sensors.h"
#include <chrono>
#include <ctime>
#include <cstdio>

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

JsonDocument get_sensor_data() {
  JsonDocument doc;
  doc.clear();
  doc["id"] = "001";

   // Format timestamp as ISO 8601-like string with microseconds
  auto now = std::chrono::system_clock::now();
  auto now_us = std::chrono::time_point_cast<std::chrono::microseconds>(now);
  auto epoch = now_us.time_since_epoch();
  long long micros = std::chrono::duration_cast<std::chrono::microseconds>(epoch).count();
  std::time_t tt = std::chrono::system_clock::to_time_t(now);
  std::tm tm = *std::localtime(&tt);
  char buffer[32];
  sprintf(buffer, "%04d-%02d-%02dT%02d:%02d:%02d.%06lld", 
          tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, 
          tm.tm_hour, tm.tm_min, tm.tm_sec, micros % 1000000);
  doc["timestamp"] = buffer;




  bool pir = (digitalRead(g_pirPin) == HIGH);

  float distance_cm = 0.0f;
  bool us = read_ultrasonic_occupied(distance_cm);

  //doc["PIR"] = pir ? "occupied" : "unoccupied";
  //doc["US"]  = us  ? "occupied" : "unoccupied";
  //doc["distance_cm"] = distance_cm;

  // Overall = PIR OR US
  doc["occupied"] = (pir || us) ? "occupied" : "unoccupied";

}
