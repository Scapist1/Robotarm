#ifndef PWM_H_
#define PWM_H_

#include <avr/io.h>

// Prototyper så andre filer kan se dine funktioner
void PWM_ph_fr_init();

// Prototyper på dine init-funktioner (hvis de skal kaldes fra main)
void init_ph_frPWM_1(void);
void init_ph_frPWM_2(void);
void init_ph_frPWM_3(void);
void init_ph_frPWM_4(void);

#endif