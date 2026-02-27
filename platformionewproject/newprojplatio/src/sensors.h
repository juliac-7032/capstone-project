#ifndef SENSORS_H
#define SENSORS_H

#include <ArduinoJson.h>

void sensors_init(int pir_pin, int echo_pin, int trig_pin);
void get_sensor_data(JsonDocument& doc);

#endif
