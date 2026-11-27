#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <LittleFS.h>

#define LED_PIN 2

const char *ssid = "TFGYunGarcia";
const char *password = "161747ICHAG";

int previousClientCount = 0;
int currentClientCount = 0;

WebServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  if (!LittleFS.begin()) {
    Serial.println("Error al montar LittleFS");
    return;
  }

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  IPAddress IP(192, 168, 1, 1);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.softAPConfig(IP, gateway, subnet);

  Serial.print("Hotspot creado. IP: ");
  Serial.println(WiFi.softAPIP());

  server.serveStatic("/", LittleFS, "/index.html");
   server.serveStatic("/", LittleFS, "/");
  server.begin();
}

void loop() {
  currentClientCount = WiFi.softAPgetStationNum();
  if (currentClientCount != previousClientCount) {
    Serial.println("Cambio en dispositivos conectados. Total: " + String(currentClientCount));
    previousClientCount = currentClientCount;
  }
  server.handleClient();
}