/*
 *
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "USART.h"


#define BUTTON_PORT     PORTC
#define BUTTON_PIN      PINC
#define BUTTON_ADDR     PC5
#define LED_DDR         DDRB
#define LED_PORT        PORTB
#define LED_ADDR        PB4
#define BUTTON_TIMING   500 /* In milliseconds */
#define BUTTON_HOLD     5000 /* In milliseconds */


// Global timer vars
volatile uint32_t time_ms = 0;


// Global status vars
volatile uint32_t up_time = 0;
volatile uint8_t clicks = 0;
volatile uint8_t click_type = 0;


ISR(TIMER1_COMPA_vect)
{
    time_ms++;
}


void read_button(void)
{
    if (click_type == 0) {
        uint32_t delta = time_ms - up_time;

        if (bit_is_clear(BUTTON_PIN, BUTTON_ADDR)) {
            if (up_time > 0 && delta <= BUTTON_TIMING) {
                clicks++;
            }

            up_time = 0;
        } else {
            up_time = time_ms;
        }

        // Update click if has been too long
        if (up_time > BUTTON_TIMING || clicks > 2) {
            click_type = clicks;
            clicks = 0;
            up_time = 0;
        }
    }
}


void do_serial_stuff(void)
{
    if (click_type > 0) {
        switch (click_type) {
            case 1:
                transmitByte('S');
                break;
            case 2:
                transmitByte('D');
                break;
            case 3:
                transmitByte('T');
                break;
            case 4:
                transmitByte('H');
                break;
            default:
                break;
        }

        // Reset click type
        click_type = 0;

        // Delay between actions
        _delay_ms(500);
    }
}


void do_led_stuff(void)
{

}


int main(void)
{
    // Configure PINS
    BUTTON_PORT |= (1 << BUTTON_ADDR);
    LED_DDR = 0XFF;

    // initialize timer
    cli();
    TCCR1A = 0;
    TCCR1B = 0;

    OCR1A = 131;
    TCCR1B |= (1 << WGM12);
    TCCR1B |= (1 << CS10);
    TCCR1B |= (1 << CS12);
    TIMSK1 |= (1 << OCIE1A);
    sei();

    for(;;) {
        read_button();
        do_serial_stuff();
        do_led_stuff();
    }

    return 0;
}
