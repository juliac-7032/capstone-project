#include <Arduino.h>
#include <Adafruit_NeoPixel.h>




#define PIN        8
#define NUMPIXELS 16
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 20


// put function declarations here:




void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  pixels.begin();
}


void loop() {
  // put your main code here, to run repeatedly:
  pixels.clear();

    for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    pixels.show();
    delay(DELAYVAL);
  }

  Serial.println("cleared");

  delay(1000);
 
 
}



