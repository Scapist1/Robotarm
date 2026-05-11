#include "ADC.h"
#include <avr/interrupt.h>
#include <util/delay.h>

volatile uint16_t joystick_values[4];
volatile uint8_t current_ch = 0;

void init_ADC() {
  ADMUX = (1 << REFS0); // Reference spænding sat til 5V (AVCC) ("=" sætter REFS0 bit høj og resten lavt)
  ADCSRA |= (1 << ADEN) | (1 << ADIE); // enable ADC and enable interrupt adc complete
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Scaling 16 Mhz to 125 kHz ADC clock frequency with prescaler 128
  DIDR0 = 0xFF;  // Deaktiverer alle digitale input A0-A7 for at spare på strømmen
}

ISR(TIMER0_COMPA_vect)  { // starter sampling
  ADMUX = (1 << REFS0) | (current_ch & 0x07); // Vælg den aktuelle kanal i ADMUX (bevar REFS0) 
  _delay_us(10); // Give the MUX a moment to settle
  ADCSRA |= (1 << ADSC);  // Start konvertering (Sæt ADSC bit)
}

ISR(ADC_vect) { // henter resultat fra sampling
  joystick_values[current_ch] = ADC;
  current_ch++;
  if (current_ch > 3) current_ch = 0;
}