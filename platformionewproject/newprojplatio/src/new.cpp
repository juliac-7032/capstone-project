#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

const char* ssid = "***";
const char* password = "*****";

//all of this will be our occupancy data
char jsonOutput[128]; //store the json we want to post
const char* key = "slay"; //words to put in the json object
const char* value = "all day";

// put function declarations here:


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi"); //connect to network

  while (WiFi.status() != WL_CONNECTED){ //check that we are connected
    Serial.print(".");
    delay(500);
  }

  Serial.println("\nConnected to WiFi"); //print out IP address
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); 
}

void loop() {
  // put your main code here, to run repeatedly:
  if((WiFi.status() == WL_CONNECTED)){ //double check that the network is connected still
    
    HTTPClient client; //instance of HTTPClient class

    client.begin("http://jsonplaceholder.typicode.com/posts"); //http that accepts posts (this is the API - we will have a different API)
    client.addHeader("Content-Type", "application/json"); //tell API we're sending jason
    
    const size_t CAPACITY = JSON_OBJECT_SIZE(1); //fixed capacity when we make a json document
    StaticJsonDocument<CAPACITY> doc; //create an empty json doc

    //the json placeholder API should take care of the id for you
    JsonObject object = doc.to<JsonObject>(); //put json object into json document
    object[key] = value; //writing to the json object (which is now in the document)

    serializeJson(doc, jsonOutput); //putting the doc we made (the doc has the object we made) into the jsonoutput char array we made

    int httpCode = client.POST(String(jsonOutput)); //the value of this number will tell you if it was successful

    if(httpCode > 0 ) {
      String payload = client.getString(); //get the json information
      Serial.println("\nStatuscode: " + String(httpCode)); //print whether it was successful
      Serial.println(payload); //print the json infromation

      client.end();
    }

  }
  else {
    Serial.println("connection lost");
  }

  delay(10000);

}