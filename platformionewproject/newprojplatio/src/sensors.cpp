#include <ArduinoJson.h>
#include <Arduino.h>

int _PIR_pin; 
int _echo_pin; 
int _trig_pin;
bool _US;
bool _PIR;

//sensor vars
int _PIR_val = 0;
int _echo_val = 0;
int _trig_val;
float _duration, _distance; 

void get_sensor_data() {
JsonDocument _sensor_data;
  _sensor_data["id"] = "001"; 

  digitalWrite(_trig_pin, LOW);  
	delayMicroseconds(2);  
	digitalWrite(_trig_pin, HIGH);  
	delayMicroseconds(10);  
	digitalWrite(_trig_pin, LOW);  

  _duration = pulseIn(_echo_pin, HIGH); //time in microseconds
  _distance = (_duration*.0343)/2; //_distance in cm 0.0343 is speed of sound in cm per microsecond
  if(_distance < 800){
    _US = true;
    
    _sensor_data["US"] = "occupied";
  }
  else{
    
    _sensor_data["US"] = "unoccupied";
    _US = false;
  }

 _PIR_val = digitalRead(_PIR_pin);
  if(_PIR_val == HIGH){ //high on pir pin means motion detected
    _sensor_data["PIR"] = "occupied";
    _PIR = true; 
  }
  else{
    _sensor_data["PIR"] = "unoccupied";
    _PIR = false; 
  }

//logic
if(_PIR)
    _sensor_data["overall"] = "occupied";
  else
    if(_US)
      
      _sensor_data["overall"] = "occupied";
    else
      
      _sensor_data["overall"] = "unoccupied";
  
  serializeJson(_sensor_data, Serial); //final serial print
  Serial.println(" ");
}
 