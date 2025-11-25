#include <Arduino.h>
#include "light_colours.h"
#include <iostream>
#include <chrono> 
#include <ArduinoJson.h>

//pins
int red =  6; //name of pin on ESP32
int green = 7;
int PIR = 2;
int echo_pin = 8;
int trig_pin = 9;

//sensor vars
int PIR_val = 0;
int echo_val = 0;
int trig_val;
float duration, distance; 

//light -related vars
double Tms = 1.0; 
double duty = 0.1;

void setup() {
  
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(PIR, INPUT); 
  pinMode(echo_pin, INPUT);
  pinMode(trig_pin, OUTPUT);
  Serial.begin(9600);
  
}

void loop() {
  
  /* US testing code
    digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);

  duration = pulseIn(echo_pin, HIGH);
  distance = (duration*.0343)/2;
  Serial.print("Distance: ");
  Serial.println(distance);
  delay(100);
  */

  
  /*   

  */
  PIR_val = digitalRead(PIR);
  if(PIR_val == HIGH){ //high on pir pin means motion detected
    red_light(Tms, duty, red);
  }

  else{
    green_light(Tms, duty, green);
  }

  
  
  
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



