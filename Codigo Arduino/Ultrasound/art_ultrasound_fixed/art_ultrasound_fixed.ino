#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

#define OUTPUT_ARDUINO_PIN 26 

const char *ssid = "TFGYunGarcia";
const char *password = "161747ICHAG";

IPAddress staticIP(192, 168, 1, 10);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

WebServer server(80);
int stimulate = 0;

void handleOnRequest() {
  Serial.println("Ultrasonido ON");
  stimulate = 1;
  server.send(200, "text/plain", "Ultrasound ON");
}

void handleOffRequest() {
  Serial.println("Ultrasonido OFF");
  stimulate = 0;
  server.send(200, "text/plain", "Ultrasound OFF");
}

void setup() {
  Serial.begin(115200);
  
  pinMode(OUTPUT_ARDUINO_PIN, OUTPUT);
  digitalWrite(OUTPUT_ARDUINO_PIN, HIGH); 

  if (!WiFi.config(staticIP, gateway, subnet)) {
    Serial.println("Error configurando IP estática.");
  }
  
  WiFi.begin(ssid, password);
  Serial.println("Buscando el Hotspot...");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Conectando....");
  }

  Serial.println("\nConectado al Hotspot");
  Serial.print("Mi IP es: ");
  Serial.println(WiFi.localIP());

  server.on("/on", HTTP_GET, handleOnRequest);
  server.on("/off", HTTP_GET, handleOffRequest);
  server.begin();
}

void loop() {
  server.handleClient();

  if (stimulate){
    digitalWrite(OUTPUT_ARDUINO_PIN, LOW); 
  } else {
    digitalWrite(OUTPUT_ARDUINO_PIN, HIGH); 
  }
}