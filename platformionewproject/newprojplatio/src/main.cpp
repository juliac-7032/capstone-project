#include <Arduino.h>
#include <ArduinoJson.h>

#include <SPI.h>
#include <Ethernet_Generic.h>

#include "sensors.h"
#include "state_machine.h"

// ===================== PIN CONFIG =====================
// CHANGE THESE TO MATCH YOUR WIRING

// W5500
static const int W5500_CS  = 7;     // Chip Select
static const int W5500_RST = -1;    // Reset pin, or -1 if not wired

// SPI pins (ESP32-C3 supports remap)
static const int SPI_SCK  = 4;
static const int SPI_MISO = 6;
static const int SPI_MOSI = 5;

// Sensors
static const int PIR_PIN  = 10;
static const int TRIG_PIN = 3;
static const int ECHO_PIN = 1;

// LEDs (safe even if not connected)
static const int RED_PIN   = 8;
static const int GREEN_PIN = 9;
// ======================================================

// MAC address (must be unique on LAN)
static byte mac[] = { 0x02, 0x12, 0x34, 0x56, 0x78, 0x9A };

// State machine
static state_e g_state = STATE_UNOCC;

// ---------- Ethernet status ----------
static bool g_ethOk = false;              // "network usable"
static bool g_linkOn = false;             // cable/link state
static unsigned long g_lastEthRetryMs = 0;
static const unsigned long ETH_RETRY_MS = 5000; // retry Ethernet every 5s if failed

// ---------- JSON publish cache ----------
static String g_lastJson = "{}";
static unsigned long g_lastPublishMs = 0;

// JSON rates
static const unsigned long JSON_PERIOD_OK_MS   = 1000; // 1s when Ethernet is OK
static const unsigned long JSON_PERIOD_FAIL_MS = 200;  // 200ms when Ethernet fails (change as you want)

// ------------------------------------------------------
// EthernetServer shim (fixes ESP32 abstract Server issue)
// ------------------------------------------------------
class EthernetServerFixed : public EthernetServer {
public:
  explicit EthernetServerFixed(uint16_t port) : EthernetServer(port) {}
  void begin(uint16_t port = 0) override {
    (void)port;
    EthernetServer::begin();
  }
};

static EthernetServerFixed server(80);

// -------------------- HTML PAGE ------------------------
static String index_html() {
  String html;
  html += "<!doctype html><html><head><meta charset='utf-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<title>Occupancy Monitor</title></head><body>";
  html += "<h2>Smart Occupancy Monitor</h2>";
  html += "<pre id='out'>Loading...</pre>";
  html += "<script>";
  html += "async function tick(){";
  html += " try{";
  html += "  const r=await fetch('/data');";
  html += "  const j=await r.json();";
  html += "  document.getElementById('out').textContent=JSON.stringify(j,null,2);";
  html += " }catch(e){document.getElementById('out').textContent='Error';}";
  html += "}";
  html += "tick(); setInterval(tick,1000);";
  html += "</script></body></html>";
  return html;
}

// -------------------- HTTP HELPERS ---------------------
static void send_http(EthernetClient &client,
                      const char* contentType,
                      const String& body) {
  client.println("HTTP/1.1 200 OK");
  client.println("Connection: close");
  client.print("Content-Type: ");
  client.println(contentType);
  client.print("Content-Length: ");
  client.println(body.length());
  client.println();
  client.print(body);
}

static void send_404(EthernetClient &client) {
  client.println("HTTP/1.1 404 Not Found");
  client.println("Connection: close");
  client.println("Content-Type: text/plain");
  client.println();
  client.println("404 Not Found");
}

// -------------------- REQUEST HANDLER ------------------
static void handle_client() {
  EthernetClient client = server.available();
  if (!client) return;

  String reqLine = client.readStringUntil('\n');
  reqLine.trim();

  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r" || line.length() == 0) break;
  }

  String path = "/";
  int s1 = reqLine.indexOf(' ');
  int s2 = reqLine.indexOf(' ', s1 + 1);
  if (s1 >= 0 && s2 > s1)
    path = reqLine.substring(s1 + 1, s2);

  if (path == "/" || path.startsWith("/index")) {
    send_http(client, "text/html; charset=utf-8", index_html());
  }
  else if (path == "/data") {
    // Return the most recent JSON snapshot
    send_http(client, "application/json", g_lastJson);
  }
  else {
    send_404(client);
  }

  delay(1);
  client.stop();
}

