#include <Arduino.h>
#include <Adafruit_NeoPixel.h>




#define PIN        8 //builtin LED pin
#define NUMPIXELS 16
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 20


// put function declarations here:




void setup() {
  // put your setup code here, to run once:


  pixels.begin();
}


void loop() {
  // put your main code here, to run repeatedly:
  pixels.clear();


  for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(150, 0, 0));
    pixels.show();
    delay(DELAYVAL);
  }
 
    for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(100, 50, 0));
    pixels.show();
    delay(DELAYVAL);
  }
 


  for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(75, 75, 0));
    pixels.show();
    delay(DELAYVAL);
  }
 
  for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(50, 100, 0));
    pixels.show();
    delay(DELAYVAL);
  }


  for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 150, 0));
    pixels.show();
    delay(DELAYVAL);
  }
 
  for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 100, 50));
    pixels.show();
    delay(DELAYVAL);
  }


  for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 75, 75));
    pixels.show();
    delay(DELAYVAL);
  }
 
    for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 50, 100));
    pixels.show();
    delay(DELAYVAL);
  }


  for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 150));
    pixels.show();
   
  }
 
  for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(50, 0, 100));
    pixels.show();
    delay(DELAYVAL);
  }


  for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(75, 0, 75));
    pixels.show();
    delay(DELAYVAL);
  }


    for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(100, 0, 50));
    pixels.show();
    delay(DELAYVAL);
  }
 
 
}




