#include "PWM.h"
#include "UART.h"

// Brug den nye type 'pwm_state_t' i stedet for 'enum pwm_state'
pwm_state_t state = init_state;


// =====================================================================
// Fast PWM (WGM14, prescale 8) — tæller kun op, højere frekvens
// Brugt til LED-dæmpning — ikke anbefalet til servomotorer
// =====================================================================

void init_fastPWM_1() {     // Timer 1 → pin 11 (PB5 / OC1A)
    DDRB |= (1 << PB5);
    TCCR1A = (1 << COM1A1) | (1 << WGM11);
    TCCR1B = (1 << CS11) | (1 << WGM13) | (1 << WGM12); // prescaling by 8
    ICR1 = 20000;
    OCR1A = 1500;
}

void init_fastPWM_2() {     // Timer 3 → pin 5 (PE3 / OC3A)
    DDRE |= (1 << PE3);
    TCCR3A = (1 << COM3A1) | (1 << WGM31);
    TCCR3B = (1 << CS31) | (1 << WGM33) | (1 << WGM32); // prescaling by 8
    ICR3 = 20000;
    OCR3A = 1500;
}

void init_fastPWM_3() {     // Timer 4 → pin 6 (PH3 / OC4A)
    DDRH |= (1 << PH3);
    TCCR4A = (1 << COM4A1) | (1 << WGM41);
    TCCR4B = (1 << CS41) | (1 << WGM43) | (1 << WGM42); // prescaling by 8
    ICR4 = 20000;
    OCR4A = 1500;
}

void init_fastPWM_4() {     // Timer 5 → pin 46 (PL3 / OC5A)
    DDRL |= (1 << PL3);
    TCCR5A = (1 << COM5A1) | (1 << WGM51);
    TCCR5B = (1 << CS51) | (1 << WGM53) | (1 << WGM52); // prescaling by 8
    ICR5 = 20000;
    OCR5A = 1500;
}

// =====================================================================
// Phase Correct PWM (WGM11, prescale 8) — symmetrisk puls, fast top (ICR)
// Ligner ph_freq_corr men ICR må ikke ændres midt i en periode
// =====================================================================

void init_phaseCorrectPWM_1() {  // Timer 1 → pin 11 (PB5 / OC1A)
    DDRB |= (1 << PB5);
    TCCR1A = (1 << COM1A1) | (1 << WGM11);
    TCCR1B = (1 << CS11) | (1 << WGM13); // prescaling by 8
    ICR1 = 20000;
    OCR1A = 1500;
}

void init_phaseCorrectPWM_2() {  // Timer 3 → pin 5 (PE3 / OC3A)
    DDRE |= (1 << PE3);
    TCCR3A = (1 << COM3A1) | (1 << WGM31);
    TCCR3B = (1 << CS31) | (1 << WGM33); // prescaling by 8
    ICR3 = 20000;
    OCR3A = 1500;
}

void init_phaseCorrectPWM_3() {  // Timer 4 → pin 6 (PH3 / OC4A)
    DDRH |= (1 << PH3);
    TCCR4A = (1 << COM4A1) | (1 << WGM41);
    TCCR4B = (1 << CS41) | (1 << WGM43); // prescaling by 8
    ICR4 = 20000;
    OCR4A = 1500;
}

void init_phaseCorrectPWM_4() {  // Timer 5 → pin 46 (PL3 / OC5A)
    DDRL |= (1 << PL3);
    TCCR5A = (1 << COM5A1) | (1 << WGM51);
    TCCR5B = (1 << CS51) | (1 << WGM53); // prescaling by 8
    ICR5 = 20000;
    OCR5A = 1500;
}

// =====================================================================
// Phase & Frequency Correct PWM (WGM13, prescale 8) — standard ved opstart
// ICR = 20000 → 50Hz → 20ms periode, passer til servomotorer
// =====================================================================

void init_ph_frPWM_1()  {   // Timer 1
  DDRB |= (1 << PB5); // pin 11
  TCCR1A = (1 << COM1A1);  // Clear OC1A on Compare Match when up-counting. Set OC1A on Compare Match when down-counting
  TCCR1B = (1 << CS11) | (1 << WGM13); // prescaling by 8
  ICR1 = 20000; // top value then OC1A pin can be used // 8bit top value
  OCR1A = 1500;  // 7.5% duty cycle
}

void init_ph_frPWM_2()  {   // Timer 3
  DDRE |= (1 << PE3); // pin 5
  TCCR3A = (1 << COM3A1);  // Clear OC1A on Compare Match when up-counting. Set OC1A on Compare Match when down-counting
  TCCR3B = (1 << CS31) | (1 << WGM33); // prescaling by 8
  ICR3 = 20000; // top value then OC1A pin can be used // 8bit top value
  OCR3A = 1500;  // 7.5% duty cycle
}

