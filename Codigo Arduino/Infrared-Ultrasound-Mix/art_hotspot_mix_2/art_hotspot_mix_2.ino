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
<meta name="viewport" content="width=device-width, initial-scale=1.0, viewport-fit=cover">
<meta name="color-scheme" content="light">
<title>Panel de control</title>
<style>
  :root {
    --bg: #F5F5F7;
    --card: #FFFFFF;
    --text: #1D1D1F;
    --muted: #6E6E73;
    --line: #E8E8ED;
    --track-off: #D1D1D6;
    --shadow: 0 1px 2px rgba(0,0,0,.04), 0 6px 20px rgba(0,0,0,.05);
  }

  * { box-sizing: border-box; -webkit-tap-highlight-color: transparent; }

  body {
    margin: 0;
    background: var(--bg);
    color: var(--text);
    font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif;
    font-size: 16px;
    line-height: 1.5;
    -webkit-font-smoothing: antialiased;
  }

  main {
    max-width: 560px;
    margin: 0 auto;
    padding: calc(28px + env(safe-area-inset-top)) 16px calc(32px + env(safe-area-inset-bottom));
  }

  h1 { font-size: 28px; font-weight: 700; letter-spacing: -0.02em; margin: 0 4px 20px; }

  .card {
    background: var(--card);
    border-radius: 20px;
    box-shadow: var(--shadow);
    margin-bottom: 16px;
    overflow: hidden;
    position: relative;
  }

  /* ---- Adornos de fondo (no interfieren con los toques) ---- */
  .deco { position: absolute; z-index: 0; pointer-events: none; }
  .flor  { opacity: .38; }
  .nube  { opacity: .30; }
  .action, .info { position: relative; z-index: 1; }

  /* ---- Botón grande ---- */
  .action { padding: 16px; }
  .row {
    width: 100%; min-height: 96px;
    display: flex; flex-direction: column; align-items: center; justify-content: center;
    gap: 4px; padding: 16px;
    border: 0; border-radius: 16px;
    background: var(--tint); color: var(--tint-text);
    font: inherit; cursor: pointer;
    transition: background .25s ease, color .25s ease, transform .1s ease;
  }
  .row:active { transform: scale(.985); }
  .row:focus-visible { outline: 2px solid var(--accent); outline-offset: 3px; }
  .row-title { display: block; width: 100%; text-align: center; font-size: 20px; font-weight: 600; letter-spacing: -0.01em; }
  .row-state { display: block; width: 100%; text-align: center; font-size: 14px; opacity: .8; }
  .on .row { background: var(--accent); color: #fff; }
  .on .row-state { opacity: .9; }

  /* ---- Texto de la obra ---- */
  .info { padding: 18px 20px 22px; border-top: 1px solid var(--line); }
  .eyebrow {
    margin: 0 0 6px; font-size: 12px; font-weight: 600;
    letter-spacing: .08em; text-transform: uppercase; color: var(--accent);
  }
  .info h2 { margin: 0; font-size: 22px; font-weight: 700; letter-spacing: -0.015em; line-height: 1.25; }
  .artist { margin: 2px 0 14px; font-size: 15px; color: var(--muted); }
  .info p { margin: 0 0 12px; font-size: 15px; line-height: 1.6; }
  .info p:last-child { margin-bottom: 0; }

  /* ---- Acento de cada dispositivo (tonos apagados) ---- */
  #infrarroja  { --accent: #A8832A; --tint: #F3EBD2; --tint-text: #5A4512; }
  #ultrasonido { --accent: #5B7C9D; --tint: #E6EDF4; --tint-text: #2F4660; }

  /* ---- Actividad ---- */
  .log { padding: 16px 20px; }
  .log h3 { margin: 0 0 8px; font-size: 13px; font-weight: 600; letter-spacing: .06em; text-transform: uppercase; color: var(--muted); }
  #statusPanel {
    max-height: 110px; overflow-y: auto;
    font-family: ui-monospace, SFMono-Regular, Menlo, Consolas, monospace;
    font-size: 13px; line-height: 1.6; color: var(--muted);
  }
</style>
</head>
<body>
<svg width="0" height="0" style="position:absolute" aria-hidden="true" focusable="false">
  <defs>
    <symbol id="girasol" viewBox="-50 -50 100 100">
      <g fill="#E6CB6E"><ellipse cx="0" cy="-34" rx="9" ry="16" transform="rotate(0)"/><ellipse cx="0" cy="-34" rx="9" ry="16" transform="rotate(30)"/><ellipse cx="0" cy="-34" rx="9" ry="16" transform="rotate(60)"/><ellipse cx="0" cy="-34" rx="9" ry="16" transform="rotate(90)"/><ellipse cx="0" cy="-34" rx="9" ry="16" transform="rotate(120)"/><ellipse cx="0" cy="-34" rx="9" ry="16" transform="rotate(150)"/><ellipse cx="0" cy="-34" rx="9" ry="16" transform="rotate(180)"/><ellipse cx="0" cy="-34" rx="9" ry="16" transform="rotate(210)"/><ellipse cx="0" cy="-34" rx="9" ry="16" transform="rotate(240)"/><ellipse cx="0" cy="-34" rx="9" ry="16" transform="rotate(270)"/><ellipse cx="0" cy="-34" rx="9" ry="16" transform="rotate(300)"/><ellipse cx="0" cy="-34" rx="9" ry="16" transform="rotate(330)"/></g>
      <g fill="#D4AE45" transform="rotate(15)"><ellipse cx="0" cy="-26" rx="8" ry="13" transform="rotate(0)"/><ellipse cx="0" cy="-26" rx="8" ry="13" transform="rotate(30)"/><ellipse cx="0" cy="-26" rx="8" ry="13" transform="rotate(60)"/><ellipse cx="0" cy="-26" rx="8" ry="13" transform="rotate(90)"/><ellipse cx="0" cy="-26" rx="8" ry="13" transform="rotate(120)"/><ellipse cx="0" cy="-26" rx="8" ry="13" transform="rotate(150)"/><ellipse cx="0" cy="-26" rx="8" ry="13" transform="rotate(180)"/><ellipse cx="0" cy="-26" rx="8" ry="13" transform="rotate(210)"/><ellipse cx="0" cy="-26" rx="8" ry="13" transform="rotate(240)"/><ellipse cx="0" cy="-26" rx="8" ry="13" transform="rotate(270)"/><ellipse cx="0" cy="-26" rx="8" ry="13" transform="rotate(300)"/><ellipse cx="0" cy="-26" rx="8" ry="13" transform="rotate(330)"/></g>
      <circle r="18" fill="#8A6420"/>
      <circle r="11" fill="#6B4B18"/>
    </symbol>
    <symbol id="nube" viewBox="0 0 120 70">
      <g fill="#6F92B5"><circle cx="40" cy="44" r="22"/><circle cx="64" cy="32" r="27"/><circle cx="90" cy="44" r="20"/><rect x="18" y="44" width="90" height="22" rx="11"/></g>
    </symbol>
  </defs>
