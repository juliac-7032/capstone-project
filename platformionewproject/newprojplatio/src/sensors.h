
#include <Arduino.h>

#include <ArduinoJson.h>

void sensors_init(int pir_pin, int echo_pin, int trig_pin);
JsonDocument get_sensor_data();
static bool read_ultrasonic_occupied(float& distance_cm_out);



