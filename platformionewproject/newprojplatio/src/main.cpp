#include <Arduino.h>
#include "light_colours.h"
#include <iostream>
#include <ArduinoJson.h>
#include "sensors.h"
#include <string>
#include "control_firmware\state_machine.h"
using namespace std;

//TODO put all the functions in a header so that we don't have to mess around with the order of everything

//pins
int PIR_pin = 10; //name of pin on ESP32 (silkscreen and GPIO number)
int echo_pin = 1; 
int trig_pin = 3;
bool US;
bool PIR;
int red; //pins
int green;

//duty cycle 
uint8_t duty; //0 to 255
uint8_t off = 0;

//sensor vars
int PIR_val = 0;
int echo_val = 0;
int trig_val;
float duration, distance; 

//ctrl vars
state_e curr;

// FUNCTIONS:

struct state_transition state_machine_init() { //inital transistion is unocc and event none
    struct state_transition initial_state;
    initial_state.from = STATE_UNOCC;
    initial_state.to = STATE_UNOCC;
    return initial_state;
}

//funtion that enters new state
static void state_enter(state_e to) {
    //we will have functions for entering each state
    //Analog writing from here directly would be weird?
    switch(to){
        case STATE_UNOCC:
            analogWrite(green, duty);
            analogWrite(red, off);
            Serial.println("green");
            break;
        case STATE_OCC:
            analogWrite(red, duty);
            analogWrite(green, off);
            Serial.println("red");
            break;
        case STATE_MAINT:
            analogWrite(red, duty);
            analogWrite(green, duty);
            Serial.println("yellow");
            break;
    }
    return;
}

state_e process_input(JsonDocument data, state_e current_state) { //input is a json document
  /*
 
  */
  
  serializeJson(data, Serial);
  

  state_e new_state; 
  if(data["overall"] == "occupied"){
    new_state = STATE_OCC;
    Serial.print("occupied ");
    Serial.println(new_state);
  } 
  else if(data["overall"] == "unoccupied"){
    new_state = STATE_UNOCC;
    Serial.print("unoccupied ");
    Serial.println(new_state);
  }
  else if(data["overall"] == "maintenance"){
    new_state = STATE_MAINT;
    Serial.print("maintenance ");
    Serial.println(new_state);
  }
    
  //FIXME should probably have an else where we return an error
  
  if(new_state != current_state){
    Serial.println("entering new state");
    state_enter(new_state);
    return(new_state);
  }
  else 
    Serial.println("the state is the same");
    return current_state;
  


}




void setup() {
  
  pinMode(PIR_pin, INPUT); 
  pinMode(echo_pin, INPUT);
  pinMode(trig_pin, OUTPUT);
  Serial.begin(9600);
  struct state_transition initial_state = state_machine_init();
  Serial.print("initial state: ");
  Serial.println(initial_state.to);
  curr = process_input(get_sensor_data(), initial_state.to);
  Serial.print("curr: ");
  Serial.println(initial_state.to);
  Serial.println("setup successful");

}

void loop() {
  

  curr = process_input(get_sensor_data(), curr);
  
  delay(1000);

}





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



