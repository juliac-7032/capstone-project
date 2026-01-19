#include <Arduino.h>
#include "light_colours.h"
#include <iostream>
#include <chrono> 
#include <ArduinoJson.h>

//pins
int red =  6; //name of pin on ESP32
int green = 7;


//sensor vars


int add(int val1, int val2){
  return val1+val2;
}

//light -related vars
double Tms = 1.0; 
double duty = 0.1;

void setup() {
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  Serial.begin(9600);

}

void loop() {
  
  analogWrite(red, 255);
  delay(10000);

  int value_2 = 5;
  int value_1 = 10;
  while(1){
    Serial.println(add(value_2, value_1));
  }
  
 
  
}

// put function definitions here:


//pseudocode

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



