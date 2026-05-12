#ifndef TIMER_H_
#define TIMER_H_

#include <avr/io.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

void init_timer0(void); // prototype

#endif