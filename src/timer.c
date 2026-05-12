#include "timer.h"
#include <avr/interrupt.h>

void init_timer0() {
  TCCR0A = (1 << WGM01);  // Sæt Timer0 til CTC mode
  OCR0A = 155;     // Sæt sammenligningsværdi (OCR0A) med prescaler 1024: (16MHz / 1024) / 156 ≈ 100 Hz (10ms mellem samples)
  TIMSK0 = (1 << OCIE0A); // Enable Timer Compare Interrupt
  TCCR0B = (1 << CS02) | (1 << CS00);  // Start timer med prescaler 1024
}


