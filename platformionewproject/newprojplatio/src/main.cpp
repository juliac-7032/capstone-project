#include <Arduino.h>
#include "light_colours.h"
#include <iostream>
#include <chrono> 
#include <ArduinoJson.h>

int red =  6;
int green = 7;
int delay_time = 500;
double Tms = 1.0; 
double duty = 0.1;

enum State{unoccupied = 0, occupied= 1, maintenance =2};

void setup() {
  
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  Serial.begin(9600);
  
}

void loop() {

  const char* input = "{\"sensor state\": \"unoccupied\"}"; //json input
  JsonDocument doc; //craete document to hold the json
  deserializeJson(doc, input); //put the input into the JsonDocument
  
  //State current_state = doc["sensor state"]; //json will map to the state variable

  State current_state = occupied;

  switch(current_state){
    case unoccupied: green_light(Tms, duty, green);
                    Serial.println("green");
                    break;
    case occupied: red_light(Tms, duty, red);
                    Serial.println("red");
                    break;
    case maintenance: yellow_light(Tms, duty, red, green); 
                      Serial.println("yellow");                                
                      break;
    default: green_light(Tms, duty, green); 
             Serial.println("default");                           

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



