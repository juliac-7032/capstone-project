#include <Arduino.h>
#include "ethernet_mqtt.h"
#include "sensors.h"

// ======== PIN CONFIG ========
static const int PIR_PIN  = 10;
static const int TRIG_PIN = 3;
static const int ECHO_PIN = 1;

// ======== PUBLISH RATE ========
static unsigned long g_lastPublishMs = 0;
static const unsigned long PUBLISH_INTERVAL_MS = 1000;

void setup()
{
  Serial.begin(115200);
  delay(200);

  sensors_init(PIR_PIN, ECHO_PIN, TRIG_PIN);
  ethernet_mqtt_init();
}

void loop()
{
  ethernet_mqtt_loop();

  unsigned long now = millis();
  if (now - g_lastPublishMs >= PUBLISH_INTERVAL_MS)
  {
    g_lastPublishMs = now;

    String payload;
    build_sensor_json(payload);

    Serial.println(payload);
    mqtt_publish(payload);
  }
}