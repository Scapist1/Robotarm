#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>
#include <stdint.h>

extern volatile uint16_t joystick_values[4];
extern volatile uint8_t current_ch;

void init_ADC(void);

#endif