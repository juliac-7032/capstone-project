#include "ethernet_mqtt.h"
#include <ArduinoJson.h> 
#include <SPI.h>
#include <Ethernet_Generic.h>
#include <PubSubClient.h>


// ======== W5500 CONFIG ========
static const int W5500_CS  = 7;
static const int W5500_RST = -1;

// ESP32-C3 remapped SPI
static const int SPI_SCK  = 4;
static const int SPI_MISO = 6;
static const int SPI_MOSI = 5;

// Unique MAC address
static byte mac[] = { 0x02, 0x12, 0x34, 0x56, 0x78, 0x9A };

// ======== MOSQUITTO BROKER CONFIG ========
// CHANGE THIS TO YOUR BROKER IP
static IPAddress MQTT_HOST(192, 168, 1, 10);
static const uint16_t MQTT_PORT = 1883;

static const char* MQTT_CLIENT_ID = "occupancy-001";
static const char* MQTT_PUB_TOPIC = "topics/occupancy";

EthernetClient ethClient;
PubSubClient mqtt(ethClient);

static bool g_ethOk = false;

static unsigned long g_lastEthRetryMs = 0;
static const unsigned long ETH_RETRY_MS = 5000;

static unsigned long g_lastMqttRetryMs = 0;
static const unsigned long MQTT_RETRY_MS = 3000;

// ================= ETHERNET =================
static void ethernet_try_begin()
{
  Serial.println("Starting Ethernet...");

  if (Ethernet.begin(mac) == 0)
  {
    Serial.println("DHCP failed — using static IP");
    IPAddress ip(192,168,1,50);
    IPAddress dns(192,168,1,1);
    IPAddress gw(192,168,1,1);
    IPAddress mask(255,255,255,0);
    Ethernet.begin(mac, ip, dns, gw, mask);
  }

  EthernetHardwareStatus hw = Ethernet.hardwareStatus();
  IPAddress ip = Ethernet.localIP();

  Serial.print("HW: ");
  Serial.println(hw == EthernetNoHardware ? "NO_HARDWARE" : "OK");

  Serial.print("IP: ");
  Serial.println(ip);

  g_ethOk = (hw != EthernetNoHardware) && (ip[0] != 0);
}

static void ethernet_retry_if_needed()
{
  if (g_ethOk) return;

  unsigned long now = millis();
  if (now - g_lastEthRetryMs < ETH_RETRY_MS) return;

  g_lastEthRetryMs = now;
  Serial.println("Ethernet retry...");
  ethernet_try_begin();
}

// ================= MQTT =================
static void mqtt_connect_if_needed()
{
  if (!g_ethOk) return;
  if (mqtt.connected()) return;

  unsigned long now = millis();
  if (now - g_lastMqttRetryMs < MQTT_RETRY_MS) return;

  g_lastMqttRetryMs = now;

  Serial.print("MQTT connect... ");
  if (mqtt.connect(MQTT_CLIENT_ID))
  {
    Serial.println("OK");
  }
  else
  {
    Serial.print("FAIL rc=");
    Serial.println(mqtt.state());
  }
}

void mqtt_publish(const String& payload)
{
  if (mqtt.connected())
  {
    mqtt.publish(MQTT_PUB_TOPIC, payload.c_str());
  }
}

void ethernet_mqtt_init()
{
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, W5500_CS);
  Ethernet.init(W5500_CS);

  if (W5500_RST >= 0)
  {
    pinMode(W5500_RST, OUTPUT);
    digitalWrite(W5500_RST, LOW);
    delay(50);
    digitalWrite(W5500_RST, HIGH);
    delay(200);
  }

  ethernet_try_begin();

  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  mqtt.setCallback(messageHandler);
}

void messageHandler(char* topic, byte* payload, unsigned int length)
{
  Serial.print("incoming: ");
  Serial.println(topic);
  JsonDocument doc;
  StaticJsonDocument<256> doc;          
  deserializeJson(doc, payload, length); //FIXME what does the json doc look like right now? We need to read doc["current state"]
  //get the current state from the message as state_e
  state_e curr = doc["current"];


  state_machine_step(doc, curr);
  //use example code from repo - put the output next into pointer and send the current state (the next is the current)
  
  //send the next state return var (send to the AWS that the state in fact changed)

  //put return into a json doc to say "state changed to: ____"

}



void ethernet_mqtt_loop()
{
  ethernet_retry_if_needed();
  mqtt_connect_if_needed();

  mqtt.loop();
  Ethernet.maintain();
}