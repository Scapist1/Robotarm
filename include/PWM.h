#ifndef PWM_H_
#define PWM_H_

#include <avr/io.h>

// 1. Definer selve enum-typen
typedef enum {
    init_state,
    listen,
    fastPWM,
    phaseCorrectPWM,
    ph_freq_corr,
    phaseCorrectPWMalt
} pwm_state_t;

// 2. Prototyper så andre filer kan se dine funktioner
void run_state_machine(char *data);

// Prototyper på dine init-funktioner (hvis de skal kaldes fra main)
void init_ph_frPWM_1(void);
void init_ph_frPWM_2(void);
void init_ph_frPWM_3(void);
void init_ph_frPWM_4(void);

#endif