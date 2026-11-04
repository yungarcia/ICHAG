#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

#define LED_STIMULI_PIN 2
#define LED_NETWORK_PIN 4
#define OUTPUT_ARDUINO_PIN 26 // This pin will be conected to D2 en the arduino, the esp32 will actuate as a pullup button

const char *ssid = "upnalabHotspot";
const char *password = "upnalab2023";

IPAddress staticIP(192, 168, 1, 5);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

WebServer server(80);

int stimulate = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("I AM ULTRASOUND'S ESP32. I will try to connect with IP: 192.168.1.5");
  pinMode(OUTPUT_ARDUINO_PIN, OUTPUT);
  pinMode(LED_STIMULI_PIN, OUTPUT);
  digitalWrite(LED_STIMULI_PIN, LOW);
  pinMode(LED_NETWORK_PIN, OUTPUT);
  digitalWrite(LED_NETWORK_PIN, LOW);

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
  if (stimulate){
    digitalWrite(OUTPUT_ARDUINO_PIN, LOW); // SIMULATES BUTTON PRESSED
  }else{
    digitalWrite(OUTPUT_ARDUINO_PIN, HIGH); // SIMULATES BUTTON PRESSED
  }
}

void handleOnRequest() {
  Serial.println("Ultrasound ON");
  stimulate = 1;
  digitalWrite(LED_STIMULI_PIN, HIGH);
  Serial.println("--------------");
  server.send(200, "text/plain", "Ultrasound ON");
}

void handleOffRequest() {
  Serial.println("Ultrasound OFF");
  digitalWrite(LED_STIMULI_PIN, LOW);
  stimulate = 0;
  Serial.println("--------------");
  server.send(200, "text/plain", "Ultrasound OFF");
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
