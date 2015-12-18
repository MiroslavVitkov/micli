#include "clock.h"

#include <stdbool.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/atomic.h>


#define ATOMIC ATOMIC_BLOCK(ATOMIC_FORCEON)


typedef clock_seconds_t time_t;
time_t g_time_sec;


ISR(WDT_vect)
{
    ++g_time_sec;
}


// It appears that <avr/wdt.h> lack support for interrupt-move wdt.
void enable_wdt(void)
{
    WDTCSR = (1<<WDCE);                          // Write enable for 4 cycles.
    WDTCSR = (1<<WDIE) | (1<<WDP2) | (WDP1);     // Interrupt mode, 1 second cycle.
}


// Enter Idle sleep mode.
void go_to_sleep(void)
{
    cli();
    sleep_enable();
    sleep_cpu();
    sei();                                       // Guaranteed to be executed before going to sleep.
    sleep_disable();
}


void clock_init(void)
{
    ATOMIC{ enable_wdt(); }
}


// Go sleep. When woken up, detect that the cause is the 1s wdt.
void clock_sleep_until_next_second(void)
{
    time_t now;
    ATOMIC{ now = g_time_sec; }
    bool elapsed = false;
    do
    {
        go_to_sleep();
        ATOMIC{ elapsed = (now == g_time_sec); }
    } while(!elapsed);
}
