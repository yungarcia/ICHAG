#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

#define LED_PIN 2

const char *ssid = "TFGYunGarcia";
const char *password = "161747ICHAG";

int previousClientCount = 0;
int currentClientCount = 0;

WebServer server(80);

const char* indexHTML = R"RAW(
<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<style>
  body, html { height: 100%; margin: 0; font-family: Arial, sans-serif; }
  .container { display: flex; flex-direction: column; height: 80%; }
  .box { flex: 1; display: flex; align-items: center; justify-content: center; padding: 10px; }
  button { width: 100%; height: 80%; font-size: 1.5em; border-radius: 10px; border: none; }
  .false { background-color: lightcoral; color: white; }
  .true { background-color: lightgreen; color: black; }
  #statusPanel { height: 20%; background-color: #222; overflow-y: auto; color: #00FF00; padding: 10px; font-family: monospace; }
</style>
</head>
<body>
<div id="statusPanel">
  Sistema iniciado. Esperando órdenes...
</div>
<div class="container">
  <div class="box">
    <button id="ultrasonido" onclick="toggleDeviceStatus()" class="false"> Ultrasonido | Estado: <span>OFF</span></button>
  </div>
</div>

<script>
  let devices = new Map();
  devices.set('ultrasonido', {"status":false, "ip":"192.168.1.10"});

  function toggleDeviceStatus() {
    let stimuliType = event.target.id;
    if (devices.has(stimuliType)) {
        let device = devices.get(stimuliType);
        device.status = !device.status;
        devices.set(stimuliType, device);
        
        let panel = document.getElementById('statusPanel');
        panel.innerHTML += "<br>> Cambiando el estado de " + stimuliType + " a " + device.status;
        panel.scrollTop = panel.scrollHeight;

        if (device.status) {
          fetch("http://" + device.ip + "/on").catch(error => console.log('Error:', error));
        } else {
          fetch("http://" + device.ip + "/off").catch(error => console.log('Error:', error));
        }
        
        let domElement = document.getElementById(stimuliType);
        if(domElement) {
          domElement.getElementsByTagName('span')[0].innerHTML = device.status ? "ON" : "OFF";
          if(device.status) {
              domElement.classList.remove('false');
              domElement.classList.add('true');
          } else {
              domElement.classList.remove('true');
              domElement.classList.add('false');
          }
        }
    }
  }
</script>
</body>
</html>
)RAW";

void handleIndexRequest(){
  server.send(200, "text/html", indexHTML);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); 

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  IPAddress IP(192, 168, 1, 1);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.softAPConfig(IP, gateway, subnet);

  Serial.print("Hotspot creado. IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", HTTP_GET, handleIndexRequest);
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



