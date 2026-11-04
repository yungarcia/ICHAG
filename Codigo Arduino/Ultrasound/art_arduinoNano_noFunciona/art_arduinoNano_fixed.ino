const int pinESP32 = 2;
const bool DEBUG = true;

void setup() {
  if (DEBUG) Serial.begin(115200);

  pinMode(pinESP32, INPUT_PULLUP);
  DDRC = DDRC | B00001111;

  if (DEBUG) Serial.println("Arduino Nano emitiendo");
}
  
void loop() {
  bool activado = (digitalRead(pinESP32) == LOW);

  if (activado) {
    PORTC = (PORTC & B11110000) | B00001010; 
    delayMicroseconds(12);

    PORTC = (PORTC & B11110000) | B00000101; 
    delayMicroseconds(12);
  } else {
    PORTC = PORTC & B11110000; 
  }

  if (DEBUG) {
    static bool estadoAnterior = false;
    if (activado != estadoAnterior) {
      estadoAnterior = activado;
    }
  }
}
