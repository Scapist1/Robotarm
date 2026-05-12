#include <avr/io.h>
#include "dance.h"
#include <util/delay.h>

extern uint16_t uart_target[4];
extern uint8_t uart_active[4];

// Dance baby
typedef enum
{
    dance_s_off,
    dance_s_forward,
    dance_s_back
} dance_state_t;

static dance_state_t dance_state = dance_s_off;
static uint8_t dance_reps = 0;

static const int16_t dance_back[3] = {767, 460, 485}; // smooth positioner
static const int16_t dance_fwd[3] = {869, 562, 588};

static void start_dance(void)
{ // får den i position
    dance_reps = 0;
    dance_state = dance_s_forward;

    for (uint8_t i = 0; i < 3; i++)
    {
        uart_target[i] = dance_fwd[i];
        uart_active[i] = 1;
    }
}

static void dance(void)
{
    if (dance_state == dance_s_off)
        return;

    // checker om den har "homet"
    if (uart_active[0] || uart_active[1] || uart_active[2])
        return;

    // Når den er "homet", gå videre
    switch (dance_state)
    {
    case dance_s_forward:

        for (uint8_t i = 0; i < 3; i++)
        {
            uart_target[i] = dance_back[i];
            uart_active[i] = 1;
        }
        dance_state = dance_s_back;
        break;

    case dance_s_back:
        // vi er tilbage, tælles som én omgang.
        dance_reps++;
        if (dance_reps >= 4) // gør 4 gange
        {
            // Done
            dance_state = dance_s_off;
        }
        else
        {
            // tag en svingom mere
            for (uint8_t i = 0; i < 3; i++)
            {
                uart_target[i] = dance_fwd[i];
                uart_active[i] = 1;
            }
            dance_state = dance_s_forward;
        }
        break;

    default:
        dance_state = dance_s_off;
        break;
    }
}