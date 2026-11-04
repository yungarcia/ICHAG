#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

#define LED_STIMULI_PIN 4
#define LED_NETWORK_PIN 2
#define MOSFET_PIN 5

const char *ssid = "upnalabHotspot";
const char *password = "upnalab2023";

IPAddress staticIP(192, 168, 1, 3);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

WebServer server(80);

void setup() {
  Serial.begin(115200);
  Serial.println("I AM HEATLAMP'S ESP32. I will try to connect with IP: 192.168.1.3");

  pinMode(LED_STIMULI_PIN, OUTPUT);
  digitalWrite(LED_STIMULI_PIN, LOW);
  pinMode(LED_NETWORK_PIN, OUTPUT);
  digitalWrite(LED_NETWORK_PIN, LOW);
  pinMode(MOSFET_PIN, OUTPUT);
  digitalWrite(MOSFET_PIN, LOW);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  if (!WiFi.config(staticIP, gateway, subnet)) {
    Serial.println("Error configuring static IP.");
  }
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    digitalWrite(LED_NETWORK_PIN, HIGH);
    delay(500);
    digitalWrite(LED_NETWORK_PIN, LOW);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected succesfully.");
  Serial.print("My IP: ");
  Serial.println(WiFi.localIP());
  digitalWrite(LED_NETWORK_PIN, HIGH);

  // Manejadores de peticiones HTTP
  server.on("/on", HTTP_GET, handleOnRequest);
  server.on("/off", HTTP_GET, handleOffRequest);
  server.onNotFound(handleBadRequest);
  server.begin();
  Serial.println("_________________");
}

void loop() {
  server.handleClient();
}

void handleOnRequest() {
  Serial.println("Heat ON");
  digitalWrite(MOSFET_PIN, HIGH);
  digitalWrite(LED_STIMULI_PIN, HIGH);
  Serial.println("--------------");
  server.send(200, "text/plain", "Heat ON");
}

void handleOffRequest() {
  Serial.println("Heat OFF");
  digitalWrite(LED_STIMULI_PIN, LOW);
  digitalWrite(MOSFET_PIN, LOW);
  Serial.println("--------------");
  server.send(200, "text/plain", "Heat OFF");
}

void handleBadRequest() {
  Serial.println("Bad request");
  Serial.print("Method: ");
  Serial.println(server.method() == HTTP_GET ? "GET" : "Unkwown");
  Serial.print("URI: ");
  Serial.println(server.uri());
  Serial.println("Cabeceras:");
  for (uint8_t i = 0; i < server.headers(); i++) {
    Serial.print(server.headerName(i));
    Serial.print(": ");
    Serial.println(server.header(i));
  }
  Serial.println("--------------");
  server.send(400, "text/plain", "Bad Request");
}
