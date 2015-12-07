#include "config.h"

#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>


typedef uint16_t time_t;
static volatile bool g_event_captured = false;


ISR(TIMER1_CAPT_vect)
{
    g_event_captured = true;
}


ISR(TIMER1_COMPA_vect)
{

}


void start_timer1_capture_rising(void)
{
    TCCR1B = (1<<ICNC1) | (1<<ICES1) | (1<<CS11);  // Enable noise canceler, interrupt on rising edge, clock prescaler == 8.
    TIMSK1 = (1<<ICIE1);                           // Enable input capture interrupt.
}


void start_timer1_capture_falling(void)
{
    TCCR1B = (1<<ICNC1) | (1<<CS11);             // Enable noise canceler, interrupt on falling edge, clock prescaler == 8.
    TIMSK1 = (1<<ICIE1);                         // Enable input capture interrupt.
}


void start_timer1_ctc(time_t max)
{
    ATOMIC_BLOCK(ATOMIC_FORCEON)
    {
        TCNT1 = 0;
        OCR1A = max;
    }
    TCCR1B = (1<<WGM11) | (1<<CS11);             // CTC mode, clock prescaler == 8.
    TIMSK1 = (1<<OCIE1A);                        // Call TIM1_COMPA_vect() when time is up.
}


// An oscilloscope trace of the ZCD pcb, used in the project can be found here:
// http://electronics.stackexchange.com/questions/201605/automatic-calibration-of-a-zero-cross-detector-latency
// It is evident, that the pulse is wide, but centered around the ZC.
// Therefore, we measure its width and divide it by two to get the offset from the falling edge to the true ZC.
void zcd_calibrate(void)
{
    start_timer1_capture_rising();
    while(!g_event_captured);
    g_event_captured = false;
    TCNT1 = 0;                                   // Clear timer1.
    start_timer1_capture_falling();
    while(!g_event_captured);
    volatile time_t count = ICR1;                // Width of the zcd pulse, in CPU cycles / 8.
    time_t zc_to_falling = count / 2;            // In CPU cycles / 8.
    const time_t half_period = 10 * F_CPU / (1000 * 8);  // 10ms in CPU cycles / 8.
    time_t falling_to_zc = half_period - zc_to_falling;
    start_timer1_ctc(falling_to_zc);
}
