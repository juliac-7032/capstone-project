#include Arduino.h 

#define RED 6
#define GREEN 7

double Tms = 1.0; 
double duty = 0.1;

// put function declarations here:


void setup() {
  // put your setup code here, to run once:
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  Serial.begin(9600);
  
}

void loop() {
  // put your main code here, to run repeatedly:
 digitalWrite(RED, HIGH);
 digitalWrite(GREEN, HIGH);
 delay(Tms*duty);
 digitalWrite(RED, LOW);
 digitalWrite(GREEN, HIGH);
 delay(1.0 - (Tms*duty));
}




