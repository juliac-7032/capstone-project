#ifndef ETHERNET_MQTT_H
#define ETHERNET_MQTT_H

#include <Arduino.h>

void ethernet_mqtt_init();
void ethernet_mqtt_loop();
void mqtt_publish(const String& payload);

#endif