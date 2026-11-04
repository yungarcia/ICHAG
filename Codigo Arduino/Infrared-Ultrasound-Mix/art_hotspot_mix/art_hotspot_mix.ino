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
  body, html { margin: 0; font-family: Georgia, serif; background-color: #1F1B16; }

  #statusPanel {
    position: sticky; top: 0; z-index: 10;
    height: 110px; overflow-y: auto; box-sizing: border-box;
    background-color: #1F1B16; color: #F4C430;
    padding: 10px; font-family: monospace;
  }

  .section {
    display: flex; flex-direction: column; align-items: center;
    padding: 30px 0;
  }

  button {
    width: 80%; min-height: 110px; font-size: 1.6em; border-radius: 14px;
    cursor: pointer; font-family: Georgia, serif;
  }

  .info {
    margin: 20px auto 0 auto; width: 90%; box-sizing: border-box;
    border-radius: 10px; padding: 14px;
    font-size: 0.95em; line-height: 1.4em;
  }
  .info h2 { margin: 0 0 8px 0; }

  /* ---------- LUZ INFRARROJA (Los Girasoles) ---------- */
  #secInfrarroja { background-color: #E8720C; }
  #infrarroja { border: 4px solid #6B3E11; }
  #infrarroja.false { background-color: #F4C430; color: #3B2107; }
  #infrarroja.true  { background-color: #D6241C; color: #FFE9A8; }
  #infoInfrarroja {
    background-color: #FFE9A8; border: 3px solid #6B3E11; color: #3B2107;
  }

  /* ---------- ULTRASONIDO (El caminante sobre el mar de nubes) ---------- */
  #secUltrasonido { background: linear-gradient(#2F4B6E 0%, #8FA9C4 55%, #DDE6EE 100%); }
  #ultrasonido { border: 4px solid #1F2A38; }
  #ultrasonido.false { background-color: #F5F8FB; color: #2F4B6E; }
  #ultrasonido.true  { background-color: #1F3A5F; color: #F5F8FB; }
  #infoUltrasonido {
    background-color: #F5F8FB; border: 3px solid #1F2A38; color: #1F2A38;
  }
</style>
</head>
<body>
<div id="statusPanel">Sistema iniciado. Esperando órdenes...</div>

<!-- ========== LUZ INFRARROJA ========== -->
<div class="section" id="secInfrarroja">
  <button id="infrarroja" onclick="toggleDeviceStatus('infrarroja')" class="false"> Luz Infrarroja | Estado: <span>OFF</span></button>

  <div class="info" id="infoInfrarroja">
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
</div>

<!-- ========== ULTRASONIDO ========== -->
<div class="section" id="secUltrasonido">
  <button id="ultrasonido" onclick="toggleDeviceStatus('ultrasonido')" class="false"> Ultrasonido | Estado: <span>OFF</span></button>

  <div class="info" id="infoUltrasonido">
    <h2>El caminante sobre el mar de nubes - Caspar David Friedrich</h2>
    <p>
      El caminante sobre el mar de nubes (1818), del pintor alemán Caspar
      David Friedrich, es la obra cumbre del Romanticismo. El cuadro presenta
      a un viajero solitario de espaldas sobre un precipicio rocoso,
      contemplando un inmenso paisaje montañoso cubierto por una densa
      niebla. Al utilizar esta postura (conocida como Rückenfigur), el
      artista invita al espectador a ocupar el lugar del protagonista y
      experimentar "lo sublime": la abrumadora inmensidad y el misterio de la
      naturaleza frente a la pequeñez del ser humano.
    </p>
    <p>
      La niebla actúa así como una poderosa metáfora del futuro incierto o de
      la introspección del alma. Aunque el paisaje parece auténtico, fue
      compuesto en su estudio a partir de varios bocetos de las montañas de
      arenisca del Elba, y actualmente puede admirarse en el museo
      Kunsthalle de Hamburgo.
    </p>
  </div>
</div>

<script>
  let devices = new Map();
  devices.set('infrarroja',  {"status":false, "ip":"192.168.1.3"});
  devices.set('ultrasonido', {"status":false, "ip":"192.168.1.10"});

  function toggleDeviceStatus(stimuliType) {
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
