// Used hardware: watchdog.
// Used interrupts: WDT_vect.
// This module uses the watchdog 128kH timer to issue 1 second interrupts.
// It also keeps track of real time since start-up.

#ifndef CLOCK_H_
#define CLOCK_H


#include <stdint.h>


typedef uint32_t clock_seconds_t;
void clock_init(void);
void clock_sleep_until_next_second(void);
clock_seconds_t clock_get(void);


#endif  // #ifdef CLOCK_H_
