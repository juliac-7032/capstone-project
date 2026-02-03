#include <Arduino.h>
//#include "light_colours.h"
#include <iostream>
#include <chrono> 
#include <ArduinoJson.h>

//pins
int red =  8; //name of pin on ESP32
int green = 9;


void setup() {
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  Serial.begin(9600);

}

void loop() {

  
  
  digitalWrite(red, HIGH);
  Serial.println("high");
  delay(1000);
  digitalWrite(red, LOW);
  Serial.println("low");
  delay(1000);

  
  
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



