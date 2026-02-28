#ifndef ETHERNET_MQTT_H
#define ETHERNET_MQTT_H

#include <Arduino.h>


static void ethernet_try_begin();
static void ethernet_retry_if_needed();
static void mqtt_connect_if_needed();
void ethernet_mqtt_init();
void ethernet_mqtt_loop();
void mqtt_publish(const String& payload);
void messageHandler(char* topic, byte* payload, unsigned int length);

#endif