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
#include "string.h"
#include "dance.h"

#define BAUD 115200
#define UBRR_VAL ((F_CPU / 8 / BAUD) - 1)

// Globale variabler
int16_t smooth_values[4] = {512, 512, 512, 512};
uint16_t uart_target[4]   = {512, 512, 512, 512};
uint8_t uart_active[4]   = {1, 1, 1, 1};

/* Benyttes i while loopet til at læse data fra UART til de rigtige steder når ny_data_klar flaget bliver sat højt */
void uart_kommando() {
  char local_buf[16];

  cli();
  for (uint8_t i = 0; i < 16; i++)
    local_buf[i] = rx_buffer[i]; // skriver alle 16 symboler fra rx_buffer over i local buffer, når ny_data_klar flaget bliver højt i while loopet (main)
    ny_data_klar = 0; // nulstiller, så vi er klar til næste besked
  sei();

  if (strcmp(local_buf, "dance") == 0) // lyt for dance kommandoen
  {
    start_dance();
    return;
  }

  /* Læser første */
  uint8_t ch = local_buf[0] - '0';  // den læser det som ASCII, så resultatet bliver ASCII værdien i local_buf[0] - 48
  int32_t val = 512;

  if (ch <= 3 && local_buf[1] == ':') {
    for (uint8_t i = 2; local_buf[i] >= '0' && local_buf[i] <= '9' && i < 6; i++) {  // kigger på om de bufferen indeholder ASCII der svarer til tal mellem 0 og 9, kommando beskedens X'er 0:XXXX
      val = val * 10 + (local_buf[i] - '0');  // bygger et 4 ciffer tal op ved at regne ASCII værdien fra buffer om til tal
    }
    if (val <= 1023) {
      uart_target[ch] = val;
      uart_active[ch] = 1;
    }
  }
}

int main(void)
{
  char buffer[20];

  init_ADC();
  init_timer0();
  
  PWM_ph_fr_init();

  I2C_Init();
  clear_display();
  InitializeDisplay();
  uart0_Init(UBRR_VAL); // 115200 baud

  printString("[ Skriv fx 0:1023 for servo 0, position 1023 ]\r\n");

  static uint8_t display_counter = 0;

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

    for (uint8_t i = 0; i < 4; i++) {
      cli();
      int16_t raw_joy = joystick_values[i]; //atomic
      sei();

      /* Easing logik til at ændre tal fra monitor eller joystick løbende */
      // UART
      if (uart_active[i]) {
        int16_t diff = uart_target[i] - smooth_values[i];

        if (diff > 0)
        {
          int16_t step = (diff / 16) + 1; // Tager højde for nuværende position
          if (step > 13) step = 13; // capper den på max 13 pr step aka 60 grader pr. 0.2 sekund
          smooth_values[i] += step;
          if (smooth_values[i] >= uart_target[i])
          {  // fanger hvis den er overshootet og korrigerer så vi stadig rammer target
            smooth_values[i] = uart_target[i]; 
            uart_active[i] = 0;
          }
        }
        else if (diff < 0)
        {
          int16_t step = (diff / 16) - 1; // Tager højde for nuværende position
          if (step < -13) step = -13; // capper den på max 13 pr step aka 60 grader pr. 0.2 sekund
          smooth_values[i] += step;
          if (smooth_values[i] <= uart_target[i])
          { // fanger overshoot den anden vej
            smooth_values[i] = uart_target[i];
            uart_active[i] = 0;
          }
        }
        else
        {
          uart_active[i] = 0;
        }
      } 

      //Joystick 
      else if (raw_joy < 500 || raw_joy > 524) { //er det uden for deadzone? Joy og UART er i samme if-else for at kun én er aktiv ad gangen
        // uart_active[i] = 0;  //ville få joystick til at kunne afbryde UART bevægelser, men tænker heller vi vil have den altid kører UART færdig
        if (raw_joy > 524) smooth_values[i] += (raw_joy - 512) / 128;
        else smooth_values[i] -= (512 - raw_joy) / 128;

        if (smooth_values[i] > 1023) smooth_values[i] = 1023;
        if (smooth_values[i] < 0)    smooth_values[i] = 0;
      }

      // Beregn PWM (Ph_fr: 500-2500 for servo)
      uint16_t pwm = (uint16_t)(((uint32_t)smooth_values[i] * 2000) / 1023) + 500;

      // Opdater registrer
      switch (i){
        case 0:
          OCR1A = pwm; // behøver ikk wrappe i cli(); og sei(); da AVR ifølge datasheetet har et dedikeret 8-bit TEMP register, så det er kun for READ af 16-bits vi skal, ikke for WRITES.
          break;
        case 1:
          OCR3A = pwm;
          break;
        case 2:
          OCR4A = pwm;
          break;
        case 3:
          OCR5A = pwm;
          break;
        }
    }
    dance_FSM(); //Tjekker hvilken state dance_FSM er i, hvis den er blevet "aktiveret" fra uart_kommando går den i gang med dansesekvensen

    // 3. DISPLAY-LOOP (Opdaterer skærmen samlet)
    if (++display_counter >= 10){ // så skærmen ikke har samme frekvens som servo - unødvendigt
      display_counter = 0;
      for (uint8_t i = 0; i <= 3; i++)
      {
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
  }

    _delay_ms(10); // Gør easing jævn og holder display fra at flimre
  }
}