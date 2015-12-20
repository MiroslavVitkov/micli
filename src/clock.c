#include "clock.h"

#include <stdbool.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/atomic.h>


#define ATOMIC ATOMIC_BLOCK(ATOMIC_FORCEON)


typedef clock_seconds_t time_t;
time_t g_time_sec;


ISR(WDT_vect)
{
    clock_init();
    ++g_time_sec;
}


void go_to_sleep(void)
{
    set_sleep_mode(SLEEP_MODE_IDLE);
    cli();
    sleep_enable();
    sei();
    sleep_cpu();
    sleep_disable();
}


void clock_init(void)
{
    wdt_reset();
    ATOMIC
    {
        wdt_enable(WDTO_1S);
        WDTCSR |= (1<<WDCE);
        WDTCSR = (1<<WDIE) | (1<<WDP2) | (1<<WDP1);  // Interrupt mode, 1 second cycle.
    }
}


// Sleep until a watchdog interrupt is executed.
// If any other interrupt weakes us up, we know it by g_time_sec.
void clock_sleep_until_next_second(void)
{
    time_t before, now;
    ATOMIC{ before = g_time_sec; }
    do
    {
        go_to_sleep();
        ATOMIC{ now = g_time_sec; }
    } while( before == now );
}


clock_seconds_t clock_get()
{
    time_t now;
    ATOMIC{ now = g_time_sec;  }
    return now;
}