// -------------------- ETHERNET INIT / RETRY ------------
static void ethernet_try_begin() {
  // Update link status
  g_linkOn = (Ethernet.linkStatus() == LinkON);

  // If no link, we consider Ethernet not OK
  if (!g_linkOn) {
    g_ethOk = false;
    return;
  }

  // Link is on: try DHCP first, fallback to static
  Serial.println("Starting Ethernet DHCP...");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("DHCP failed — using static IP");
    IPAddress ip(192, 168, 1, 50);
    IPAddress dns(192, 168, 1, 1);
    IPAddress gw(192, 168, 1, 1);
    IPAddress mask(255, 255, 255, 0);
    Ethernet.begin(mac, ip, dns, gw, mask);
  }

  IPAddress ip = Ethernet.localIP();
  g_ethOk = (ip[0] != 0); // crude but effective: 0.0.0.0 means no IP

  Serial.print("IP: ");
  Serial.println(ip);

  Serial.print("Link: ");
  Serial.println(g_linkOn ? "ON" : "OFF");

  if (g_ethOk) {
    server.begin();
    Serial.println("================================");
    Serial.print("Web:   http://");
    Serial.print(ip);
    Serial.println("/");
    Serial.print("JSON:  http://");
    Serial.print(ip);
    Serial.println("/data");
    Serial.println("================================");
  } else {
    Serial.println("Ethernet not OK (no valid IP).");
  }
}

static void ethernet_begin() {
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, W5500_CS);
  Ethernet.init(W5500_CS);

  if (W5500_RST >= 0) {
    pinMode(W5500_RST, OUTPUT);
    digitalWrite(W5500_RST, LOW);
    delay(50);
    digitalWrite(W5500_RST, HIGH);
    delay(200);
  }

  // Try immediately
  ethernet_try_begin();
  g_lastEthRetryMs = millis();
}

// Retry Ethernet in the background if it failed
static void ethernet_retry_if_needed() {
  const unsigned long now = millis();
  if (g_ethOk) return;
  if (now - g_lastEthRetryMs < ETH_RETRY_MS) return;
  g_lastEthRetryMs = now;

  Serial.println("Ethernet retry...");
  ethernet_try_begin();
}

// -------------------- JSON PUBLISH ---------------------
static void publish_json_periodic() {
  const unsigned long now = millis();
  const unsigned long period = g_ethOk ? JSON_PERIOD_OK_MS : JSON_PERIOD_FAIL_MS;

  if (now - g_lastPublishMs < period) return;
  g_lastPublishMs = now;

  JsonDocument doc;
  get_sensor_data(doc);

  // Add Ethernet status into the JSON payload
  doc["eth_link"] = g_linkOn ? "on" : "off";
  doc["eth_ok"]   = g_ethOk ? "yes" : "no";

  g_state = state_machine_step(doc, g_state);

  g_lastJson = "";
  serializeJson(doc, g_lastJson);

  // Always print to Serial; prints faster when Ethernet fails
  Serial.println(g_lastJson);
}

// -------------------- SETUP / LOOP ---------------------
void setup() {
  Serial.begin(115200);
  delay(200);

  sensors_init(PIR_PIN, ECHO_PIN, TRIG_PIN);
  state_machine_init(RED_PIN, GREEN_PIN, 255);

  // Force immediate JSON print at boot (even if Ethernet init blocks later)
  g_lastPublishMs = millis() - JSON_PERIOD_FAIL_MS;
  publish_json_periodic();

  ethernet_begin();
}

void loop() {
  publish_json_periodic();       // always prints; faster when Ethernet is down
  ethernet_retry_if_needed();    // tries to recover Ethernet automatically

  // Only bother serving clients if Ethernet is actually up
  if (g_ethOk) {
    handle_client();
    Ethernet.maintain();
  }
}
