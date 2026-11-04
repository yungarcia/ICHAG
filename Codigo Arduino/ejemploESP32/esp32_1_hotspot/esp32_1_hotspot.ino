// HOTSPOT 192.168.1.1
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

#define LED_PIN 2 // this led will indicate that the hotspot is ready

const char *ssid = "upnalabHotspot";
const char *password = "upnalab2023";

int previousClientCount = 0;
int currentClientCount = previousClientCount;

WebServer server(80); //Para crear el servidor y indicar en que puerto debes escuchar las peticiones (80 es el estandar para trafico HTTP)
/* La IP al ser 192.168.1.1 el navegador asume que se conecta al puerto 80. Si el puerto fuera otro (ej. 8000) 
habria que escribirlo obligatoriamente. [http://192.168.1.1:8080 para poder abrir la pagina web)*/

//Guarda el "archivo" html en una cadena de texto, utilizando sintaxis RAW de C++ 
/* Comunicacion Movil - ESP32 (1): Esto una vez que abres la IP del Hotspot en el movil
    1- El navegador pide la pagina: GET 
    2- ESP32 envia el HTML 
    3- El navegador dibuja la web y ejecuta el javaScript */
const char* indexHTML = R"RAW(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<style>
  body, html {
    height: 100%;
    margin: 0;
  }
  .container {
    display: flex;
    flex-direction: column;
    height: 80%;
  }
  .box {
    flex: 1;
    display: flex;       
    align-items: center;
    justify-content: center;
    border: 1px solid #000; /* Para visualizar los límites del div */
  }
  button {
    width: 100%;
    height: 100%;
    font-size: 1em;
  }
  .false{
    background-color: lightcoral;
  }
  .true{
    background-color: lightgreen;
  }
  #statusPanel{
    height: 20%;
    background-color: black;
    overflow-y: auto;
    color:white;
  }
</style>
</head>
<body>
<div class="container" id="statusPanel">
Updated info will be shown here uppon request.
</div>
<div class="container">
  <div class="box">
    <button onclick="updateInfo()">🔄 ℹ️ Update info</button>
  </div>
  <div class="box">
    <button id="electrostatics" onclick="toggleDeviceStatus()" class="false">192.168.1.2 - 👻 Electrostatics | Status: <span>false</span></button>
  </div>
  <div class="box">
    <button id="heat" onclick="toggleDeviceStatus()" class="false">192.168.1.3 - 🔥 Heat | Status: <span>false</span></button>
  </div>
  <div class="box">
    <button id="plasma" onclick="toggleDeviceStatus()" class="false">192.168.1.4 - ⚡ Plasma | Status: <span>false</span></button>
  </div>
  <div class="box">
    <button id="ultrasound" onclick="toggleDeviceStatus()" class="false">192.168.1.5 - 🔊 Ultrasound | Status: <span>false</span></button>
  </div>

</div>
<script>
  let devices = new Map();
  devices.set('electrostatics', {"status":false, "ip":"192.168.1.2"});
  devices.set('heat', {"status":false, "ip":"192.168.1.3"});
  devices.set('plasma', {"status":false, "ip":"192.168.1.4"});
  devices.set('ultrasound', {"status":false, "ip":"192.168.1.5"});
  function updateInfo() {
    fetch("http://192.168.1.1/info")
    .then(response => response.text())
    .then(data => {
      document.getElementById('statusPanel').innerHTML += "<br>" + data;
      document.getElementById('statusPanel').scrollTop = document.getElementById('statusPanel').scrollHeight;
    })
    .catch(error => console.log('Error:', error));
  }
  function toggleDeviceStatus() {
    let stimuliType = event.target.id
    if (devices.has(stimuliType)) {
        let device = devices.get(stimuliType);
        device.status = !device.status;
        devices.set(stimuliType, device);
        console.log(`El estado de ${stimuliType} ha sido cambiado a ${device.status}`);
        document.getElementById('statusPanel').innerHTML += "<br>" + `El estado de ${stimuliType} ha sido cambiado a ${device.status}`;
        document.getElementById('statusPanel').scrollTop = document.getElementById('statusPanel').scrollHeight;
        if (device.status)
          fetch("http://"+device.ip+"/on").then().catch(error => console.log('Error:', error));
        else
          fetch("http://"+device.ip+"/off").then().catch(error => console.log('Error:', error));
        domElement = document.getElementById(stimuliType);
        if(domElement) {
          domElement.getElementsByTagName('span')[0].innerHTML = device.status;
            if(domElement.classList.contains('false')) {
                domElement.classList.remove('false');
                domElement.classList.add('true');
            } else if(domElement.classList.contains('true')) {
                domElement.classList.remove('true');
                domElement.classList.add('false');
            }
        }
    } else {
        console.log(`No se encontró ningún dispositivo con la clave ${key}`);
        document.getElementById('statusPanel').innerHTML += "<br>" + `No se encontró ningún dispositivo con la clave ${key}`;
    }
  }
</script>
</body>
</html>
)RAW";


void handleBadRequest() {
  Serial.println("Received an non expected request! I'm just the hotspot, don't communicate with me!");
  server.send(400, "text/plain", "I'm just the hotspot, don't communicate with me!");
}

void handleIndexRequest(){
  server.send(200, "text/html", indexHTML);
  Serial.println("Received a request in /");
}
void handleInfoRequest(){
  server.send(200, "text/html", "INFO -> Number of clients connected: "+String(WiFi.softAPgetStationNum()));
  Serial.println("Received a request in /info");
}   

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password,1,0,9);

  IPAddress IP(192, 168, 1, 1);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.softAPConfig(IP, gateway, subnet);

  Serial.print("Punto de acceso iniciado, IP: ");
  Serial.println(WiFi.softAPIP());

  // Manejadores de peticiones HTTP
  server.on("/on", HTTP_GET, handleBadRequest);
  server.on("/off", HTTP_GET, handleBadRequest);
  server.onNotFound(handleBadRequest);
  server.on("/", HTTP_GET, handleIndexRequest);
  server.on("/info", HTTP_GET, handleInfoRequest);
  server.begin();
}

void loop() {
  currentClientCount = WiFi.softAPgetStationNum();
  
  if (currentClientCount > previousClientCount) {
    Serial.println("New device connected to hotspot. (Current number of devices: "+ String(currentClientCount)+")");
  } else if (currentClientCount < previousClientCount) {
    Serial.println("One device has been disconnected from the hotspot. Ciao bella! (Current number of devices: "+ String(currentClientCount)+")");
  }

  previousClientCount = currentClientCount;

  server.handleClient();
}


