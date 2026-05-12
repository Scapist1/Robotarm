#include "PWM.h"
#include "UART.h"


// Phase og Frequency Correct PWM (WGM13, prescale 8)
// ICR = 20000 → 50Hz → 20ms periode, passer til servomotorer


void init_ph_frPWM_1()  {   // Timer 1
  DDRB |= (1 << PB5); // pin 11
  TCCR1A = (1 << COM1A1);  // Clear OC1A ved Compare Match på opadgående. Set OC1A ved Compare Match på nedadgående.
  TCCR1B = (1 << CS11) | (1 << WGM13); // prescaling by 8
  ICR1 = 20000; // top value then OC1A pin can be used // 16 bit top value
  OCR1A = 1500;  // 7.5% duty cycle
}

void init_ph_frPWM_2()  {   // Timer 3
  DDRE |= (1 << PE3); // pin 5
  TCCR3A = (1 << COM3A1); // Clear OC1A ved Compare Match på opadgående. Set OC1A ved Compare Match på nedadgående.
  TCCR3B = (1 << CS31) | (1 << WGM33); // prescaling by 8
  ICR3 = 20000; // top value then OC1A pin can be used // 16 bit top value
  OCR3A = 1500;  // 7.5% duty cycle
}

void init_ph_frPWM_3()  {   // Timer 4
  DDRH |= (1 << PH3); // pin 6
  TCCR4A = (1 << COM4A1);  // Clear OC1A ved Compare Match på opadgående. Set OC1A ved Compare Match på nedadgående.
  TCCR4B = (1 << CS41) | (1 << WGM43); // prescaling by 8
  ICR4 = 20000; // top value then OC1A pin can be used // 16 bit top value
  OCR4A = 1500;  // 7.5% duty cycle // bliver ændret af int fra joystick
}

void init_ph_frPWM_4()  {   // Timer 5
  DDRL |= (1 << PL3); // pin 46
  TCCR5A = (1 << COM5A1); // Clear OC1A ved Compare Match på opadgående. Set OC1A ved Compare Match på nedadgående.
  TCCR5B = (1 << CS51) | (1 << WGM53); // prescaling by 8
  ICR5 = 20000; // top value then OC1A pin can be used // 16 bit top value
  OCR5A = 1500;  // 7.5% duty cycle
}


void PWM_ph_fr_init() {
  // 1. Sluk timere (nulstil control registers)
  TCCR1A = 0; TCCR1B = 0;
  TCCR3A = 0; TCCR3B = 0;
  TCCR4A = 0; TCCR4B = 0;
  TCCR5A = 0; TCCR5B = 0;

  // 2. Nulstil selve tælleren (forhindrer overflow-glitches)
  TCNT1 = 0; 
  TCNT3 = 0; 
  TCNT4 = 0; 
  TCNT5 = 0;

      init_ph_frPWM_1(); init_ph_frPWM_2(); init_ph_frPWM_3(); init_ph_frPWM_4();
}
