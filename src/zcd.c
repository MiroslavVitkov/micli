#include "config.h"
#include "zcd.h"

#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>


#define PROC_VAL_MAX (ZCD_PROC_VAL_MAX)
#define ATOMIC ATOMIC_BLOCK(ATOMIC_FORCEON)


typedef zcd_time_t time_t;
typedef zcd_proc_val_t proc_val_t;


static volatile bool g_event_captured = false;
static volatile bool g_calibration_mode = false;
static time_t g_calibration;
static proc_val_t g_setpoint;


bool should_turn_on(void);
void start_timer1_ctc(time_t max);
void start_timer1_capture_rising(void);


// We use the falling edge interrupt both in calibration and in live run.
ISR(TIMER1_CAPT_vect)
{
    if(g_calibration_mode)
    {
        g_event_captured = true;
    }
    else
    {
        start_timer1_ctc(g_calibration);         // Will call TIMER1_COMPA_vect() at next true ZC.
    }
}


// We use the CTC interrupt only in live run mode.
ISR(TIMER1_COMPA_vect)
{
    if(should_turn_on())
    {
        HEATER_PORT |= (1<<HEATER_PIN);
    }
    else
    {
        HEATER_PORT &= ~(1<<HEATER_PIN);
    }
    start_timer1_capture_rising();
}


void start_timer1_capture_rising(void)
{
    ATOMIC
    {
        TCCR1B = (1<<ICNC1) | (1<<ICES1) | (1<<CS10);  // Enable noise canceler, interrupt on rising edge, clock prescaler == 1.
        TIMSK1 = (1<<ICIE1);                           // Call TIMER1_CAPT_vect() on rising edge.
    }
}


void start_timer1_capture_falling(void)
{
    ATOMIC
    {
        TCCR1B = (1<<ICNC1) | (1<<CS10);         // Enable noise canceler, interrupt on falling edge, clock prescaler == 1.
        TIMSK1 = (1<<ICIE1);                     // Call TIMER1_CAPT_vect() on falling edge.
    }
}


void start_timer1_ctc(time_t max)
{
    ATOMIC
    {
        TCNT1 = 0;
        OCR1A = max;
    }
    TCCR1B = (1<<WGM12) | (1<<CS10);             // CTC mode, clock prescaler == 1.
    TIMSK1 = (1<<OCIE1A);                        // Call TIM1_COMPA_vect() when time is up.
}


void stop_timer1(void)
{
    TCCR1B = 0;                                  // Stop clock.
    TIMSK1 = 0;                                  // Disable interrupts.
}


// Decide if this particular half-wave should be turned on. Further discussion:
// http://programmers.stackexchange.com/questions/304546/algorithm-to-express-an-integer-as-a-sum-of-some-binary-numbers
// The function simulates an uint16_t overflow.
bool should_turn_on()
{
    static uint32_t acu = 0;
    acu += g_setpoint;
    if(acu > PROC_VAL_MAX)
    {
        acu &= PROC_VAL_MAX;
        return true;
    }
    else
    {
        return false;
    }
}


// An oscilloscope trace of the ZCD pcb, used in the project can be found here:
// http://electronics.stackexchange.com/questions/201605/automatic-calibration-of-a-zero-cross-detector-latency
// It is evident, that the pulse is wide, but centered around the ZC.
// Therefore, we measure its width and divide it by two to get the offset from the rising edge to the true ZC.
time_t zcd_calibrate(void)
{
    char sreg = SREG;
    sei();

    // Wait for rising edge of the zcd pulse.
    g_calibration_mode = true;
    start_timer1_capture_rising();
    while(!g_event_captured);
    g_event_captured = false;
    ATOMIC{ TCNT1 = 0; }   // Clear timer1.

    // Measure pulse width.
    start_timer1_capture_falling();
    while(!g_event_captured);
    volatile time_t count;
    ATOMIC{ count = ICR1; } // Width of the zcd pulse, in CPU cycles.
    stop_timer1();

    // Estimate offset from measured to real zc.
    time_t rising_to_zc = count / 2;             // In CPU cycles.

    SREG = sreg;
    return rising_to_zc;
}


void zcd_adjust_setpoint(proc_val_t setpoint)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        g_setpoint = setpoint;
    }
}


// Wait for rising edge (input capture mode).
// Then wait for half a pulse width duration (ctc mode).
// Then set or clear the triac triger pin.
// Repeat indeffinetely.
void zcd_run(time_t calibration)
{
    sei();

    g_calibration_mode = false;
    ATOMIC{ g_calibration = calibration; }
    start_timer1_capture_rising();
}
