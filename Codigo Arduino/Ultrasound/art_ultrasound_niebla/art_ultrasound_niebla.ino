#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

#define OUTPUT_ARDUINO_PIN 26 // Ultrasonido

const int NUM_MISTS = 2;
const int MIST_PINS[NUM_MISTS] = {27, 25}; //Mist


const int PRESSES_TO_TURN_OFF = 2;

const int PRESS_MS = 150;  // duracion de cada pulsacion
const int GAP_MS   = 350;  // pausa entre pulsaciones

const char *ssid = "TFGYunGarcia";
const char *password = "161747ICHAG";

IPAddress staticIP(192, 168, 1, 10);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

WebServer server(80);
int stimulate = 0;
bool mistOn[NUM_MISTS] = {false, false};  

void applyUltrasound() {
  digitalWrite(OUTPUT_ARDUINO_PIN, stimulate ? LOW : HIGH);
}

void setMists(bool turnOn) {
  bool need[NUM_MISTS];
  bool any = false;
  for (int i = 0; i < NUM_MISTS; i++) {
    need[i] = (mistOn[i] != turnOn);
    if (need[i]) any = true;
  }
  if (!any) return;

  int times = turnOn ? 1 : PRESSES_TO_TURN_OFF;
  for (int n = 0; n < times; n++) {
    for (int i = 0; i < NUM_MISTS; i++) if (need[i]) digitalWrite(MIST_PINS[i], HIGH);
    delay(PRESS_MS);
    for (int i = 0; i < NUM_MISTS; i++) if (need[i]) digitalWrite(MIST_PINS[i], LOW);
    if (n < times - 1) delay(GAP_MS);
  }
  for (int i = 0; i < NUM_MISTS; i++) if (need[i]) mistOn[i] = turnOn;
}

void handleOnRequest() {
  Serial.println("Ultrasonido + niebla ON");
  stimulate = 1;
  applyUltrasound();                      
  server.send(200, "text/plain", "Ultrasound ON");
  setMists(true);
}

void handleOffRequest() {
  Serial.println("Ultrasonido + niebla OFF");
  stimulate = 0;
  applyUltrasound();
  server.send(200, "text/plain", "Ultrasound OFF");
  setMists(false);
}

void pressOne(int i) {
  Serial.println("Prueba: una pulsacion en el modulo " + String(i + 1));
  server.send(200, "text/plain", "Pulsacion enviada");
  digitalWrite(MIST_PINS[i], HIGH);
  delay(PRESS_MS);
  digitalWrite(MIST_PINS[i], LOW);
}
void handleMist1Test() { pressOne(0); }
void handleMist2Test() { pressOne(1); }

void setup() {
  Serial.begin(115200);

  pinMode(OUTPUT_ARDUINO_PIN, OUTPUT);
  digitalWrite(OUTPUT_ARDUINO_PIN, HIGH);
  for (int i = 0; i < NUM_MISTS; i++) {
    pinMode(MIST_PINS[i], OUTPUT);
    digitalWrite(MIST_PINS[i], LOW);
  }

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
  server.on("/mist1", HTTP_GET, handleMist1Test);
  server.on("/mist2", HTTP_GET, handleMist2Test);
  server.begin();
}

void loop() {
  server.handleClient();
  applyUltrasound();
}