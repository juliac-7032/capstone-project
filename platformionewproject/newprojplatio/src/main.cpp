#include <Arduino.h>
#include <ArduinoJson.h>

#include "ethernet_mqtt.h"
#include "sensors.h"
#include "state_machine.h"

// ======== PIN CONFIG ========
static const int PIR_PIN   = 10;
static const int TRIG_PIN  = 3;
static const int ECHO_PIN  = 1;

static const int RED_PIN   = 8;
static const int GREEN_PIN = 9;

// ======== PUBLISH RATE ========
static unsigned long g_lastPublishMs = 0;
static const unsigned long PUBLISH_INTERVAL_MS = 1000;

// LED state machine
static state_e g_state = STATE_UNOCC;

void setup() {
  Serial.begin(115200);
  delay(200);

  sensors_init(PIR_PIN, ECHO_PIN, TRIG_PIN);

  // LED PWM init (0..255 brightness)
  state_machine_init(RED_PIN, GREEN_PIN, 255);

  ethernet_mqtt_init();
}

void loop() {
  ethernet_mqtt_loop();

  const unsigned long now = millis();
  if (now - g_lastPublishMs >= PUBLISH_INTERVAL_MS) {
    g_lastPublishMs = now;

    // 1) Read sensors into JSON
    StaticJsonDocument<256> doc;
    get_sensor_data(doc);

    // 2) Drive LEDs based on doc["overall"]
    g_state = state_machine_step(doc, g_state);

    // 3) Serialize once (for Serial + MQTT)
    String payload;
    serializeJson(doc, payload);

    Serial.println(payload);
    mqtt_publish(payload);
  }
}