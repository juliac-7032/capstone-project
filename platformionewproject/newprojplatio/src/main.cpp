#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
#include "state_machine.h"
#include "sensors.h"


#define AWS_IOT_PUBLISH_TOPIC   "topics/occupancy"
#define AWS_IOT_SUBSCRIBE_TOPIC "devices/001/mode"


// ======== PIN CONFIG ========
int PIR_PIN  = 10;
int TRIG_PIN = 3;
int ECHO_PIN = 1;

int RED_PIN = 8;
int GREEN_PIN = 9;
uint8_t duty = 100; 




WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

void messageHandler(char* topic, byte* payload, unsigned int length)
{
  Serial.print("incoming: ");
  Serial.println(topic);
  JsonDocument doc;         
  deserializeJson(doc, payload); 
  state_machine_step(doc); //take in the doc and step state machine. 
}

void print_incoming(JsonDocument data){
  serializeJson(data, Serial);
}

void connectAWS()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(WiFi.status());
  }

  //wifi stutus interpretation
  /*WL_NO_SHIELD        = 255,   // for compatibility with WiFi Shield library
    WL_IDLE_STATUS      = 0,
    WL_NO_SSID_AVAIL    = 1,
    WL_SCAN_COMPLETED   = 2,
    WL_CONNECTED        = 3,
    WL_CONNECT_FAILED   = 4,
    WL_CONNECTION_LOST  = 5,
    WL_DISCONNECTED     = 6   */  

  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(AWS_IOT_ENDPOINT, 8883);

  // Create a message handler
  client.setCallback(messageHandler);

  Serial.println("Connecting to AWS IOT");

  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(100);
  }

  if (!client.connected())
  {
    Serial.println("AWS IoT Timeout!");
    return;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT Connected!");
}

void publishMessage() //sending occ data
{
  JsonDocument doc;//function
  doc = get_sensor_data();

  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client

  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}


void setup()
{
  Serial.begin(9600);
  delay(200);
  sensors_init(PIR_PIN, ECHO_PIN, TRIG_PIN);
  state_machine_init(RED_PIN, GREEN_PIN, duty);
  connectAWS();
  
}

void loop()
{
  

  //Serial.println("in loop");
  //publishMessage();

  client.loop();
  delay(1000);
}