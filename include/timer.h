#ifndef TIMER_H_
#define TIMER_H_

#include <avr/io.h>

// TCNT1 (Timer 1) tæller op konstant, OCR1A er slutværdien der sender TCNT1 tilbage til 0
// Vi bruger Timer 1 (16-bits), da det er mere præcist desto flere bits. Den har 65.535 ticks. En 8-bit har kun 255 ticks. stor forskel!

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

void init_timer0(void); // prototype

#endif