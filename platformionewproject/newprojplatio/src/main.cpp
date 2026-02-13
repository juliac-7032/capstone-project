#include <Arduino.h>
#include <ArduinoJson.h>

#include <SPI.h>
#include <Ethernet_Generic.h>

#include "sensors.h"
#include "state_machine.h"

// ===================== PIN CONFIG =====================
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

// LEDs
static const int RED_PIN   = 8;
static const int GREEN_PIN = 9;
// ======================================================

// MAC address (must be unique on LAN)
static byte mac[] = { 0x02, 0x12, 0x34, 0x56, 0x78, 0x9A };

// State machine
static state_e g_state = STATE_UNOCC;

// ---------- Ethernet status ----------
static bool g_ethOk = false;
static bool g_linkOn = false;
static unsigned long g_lastEthRetryMs = 0;
static const unsigned long ETH_RETRY_MS = 5000;

// ---------- JSON publish cache ----------
static String g_lastJson = "{}";
static unsigned long g_lastPublishMs = 0;

// JSON rates
static const unsigned long JSON_PERIOD_OK_MS   = 1000;
static const unsigned long JSON_PERIOD_FAIL_MS = 200;

// ---------- Maintenance override (from website) ----------
static volatile bool g_forceMaint = false;

// ---------- Serial printing control ----------
static bool g_printedLinksOnce = false;   // print web links only once when IP is valid

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

  html += "<button id='maintBtn'>Maintenance: ...</button>";
  html += "<p><small>Maintenance forces the LEDs into MAINT mode until turned off.</small></p>";

  html += "<pre id='out'>Loading...</pre>";

  html += "<script>";
  html += "async function setMaint(on){";
  html += "  try{ await fetch('/maint?on='+(on?1:0)); }catch(e){}";
  html += "  await tick();";
  html += "}";
  html += "document.getElementById('maintBtn').addEventListener('click', async ()=>{";
  html += "  const isOn = document.getElementById('maintBtn').dataset.on === '1';";
  html += "  await setMaint(!isOn);";
  html += "});";

  html += "async function tick(){";
  html += " try{";
  html += "  const r=await fetch('/data');";
  html += "  const j=await r.json();";
  html += "  document.getElementById('out').textContent=JSON.stringify(j,null,2);";
  html += "  const maint = (j.maintenance === 'on');";
  html += "  const b=document.getElementById('maintBtn');";
  html += "  b.textContent='Maintenance: ' + (maint?'ON':'OFF');";
  html += "  b.dataset.on = maint? '1':'0';";
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

// Very small query parser: returns value for key in "?a=1&b=2"
static String get_query_value(const String& pathAndQuery, const char* key) {
  int q = pathAndQuery.indexOf('?');
  if (q < 0) return "";

  String qs = pathAndQuery.substring(q + 1);
  String k = String(key) + "=";

  int start = qs.indexOf(k);
  if (start < 0) return "";
  start += k.length();

  int end = qs.indexOf('&', start);
  if (end < 0) end = qs.length();

  return qs.substring(start, end);
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

  String route = path;
  int q = route.indexOf('?');
  if (q >= 0) route = route.substring(0, q);

  if (route == "/" || route.startsWith("/index")) {
    send_http(client, "text/html; charset=utf-8", index_html());
  }
  else if (route == "/data") {
    send_http(client, "application/json", g_lastJson);
  }
  else if (route == "/maint") {
    String v = get_query_value(path, "on");
    if (v.length() > 0) {
      g_forceMaint = (v == "1" || v == "true" || v == "on");
    }
    String body = String("{\"maintenance\":\"") + (g_forceMaint ? "on" : "off") + "\"}";
    send_http(client, "application/json", body);
  }
  else {
    send_404(client);
  }

  delay(1);
  client.stop();
}

// -------------------- ETHERNET INIT / RETRY ------------
static void print_links_once_if_needed() {
  if (g_printedLinksOnce) return;
  if (!g_ethOk) return;

  IPAddress ip = Ethernet.localIP();

  Serial.println("================================");
  Serial.print("Web:   http://");
  Serial.print(ip);
  Serial.println("/");

  Serial.print("JSON:  http://");
  Serial.print(ip);
  Serial.println("/data");
  Serial.println("================================");

  g_printedLinksOnce = true;
}

static void ethernet_try_begin() {
  Serial.println("Starting Ethernet...");

  // IMPORTANT FIX:
  // Always attempt begin first. On W5500, linkStatus() can be UNKNOWN/OFF
  // until after the chip is initialized, so checking link first can cause
  // infinite retries.
  if (Ethernet.begin(mac) == 0) {
    Serial.println("DHCP failed — using static IP");
    IPAddress ip(192, 168, 1, 50);
    IPAddress dns(192, 168, 1, 1);
    IPAddress gw(192, 168, 1, 1);
    IPAddress mask(255, 255, 255, 0);
    Ethernet.begin(mac, ip, dns, gw, mask);
  }

  // Read status AFTER begin
  EthernetHardwareStatus hw = Ethernet.hardwareStatus();
  EthernetLinkStatus ls = Ethernet.linkStatus();
  IPAddress ip = Ethernet.localIP();

  Serial.print("HW: ");
  Serial.println(hw == EthernetNoHardware ? "NO_HARDWARE" : "OK");

  Serial.print("Link: ");
  Serial.println(ls == LinkON ? "ON" : (ls == LinkOFF ? "OFF" : "UNKNOWN"));

  Serial.print("IP: ");
  Serial.println(ip);

  g_linkOn = (ls == LinkON);
  g_ethOk  = (ip[0] != 0) && (hw != EthernetNoHardware);

  if (g_ethOk) {
    server.begin();
    print_links_once_if_needed();
  } else {
    Serial.println("Ethernet not OK yet.");
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

  ethernet_try_begin();
  g_lastEthRetryMs = millis();
}

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

  StaticJsonDocument<256> doc;
  get_sensor_data(doc);

  doc["eth_link"] = g_linkOn ? "on" : "off";
  doc["eth_ok"]   = g_ethOk ? "yes" : "no";

  doc["maintenance"] = g_forceMaint ? "on" : "off";
  if (g_forceMaint) {
    doc["overall"] = "maintenance";
  }

  g_state = state_machine_step(doc, g_state);

  g_lastJson = "";
  serializeJson(doc, g_lastJson);

  // NO JSON printing to Serial
}

// -------------------- SETUP / LOOP ---------------------
void setup() {
  Serial.begin(115200);
  delay(200);

  sensors_init(PIR_PIN, ECHO_PIN, TRIG_PIN);
  state_machine_init(RED_PIN, GREEN_PIN, 255);

  // Build first JSON snapshot so /data works immediately
  g_lastPublishMs = millis() - JSON_PERIOD_FAIL_MS;
  publish_json_periodic();

  ethernet_begin();
}

void loop() {
  publish_json_periodic();
  ethernet_retry_if_needed();

  if (g_ethOk) {
    handle_client();
    Ethernet.maintain();
  }
}
