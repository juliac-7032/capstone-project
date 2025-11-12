#include <Arduino.h>
#include "light_colours.h"


int red =  6;
int green = 7;
int delay_time = 500;
double Tms = 1000; 
double duty = 0.9;

void setup() {
  
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  Serial.begin(9600);
  
}

void loop() {
  





  
  red_light(Tms, duty, red);
  delay(delay_time);
  
  red_light(Tms, duty, red);
  delay(delay_time);

  red_light(Tms, duty, red);
  delay(delay_time);

  yellow_light(Tms, duty, red, green);
  delay(delay_time);

  yellow_light(Tms, duty, red, green);
  delay(delay_time);

  yellow_light(Tms, duty, red, green);
  delay(delay_time);

  green_light(Tms, duty, green);
  delay(delay_time);

  green_light(Tms, duty, green);
  delay(delay_time);

  green_light(Tms, duty, green);
  delay(delay_time);
}

// put function definitions here:


//pseudocode
//read microwave data


//1.
//create json objects to store the data
//"occupied": yes
//post json to our API - let software take it from there

//2. (could be at the same time or we could read the json that just got created)
//if occupied
//  turn the light red (change red pwm to 100 and the others to 0)
//else if available
//  turn the light green (green duty cycle)
//else if cleaning ****for the cleaning should we do this as a schedule (we take info from the API that it's scheduled for maintenance)?
//  turn yellow (red 50, green 50)



