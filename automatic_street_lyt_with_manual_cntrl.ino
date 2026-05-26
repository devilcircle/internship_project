#include <WiFi.h>
#include <WebServer.h>

// WIFI DETAILS
const char* ssid = "realme narzo 30A";
const char* password = "0987654321";

WebServer server(80);

// PINS
#define LED 4        // Built-in LED or connect external LED
#define LDR 34       // Analog pin for LDR

bool manualMode = false;
int threshold = 2000;   // ESP32 ADC range = 0 to 4095

// ---------------- HOME PAGE ----------------
void handleRoot() {

  int ldrValue = analogRead(LDR);

  String page = "<html><body style='text-align:center;font-family:Arial;'>";

  page += "<h1>ESP32 LED CONTROL</h1>";

  page += "<h2>LDR : " + String(ldrValue) + "</h2>";

  if(manualMode)
    page += "<h2>MANUAL MODE</h2>";
  else
    page += "<h2>AUTO MODE</h2>";

  page += "<a href='/manual'><button style='padding:15px;'>MANUAL</button></a>";
  page += "<a href='/auto'><button style='padding:15px;'>AUTO</button></a><br><br>";

  if(manualMode) {

    page += "<a href='/on'><button style='padding:15px;'>ON</button></a>";
    page += "<a href='/off'><button style='padding:15px;'>OFF</button></a>";
  }

  page += "</body></html>";

  server.send(200, "text/html", page);
}

// ---------------- ROUTES ----------------
void handleManual() {
  manualMode = true;
  handleRoot();
}

void handleAuto() {
  manualMode = false;
  handleRoot();
}

void handleOn() {
  digitalWrite(LED, HIGH);
  handleRoot();
}

void handleOff() {
  digitalWrite(LED, LOW);
  handleRoot();
}

// ---------------- SETUP ----------------
void setup() {

  pinMode(LED, OUTPUT);

  Serial.begin(115200);

  // ESP32 ADC setup
  analogReadResolution(12);

  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);

  Serial.print("Connecting");

  while(WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // ROUTES
  server.on("/", handleRoot);
  server.on("/manual", handleManual);
  server.on("/auto", handleAuto);
  server.on("/on", handleOn);
  server.on("/off", handleOff);

  server.begin();

  Serial.println("Server Started");
}

// ---------------- LOOP ----------------
void loop() {

  server.handleClient();

  // AUTO MODE
  if(!manualMode) {

    int ldrValue = analogRead(LDR);

    if(ldrValue < threshold)
      digitalWrite(LED, HIGH);

    else
      digitalWrite(LED, LOW);
  }
}