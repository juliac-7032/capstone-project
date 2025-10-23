#include <Arduino.h>
#include <Adafruit_NeoPixel.h>


#define PIN 6


// put function declarations here:


void setup() {
  // put your setup code here, to run once:
  pinMode(PIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
 digitalWrite(PIN, HIGH);
 Serial.println("Pin is high");
 delay(10000);
 digitalWrite(PIN, LOW);
 Serial.println("Pin is low");
 delay(10000);

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



