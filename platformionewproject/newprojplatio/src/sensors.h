#include <ArduinoJson.h>

JsonDocument get_sensor_data();

void init_sensors(int PIR_arg, int echo_arg, int trig_arg);