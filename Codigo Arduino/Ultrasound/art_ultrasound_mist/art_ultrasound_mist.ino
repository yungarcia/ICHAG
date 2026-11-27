#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

#define OUTPUT_ARDUINO_PIN 26 //Ultrasonido

const int NUM_MISTS = 2;
const int MIST_PINS[NUM_MISTS] = {27, 25}; //Negro -> PIN 27, Amarillo -> PIN 25

const int PRESSES_TO_TURN_ON[NUM_MISTS]  = {1, 1};   
const int PRESSES_TO_TURN_OFF[NUM_MISTS] = {2, 3};   

const int PRESS_MS[NUM_MISTS] = {250, 250}; //Cuanto tiempo se mantiene pulsado el boton 
const int GAP_MS[NUM_MISTS]   = {600, 1200}; //Pausa entre pulsaciones, "levantado"

const char *ssid = "TFGYunGarcia";
const char *password = "161747ICHAG";

IPAddress staticIP(192, 168, 1, 10);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

WebServer server(80);
int stimulate = 0;

bool mistOn[NUM_MISTS] = {false, false}; //Modulos apagados al iniciar
int pendingPresses[NUM_MISTS] = {0, 0}; //Cuantas pulsaciones quedan pendientes
bool pressHeld[NUM_MISTS] = {false, false}; //Si esta pulsado el boton del modulo o no 
unsigned long lastChange[NUM_MISTS] = {0, 0}; //Tiempo en el que se pulso o solto por ultima vez 

void applyUltrasound() {
  digitalWrite(OUTPUT_ARDUINO_PIN, stimulate ? LOW : HIGH);
}

void queuePresses(int i, int n) { //Para sumar las pulsaciones pendientes
  pendingPresses[i] += n;
  if (pendingPresses[i] > 10) pendingPresses[i] = 10;
}

void updateMists() {
  unsigned long now = millis();
  for (int i = 0; i < NUM_MISTS; i++) {
    if (pressHeld[i]) {
      if (now - lastChange[i] >= (unsigned long)PRESS_MS[i]) { //El boton esta pulsado, se tacha una pulsacion 
        digitalWrite(MIST_PINS[i], LOW);   
        pressHeld[i] = false;
        lastChange[i] = now;
        pendingPresses[i]--;
      }
    } else if (pendingPresses[i] > 0 && now - lastChange[i] >= (unsigned long)GAP_MS[i]) { //El boton esta suelto y quedan pulsaciones 
      digitalWrite(MIST_PINS[i], HIGH);      
      pressHeld[i] = true;
      lastChange[i] = now;
    }
  }
}

void setMists(bool turnOn) {
  for (int i = 0; i < NUM_MISTS; i++) {
    if (mistOn[i] != turnOn) { //No coinciden los estados, apuntamos las pulsaciones necesarias
      queuePresses(i, turnOn ? PRESSES_TO_TURN_ON[i] : PRESSES_TO_TURN_OFF[i]);
      mistOn[i] = turnOn;
    }
  }
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
  int press = constrain(server.hasArg("press") ? server.arg("press").toInt() : PRESS_MS[i], 20, 1000);
  int times = constrain(server.hasArg("times") ? server.arg("times").toInt() : 1, 1, 4);
  int gap   = constrain(server.hasArg("gap")   ? server.arg("gap").toInt()   : GAP_MS[i], 50, 3000);
  Serial.println("Prueba modulo " + String(i + 1) + ": " + String(times) + " pulsacion(es) de " + String(press) + " ms, pausa " + String(gap) + " ms");
  server.send(200, "text/plain", "Pulsacion enviada");
  for (int n = 0; n < times; n++) {
    digitalWrite(MIST_PINS[i], HIGH);
    delay(press);
    digitalWrite(MIST_PINS[i], LOW);
    if (n < times - 1) delay(gap);
  }
}
void handleMist1Test() { pressOne(0); }
void handleMist2Test() { pressOne(1); }

void handleSyncRequest() {
  for (int i = 0; i < NUM_MISTS; i++) {
    mistOn[i] = false;
    pendingPresses[i] = 0;
  }
  server.send(200, "text/plain", "Sincronizado: modulos marcados como apagados");
}

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

  WiFi.setSleep(false); 

  Serial.println("\nConectado al Hotspot");
  Serial.print("Mi IP es: ");
  Serial.println(WiFi.localIP());

  server.on("/on", HTTP_GET, handleOnRequest);
  server.on("/off", HTTP_GET, handleOffRequest);
  server.on("/mist1", HTTP_GET, handleMist1Test);
  server.on("/mist2", HTTP_GET, handleMist2Test);
  server.on("/sync", HTTP_GET, handleSyncRequest);
  server.begin();
}

void loop() {
  server.handleClient();
  updateMists();
  applyUltrasound();
}