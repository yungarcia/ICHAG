#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

#define LED_PIN 2

const char *ssid = "TFGYunGarcia";
const char *password = "161747ICHAG";

WebServer server(80);

const char* indexHTML = R"RAW(
<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<style>
  body, html {
    height: 100%; margin: 0; font-family: Georgia, serif;
    background-color: #E8720C;
  }
  .container { display: flex; flex-direction: column; height: 80%; align-items: center; justify-content: center; }
  button {
    width: 80%; height: 35%; font-size: 1.6em; border-radius: 14px;
    border: 4px solid #6B3E11; cursor: pointer; font-family: Georgia, serif;
  }
  .false { background-color: #F4C430; color: #3B2107; }
  .true  { background-color: #D6241C; color: #FFE9A8; }
  #statusPanel { height: 20%; background-color: #3B2107; overflow-y: auto; color: #F4C430; padding: 10px; font-family: monospace; }
  #info {
    margin: 20px auto; max-width: 90%; background-color: #FFE9A8;
    border: 3px solid #6B3E11; border-radius: 10px; padding: 14px;
    color: #3B2107; font-size: 0.95em; line-height: 1.4em;
  }
  #info h2 { margin: 0 0 8px 0; }
</style>
</head>
<body>
<div id="statusPanel">Sistema iniciado. Esperando órdenes...</div>
<div class="container">
  <button id="infrarroja" onclick="toggleDeviceStatus()" class="false"> Luz Infrarroja | Estado: <span>OFF</span></button>
</div>

<div id="info">
  <h2>Los Girasoles - Vincent van Gogh</h2>
  <p>
    Van Gogh no pintó un único cuadro de girasoles, sino una serie de siete,
    realizados en Arlés (Francia) entre agosto de 1888 y enero de 1889,
    mientras esperaba la llegada de su amigo el pintor Paul Gauguin. Esta
    versión concreta, con el fondo turquesa y el jarrón y la mesa en
    amarillo, es una de las dos que Van Gogh consideraba las más
    importantes de la serie, y se conserva hoy en la Neue Pinakothek de
    Múnich.
  </p>
  <p>
    Como en el resto de la serie, casi todo el cuadro está pintado dentro
    de una misma gama de amarillos y ocres, con pequeños contrastes de
    verde y turquesa de fondo. Van Gogh llegó a describir el amarillo como
    un color que representaba para él la luz y la gratitud. Hoy la serie
    completa es una de las más reconocidas del postimpresionismo.
  </p>
</div>

<script>
  let devices = new Map();
  devices.set('infrarroja', {"status":false, "ip":"192.168.1.3"});

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
  server.handleClient();
}
