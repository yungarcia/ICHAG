#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

#define MOSFET_PIN 5

const char *ssid = "TFGYunGarcia";
const char *password = "161747ICHAG";

IPAddress staticIP(192, 168, 1, 3);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

WebServer server(80);

void handleOnRequest() {
  digitalWrite(MOSFET_PIN, HIGH);
  server.send(200, "text/plain", "OK ON");
}

void handleOffRequest() {
  digitalWrite(MOSFET_PIN, LOW);
  server.send(200, "text/plain", "OK OFF");
}

void setup() {
  pinMode(MOSFET_PIN, OUTPUT);
  digitalWrite(MOSFET_PIN, LOW);

  WiFi.config(staticIP, gateway, subnet);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  server.on("/on", HTTP_GET, handleOnRequest);
  server.on("/off", HTTP_GET, handleOffRequest);
  server.begin();
}

void loop() {
  server.handleClient();
}