void init_ph_frPWM_3()  {   // Timer 4
  DDRH |= (1 << PH3); // pin 6
  TCCR4A = (1 << COM4A1);  // Clear OC1A on Compare Match when up-counting. Set OC1A on Compare Match when down-counting
  TCCR4B = (1 << CS41) | (1 << WGM43); // prescaling by 8
  ICR4 = 20000; // top value then OC1A pin can be used // 8bit top value
  OCR4A = 1500;  // 7.5% duty cycle // bliver ændret af int fra joystick
}

void init_ph_frPWM_4()  {   // Timer 5
  DDRL |= (1 << PL3); // pin 46
  TCCR5A = (1 << COM5A1);  // Clear OC1A on Compare Match when up-counting. Set OC1A on Compare Match when down-counting
  TCCR5B = (1 << CS51) | (1 << WGM53); // prescaling by 8
  ICR5 = 20000; // top value then OC1A pin can be used // 8bit top value
  OCR5A = 1500;  // 7.5% duty cycle
}

// =====================================================================
// Phase Correct PWM alternativ (WGM10, 8-bit top 0xFF, prescale 8)
// Bruger fast 8-bit top fremfor ICR — lavere opløsning men simplere
// =====================================================================

void init_phaseCorrectPWMalt_1() {  // Timer 1 → pin 11 (PB5 / OC1A)
    DDRB |= (1 << PB5);
    TCCR1A = (1 << COM1A1) | (1 << WGM10);
    TCCR1B = (1 << CS11); // prescaling by 8
    OCR1A = 128; // 50% duty cycle ved 8-bit top
}

void init_phaseCorrectPWMalt_2() {  // Timer 3 → pin 5 (PE3 / OC3A)
    DDRE |= (1 << PE3);
    TCCR3A = (1 << COM3A1) | (1 << WGM30);
    TCCR3B = (1 << CS31); // prescaling by 8
    OCR3A = 128;
}

void init_phaseCorrectPWMalt_3() {  // Timer 4 → pin 6 (PH3 / OC4A)
    DDRH |= (1 << PH3);
    TCCR4A = (1 << COM4A1) | (1 << WGM40);
    TCCR4B = (1 << CS41); // prescaling by 8
    OCR4A = 128;
}

void init_phaseCorrectPWMalt_4() {  // Timer 5 → pin 46 (PL3 / OC5A)
    DDRL |= (1 << PL3);
    TCCR5A = (1 << COM5A1) | (1 << WGM50);
    TCCR5B = (1 << CS51); // prescaling by 8
    OCR5A = 128;
}

// =====================================================================
// State machine — kald med run_state_machine(&data) i main loop
// Skift PWM mode via UART: a=fastPWM, b=phaseCorrect,
//                          c=ph_freq_corr, d=phaseCorrectAlt
// =====================================================================

void run_state_machine(char *data) {
    switch(state) {
        case init_state:
            state = listen;
            break;

        case listen:
            // Vent på UART kommando og skift PWM mode
            if      (*data == 'a') { state = fastPWM;            *data = 0; }
            else if (*data == 'b') { state = phaseCorrectPWM;    *data = 0; }
            else if (*data == 'c') { state = ph_freq_corr;       *data = 0; }
            else if (*data == 'd') { state = phaseCorrectPWMalt; *data = 0; }
            break;

        case fastPWM:
        case phaseCorrectPWM:
        case ph_freq_corr:
        case phaseCorrectPWMalt:
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

            // 3. Initialiser baseret på den valgte state
            if (state == fastPWM) {
                init_fastPWM_1(); init_fastPWM_2(); init_fastPWM_3(); init_fastPWM_4();
                printString("\r\n[ Mode a: Fast PWM aktiveret ]\r\n");
            } 
            else if (state == phaseCorrectPWM) {
                init_phaseCorrectPWM_1(); init_phaseCorrectPWM_2(); init_phaseCorrectPWM_3(); init_phaseCorrectPWM_4();
                printString("\r\n[ Mode b: Phase Correct aktiveret ]\r\n");
            } 
            else if (state == ph_freq_corr) {
                init_ph_frPWM_1(); init_ph_frPWM_2(); init_ph_frPWM_3(); init_ph_frPWM_4();
                printString("\r\n[ Mode c: Phase & Frequency Correct aktiveret ]\r\n");
            } 
            else if (state == phaseCorrectPWMalt) {
                init_phaseCorrectPWMalt_1(); init_phaseCorrectPWMalt_2(); init_phaseCorrectPWMalt_3(); init_phaseCorrectPWMalt_4();
                printString("\r\n[ Mode d: Phase Correct Alt (8-bit) aktiveret ]\r\n");
            }

            // 4. Gå tilbage til lytte-tilstand
            state = listen;
            break;

        default:
            state = init_state;
            break;
    }
}
