#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

void sensors_init(int pir_pin, int echo_pin, int trig_pin);
void build_sensor_json(String& out);

#endif