</svg>
<main>
  <h1>Panel de control</h1>

  <!-- ========== LUZ INFRARROJA ========== -->
  <section class="card" id="infrarroja">
    <svg class="deco flor" style="right:-46px;bottom:-46px;width:190px;height:190px;transform:rotate(12deg)" viewBox="0 0 100 100" aria-hidden="true"><use href="#girasol" width="100" height="100"/></svg>
    <svg class="deco flor" style="right:92px;bottom:-34px;width:110px;height:110px;transform:rotate(-14deg)" viewBox="0 0 100 100" aria-hidden="true"><use href="#girasol" width="100" height="100"/></svg>
    <svg class="deco flor" style="right:-18px;bottom:128px;width:84px;height:84px;transform:rotate(28deg)" viewBox="0 0 100 100" aria-hidden="true"><use href="#girasol" width="100" height="100"/></svg>
    <svg class="deco flor" style="left:-30px;bottom:6px;width:96px;height:96px;transform:rotate(-8deg)" viewBox="0 0 100 100" aria-hidden="true"><use href="#girasol" width="100" height="100"/></svg>
    <div class="action">
      <button class="row" aria-pressed="false" onclick="toggleDeviceStatus('infrarroja')">
        <span class="row-title">Luz infrarroja</span>
        <span class="row-state">Desactivada</span>
      </button>
    </div>


    <div class="info">
      <p class="eyebrow">Obra</p>

      <h2>Los Girasoles</h2>
      <p class="artist">Vincent van Gogh</p>
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
  </section>

  <!-- ========== ULTRASONIDO ========== -->
  <section class="card" id="ultrasonido">
    <svg class="deco nube" style="right:-46px;bottom:34px;width:210px;height:123px" viewBox="0 0 120 70" aria-hidden="true"><use href="#nube" width="120" height="70"/></svg>
    <svg class="deco nube" style="left:-44px;bottom:-14px;width:150px;height:88px" viewBox="0 0 120 70" aria-hidden="true"><use href="#nube" width="120" height="70"/></svg>
    <svg class="deco nube" style="right:40px;top:132px;width:120px;height:70px" viewBox="0 0 120 70" aria-hidden="true"><use href="#nube" width="120" height="70"/></svg>
    <svg class="deco nube" style="left:20px;top:250px;width:90px;height:53px" viewBox="0 0 120 70" aria-hidden="true"><use href="#nube" width="120" height="70"/></svg>
    <div class="action">
      <button class="row" aria-pressed="false" onclick="toggleDeviceStatus('ultrasonido')">
        <span class="row-title">Ultrasonido</span>
        <span class="row-state">Desactivado</span>
      </button>
    </div>

    <div class="info">
      <p class="eyebrow">Obra</p>
      <h2>El caminante sobre el mar de nubes</h2>
      <p class="artist">Caspar David Friedrich</p>
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
  </section>

  <!-- ========== ACTIVIDAD ========== -->
  <section class="card log">
    <h3>Actividad</h3>
    <div id="statusPanel">Sistema iniciado. Esperando órdenes...</div>
  </section>
</main>

<script>
  let devices = new Map();
  devices.set('infrarroja',  {"status":false, "ip":"192.168.1.3",  "name":"Luz infrarroja", "on":"Activada",  "off":"Desactivada"});
  devices.set('ultrasonido', {"status":false, "ip":"192.168.1.10", "name":"Ultrasonido",    "on":"Activado",  "off":"Desactivado"});

  function toggleDeviceStatus(stimuliType) {
    if (devices.has(stimuliType)) {
        let device = devices.get(stimuliType);
        device.status = !device.status;
        devices.set(stimuliType, device);

        if (device.status) {
          fetch("http://" + device.ip + "/on").catch(error => console.log('Error:', error));
        } else {
          fetch("http://" + device.ip + "/off").catch(error => console.log('Error:', error));
        }

        let card = document.getElementById(stimuliType);
        if (card) {
          card.classList.toggle('on', device.status);
          card.querySelector('.row').setAttribute('aria-pressed', device.status ? 'true' : 'false');
          card.querySelector('.row-state').textContent = device.status ? device.on : device.off;
        }

        let panel = document.getElementById('statusPanel');
        let line = document.createElement('div');
        line.textContent = new Date().toLocaleTimeString() + "  " + device.name + ": " + (device.status ? device.on.toLowerCase() : device.off.toLowerCase());
        panel.appendChild(line);
        panel.scrollTop = panel.scrollHeight;
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
