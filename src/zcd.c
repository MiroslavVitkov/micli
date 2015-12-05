#include "usart.h"

#include <avr/io.h>
#include <avr/interrupt.h>


ISR(INT1_vect)
{
    printf("INT1, configured for level change, activated!" NEWLINE);
}


void enable_int1(void)
{
    DDRD  &= ~(1<<PD3);                          // Configure pin as input
    PORTD &= ~(1<<PD3);                          // with no pull-up.
    EICRA |= (1<<ISC10);                         // Interrupt on level change.
    EIMSK |= (1<<INT1);                          // Enable INT1.
    sei();
}

