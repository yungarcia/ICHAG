#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>

// Envolvente, en nº de interrupciones (cada una dura 12,5 us):
//   frecuencia de la envolvente = 80000 / (ON_TICKS + OFF_TICKS)
const uint16_t ON_TICKS  = 200;   // 2,5 ms emitiendo
const uint16_t OFF_TICKS = 200;   // 2,5 ms en silencio  -> 200 Hz, 50 %
// OFF_TICKS = 0  -> ultrasonido continuo, sin envolvente (para comparar)

void setup() {
  // D2: entrada con pull-up (el ESP32 la pone a LOW para activar)
  DDRD  &= ~_BV(PD2);
  PORTD |=  _BV(PD2);

  // A0-A3 como salidas, a 0 V al arrancar (el driver no envía tensión)
  DDRC  |= 0x0F;
  PORTC &= 0xF0;

  // Timer1 en modo CTC, sin prescaler: 16 MHz / 200 = 80 kHz
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  OCR1A  = 199;
  TCCR1B |= _BV(WGM12) | _BV(CS10);
  TIMSK1 |= _BV(OCIE1A);
  sei();

  // Apagamos lo que no se usa (timer0 incluido, para evitar jitter)
  power_adc_disable();
  power_spi_disable();
  power_twi_disable();
  power_timer0_disable();
  power_usart0_disable();
}

ISR(TIMER1_COMPA_vect) {
  static uint8_t  pattern   = 0b00001010;  // A1 y A3 en alto, A0 y A2 en bajo
  static uint16_t counter   = 0;
  static bool     emitting  = true;
  static bool     wasActive = false;

  // D2 en HIGH -> apagado: todas las entradas del driver a 0 V
  if (PIND & _BV(PD2)) {
    PORTC &= 0xF0;
    wasActive = false;
    return;
  }

  // Al activarse, la envolvente empieza siempre por un tramo completo
  if (!wasActive) {
    wasActive = true;
    counter   = 0;
    emitting  = true;
  }

  // Envolvente
  if (OFF_TICKS > 0) {
    counter++;
    if (counter >= (emitting ? ON_TICKS : OFF_TICKS)) {
      emitting = !emitting;
      counter  = 0;
    }
  }

  // Portadora: invierte el patrón push-pull en cada interrupción
  if (emitting) {
    PORTC = (PORTC & 0xF0) | pattern;
    pattern ^= 0x0F;
  } else {
    PORTC &= 0xF0;
  }
}

void loop() {
  // Todo se hace por hardware/interrupciones.
}
