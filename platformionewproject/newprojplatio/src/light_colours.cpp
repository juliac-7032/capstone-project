#include <Arduino.h>
#include "light_colours.h"

void yellow_light(double Tms, double duty, int red, int green) {
 digitalWrite(red, HIGH);
 digitalWrite(green, HIGH);
 delay(Tms*duty);
 digitalWrite(red, LOW);
 digitalWrite(green, LOW);
 delay(1.0 - (Tms*duty));
}

void green_light(double Tms, double duty, int green) {
 digitalWrite(green, HIGH);
 delay(Tms*duty);
 digitalWrite(green, LOW);
 delay(1.0 - (Tms*duty));
}

void red_light(double Tms, double duty, int red) {
 digitalWrite(red, HIGH);
 delay(Tms*duty);
 digitalWrite(red, LOW);
 delay(1.0 - (Tms*duty));
}






