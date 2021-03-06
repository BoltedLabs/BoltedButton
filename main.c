/* Bolted Button
 * Developed by Bolted
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "USART.h"


#define BUTTON_PORT             PORTC
#define BUTTON_PIN              PINC
#define BUTTON_ADDR             PC5
#define LED_DDR                 DDRB
#define LED_PORT                PORTB
#define LED_ADDR                PB4
#define ACTION_COOLDOWN         500 /* In milliseconds */
#define BUTTON_TIMING           500 /* In milliseconds */
#define BUTTON_HOLD             1000 /* In milliseconds */
#define BUTTON_MAX_CLICKS       3
#define INTERVAL_WAIT_TIMEOUT   1000


volatile uint8_t listen = 0;
volatile uint16_t time_ms = 0;
volatile uint16_t cooldown_ms = 0;
volatile uint8_t is_down = 0;
volatile uint8_t is_up = 1;
volatile uint8_t clicks = 0;
volatile char led_command = 0;
volatile uint16_t flash_interval = 0;
volatile uint16_t led_ms = 0;


ISR(TIMER1_COMPA_vect)
{
    if (listen) time_ms++;
    if (cooldown_ms > 0) cooldown_ms--;
    if (flash_interval) led_ms++;
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

    OCR1A = 15;
    TCCR1B |= (1 << WGM12);
    TCCR1B |= (1 << CS10);
    TCCR1B |= (1 << CS12);
    TIMSK1 |= (1 << OCIE1A);
    sei();

    // Ready!
    LED_PORT &= ~(1 << LED_ADDR);
    transmitByte('R');

    for(;;) {
        // Wait for cooldown
        if (!cooldown_ms) {
            if (bit_is_clear(PINC, PC5)) {
                // Add clicks
                if (listen && time_ms <= BUTTON_TIMING) {
                    if (is_up) {
                        time_ms = 0;
                        if (++clicks >= BUTTON_MAX_CLICKS) {
                            listen = 0;
                            cooldown_ms = ACTION_COOLDOWN;
                        }
                    }
                } else if (!listen && is_up) {
                    // First click
                    time_ms = 0;
                    clicks = 1;
                    listen = 1;
                }

                // Holding button down
                if (listen && clicks == 1 && time_ms >= BUTTON_HOLD) {
                    transmitByte('H');
                    listen = 0;
                    clicks = 0;
                    cooldown_ms = ACTION_COOLDOWN;
                }

                is_up = 0;
                is_down = 1;
            } else {
                // Reset
                if (time_ms >= BUTTON_TIMING) {
                    time_ms = 0;
                    listen = 0;
                    cooldown_ms = ACTION_COOLDOWN;
                }

                is_up = 1;
                is_down = 0;
            }
        }

        // Transmit data
        if (clicks > 0 && !listen) {
            switch (clicks) {
                case 1:
                    transmitByte('S');
                    break;
                case 2:
                    transmitByte('D');
                    break;
                case 3:
                    transmitByte('T');
                    break;
                default:
                    break;
            }

            clicks = 0;
        }

        // Receive commands
        if (bit_is_set(UCSR0A, RXC0)) {
            led_command = UDR0;
            // Clear flash
            flash_interval = 0;
            led_ms = 0;

            switch (led_command) {
                case 'O':
                    LED_PORT |= (1 << LED_ADDR);
                    break;
                case 'N':
                    LED_PORT &= ~(1 << LED_ADDR);
                    break;
                case 'F':
                    flash_interval = getNumber() * 100;
                    break;
                default:
                    break;
            }
        }

        // Flash LED
        if (flash_interval) {
            if (led_ms >= flash_interval) {
                led_ms = 0;
                LED_PORT ^= (1 << LED_ADDR);
            }
        }
    }

    return 0;
}
