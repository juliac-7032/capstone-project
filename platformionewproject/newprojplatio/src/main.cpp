#include <Arduino.h>
#include "light_colours.h"
#include <iostream>
#include <chrono> 
#include <ArduinoJson.h>

//pins
int red =  6; //name of pin on ESP32 (silkscreen and GPIO number)
int green = 7;
int PIR_pin = 10; 
int echo_pin = 1; 
int trig_pin = 3;
bool US;
bool PIR;

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
  pinMode(PIR_pin, INPUT); 
  pinMode(echo_pin, INPUT);
  pinMode(trig_pin, OUTPUT);
  Serial.begin(9600);
  
}

void loop() {
  
  /* US testing code
  //for right now, I am going to define a distance of less than 40cm as "occupied" so that we can easily test it on my desk with the measuring tape
 
  */

  digitalWrite(trig_pin, LOW);  
	delayMicroseconds(2);  
	digitalWrite(trig_pin, HIGH);  
	delayMicroseconds(10);  
	digitalWrite(trig_pin, LOW);  

  duration = pulseIn(echo_pin, HIGH); //time in microseconds
  distance = (duration*.0343)/2; //distance in cm 0.0343 is speed of sound in cm per microsecond
  if(distance < 40){
    US = true;
    Serial.println("US: occupied");
  }
  else{
    Serial.println("US: unoccupied");
    US = false;
  }
  
  /*   
 
  */

 PIR_val = digitalRead(PIR_pin);
  if(PIR_val == HIGH){ //high on pir pin means motion detected
    Serial.println("PIR: occupied");
    PIR = true; 
  }
  else{
    Serial.println("PIR: unoccupied");
    PIR = false; 
  }

//logic
if(PIR)
    Serial.println("occupied");
  else
    if(US)
      Serial.println("occupied");
    else
      Serial.println("unoccupied");

  delay(1000);
  
  
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



