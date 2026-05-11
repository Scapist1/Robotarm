#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "ssd1306.h"
#include "I2C.h"
#include "UART.h"
#include "ADC.h"
#include "timer.h"
#include "PWM.h"

// Globale variabler
int16_t smooth_values[4] = {512, 512, 512, 512};
int16_t uart_target[4]   = {-1, -1, -1, -1};
char PWMmode = 'c'; // Start i mode 'c' (Phase & Frequency Correct)

/* Benyttes i while loopet til at læse data fra UART til de rigtige steder når ny_data_klar flaget bliver sat højt */
void uart_kommando() {
  char local_buf[16];

  for (uint8_t i = 0; i < 16; i++) local_buf[i] = rx_buffer[i]; // skriver alle 16 symboler fra rx_buffer over i local buffer, når ny_data_klar flaget bliver højt i while loopet (main)
  ny_data_klar = 0; // nulstiller, så vi er klar til næste besked

  if (local_buf[0] >= 'a' && local_buf[0] <= 'd' && local_buf[1] == '\0') {  // Tjekker om det er a, b, c, d (PWM mode skift)
    PWMmode = local_buf[0]; // sætter PWMmode a, b, c eller d
    return;
  }

  /* Læser første */
  uint8_t ch = local_buf[0] - '0';  // den læser det som ASCII, så resultatet bliver ASCII værdien i local_buf[0] - 48
  int32_t val = 0;

  if (ch <= 3 && local_buf[1] == ':') {
    for (uint8_t i = 2; local_buf[i] >= '0' && local_buf[i] <= '9' && i < 6; i++) {  // kigger på om de bufferen indeholder ASCII der svarer til tal mellem 0 og 9, kommando beskedens X'er 0:XXXX
      val = val * 10 + (local_buf[i] - '0');  // bygger et 4 ciffer tal op ved at regne ASCII værdien fra buffer om til tal
    }
    if (ch >= 0 && ch <= 3 && val >= 0 && val <= 1023) {
      uart_target[ch] = val;
    }
  }
}

int main(void) {
  char buffer[20];

  init_ADC();
  init_timer0();
  
  // Initialiser state machine med start-mode
  run_state_machine(&PWMmode);
  
  I2C_Init();
  clear_display();
  InitializeDisplay();
  uart0_Init(16); // 115200 baud
  
  printString("[ Skriv 0:1023 for servo eller a,b,c,d for PWM mode ]\r\n");

  /* Sætter tal indikator ud for værdier på display (0, 1, 2, 3) */
  for(uint8_t i = 0; i <= 3; i++) {
      sprintf(buffer, "%d", i);
      sendStrXY(buffer, i + 2, 1);
  }

  sei();

  while (1) {
    if (ny_data_klar) {
      uart_kommando(); 
    }

    run_state_machine(&PWMmode); // Opdater timere hvis PWMmode er ændret

    for (uint8_t i = 0; i < 4; i++) {
      cli();
      int16_t raw_joy = joystick_values[i];
      sei();

      /* Easing logik til at ændre tal fra monitor eller joystick løbende */
      if (uart_target[i] >= 0) {
        int16_t diff = uart_target[i] - smooth_values[i];
        if (diff > 0) {
          smooth_values[i] += (diff / 16) + 1;
          if (smooth_values[i] >= uart_target[i]) {
            smooth_values[i] = uart_target[i]; 
            uart_target[i] = -1; 
          }
        } else if (diff < 0) {
          smooth_values[i] += (diff / 16) - 1;
          if (smooth_values[i] <= uart_target[i]) { 
            smooth_values[i] = uart_target[i]; 
            uart_target[i] = -1; 
          }
        } else { uart_target[i] = -1; }
      } 
      else if (raw_joy < 500 || raw_joy > 524) {
        if (raw_joy > 524) smooth_values[i] += (raw_joy - 512) / 128;
        else smooth_values[i] -= (512 - raw_joy) / 128;

        if (smooth_values[i] > 1023) smooth_values[i] = 1023;
        if (smooth_values[i] < 0)    smooth_values[i] = 0;
      }

      // Beregn PWM (Mode C: 500-2500 for servo)
      uint16_t pwm = (uint16_t)(((uint32_t)smooth_values[i] * 2000) / 1023) + 500;

      // Opdater registrer
      if (i == 0) OCR1A = pwm;
      if (i == 1) OCR3A = pwm;
      if (i == 2) OCR4A = pwm;
      if (i == 3) OCR5A = pwm;

    }
    
    // 3. DISPLAY-LOOP (Opdaterer skærmen samlet)
    for (uint8_t i = 0; i <= 3; i++) {
        // Hent den aktuelle PWM værdi fra registret (eller genberegn den)
        // Her genberegner vi den kort for at få duty cycle
        uint16_t temp_pwm = ((uint32_t)smooth_values[i] * 2000) / 1023 + 500;
        uint16_t duty_x10 = temp_pwm / 20;

        // Skriv rå værdi (0-1023)
        sprintf(buffer, "%4d", smooth_values[i]);
        sendStrXY(buffer, i + 2, 3);
        
        // Skriv procent (f.eks. 7.5%)
        sprintf(buffer, "%2d.%1d%%", duty_x10 / 10, duty_x10 % 10);
        sendStrXY(buffer, i + 2, 10);
    }

    _delay_ms(10); // Gør easing jævn og forhindrer OLED I2C spam

    // --- Display opdatering ---
    for(uint8_t i = 0; i <= 3; i++) {
      sprintf(buffer, "%4d", smooth_values[i]);
      sendStrXY(buffer, i + 2, 3);
    }
  }
}