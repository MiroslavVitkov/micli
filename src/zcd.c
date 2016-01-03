#include "config.h"
#include "usart.h"
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


static volatile bool g_event_captured = false;    // TODO: maybe merge with g_rising_detected
static volatile bool g_calibration_mode = false;
static time_t g_calibration;
static proc_val_t g_setpoint;

// This is a horrible hack.
// For some reason, when reprogramming TIMSK1 inside an interrupt,
// even if the timer is stopped, triggers an immediate CTC interrupt.
// So we enable the interrupts in public api, and then use this variable
// to select the correct CTC interrupt.
static volatile bool g_rising_detected = false;


zcd_time_t zcd_calibrate(void);
void zcd_run(zcd_time_t calibration);

// Get current setpoint in [0, 2^16] range.
zcd_proc_val_t zcd_get(void);
bool should_turn_on(void);
void start_timer1_ctc(time_t max);
void start_timer1_capture_rising(void);


// We use the rising edge interrupt both in calibration and in live run.
ISR(TIMER1_CAPT_vect)
{
    if(g_calibration_mode)
    {
        g_event_captured = true;
    }
    else
    {
        // Make sure the triac control is off before the zc
        // to avoid accidentally enabling it for the next half-wave.
        // Detect the true zero crossing and make a decision for the next half-wave.
        PORT_HEATER &= ~(1<<PIN_HEATER);
        g_rising_detected = true;
        start_timer1_ctc(g_calibration);         // Will call TIMER1_COMPA_vect() at next true ZC.
    }
}


// We use the CTC interrupt only in live run mode.
ISR(TIMER1_COMPA_vect)
{
    if(!g_rising_detected)
    {
        return;
    }
    g_rising_detected = false;
    if(should_turn_on())
    {
        PORT_HEATER |= (1<<PIN_HEATER);
    }
    else
    {
        PORT_HEATER &= ~(1<<PIN_HEATER);
    }
    start_timer1_capture_rising();
}


inline void start_timer1_capture_rising(void)
{
    TCCR1B = (1<<ICNC1) | (1<<ICES1) | (1<<CS10);  // Enable noise canceler, interrupt on rising edge, clock prescaler == 1.
}


inline void start_timer1_capture_falling(void)
{
    TCCR1B = (1<<ICNC1) | (1<<CS10);             // Enable noise canceler, interrupt on falling edge, clock prescaler == 1.
}


void start_timer1_ctc(time_t max)
{
    TCNT1 = 0;
    OCR1A = max;
    TCCR1B = (1<<WGM12) | (1<<CS10);             // CTC mode, clock prescaler == 1.
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


// Initialize the triac control.
void zcd_init(void)
{
    zcd_time_t zcd_calibration = zcd_calibrate();
    zcd_set(0);
    zcd_run(zcd_calibration);
    printf("Running triac with calibration of %li us." NEWLINE, zcd_calibration / (F_CPU / 1000000));
}


// Measure the offset from the rising edge of the ZCD to the true zero crossing.
// An oscilloscope trace of the ZCD pcb, used in the project can be found here:
// http://electronics.stackexchange.com/questions/201605/automatic-calibration-of-a-zero-cross-detector-latency
// It is evident, that the pulse is wide, but centered around the ZC.
// Therefore, we measure its width and divide it by two to get the offset from the rising edge to the true ZC.
time_t zcd_calibrate(void)
{
    // Restore interrupt status after exit.
    // Enable all interrupts here, because there were glitches when altering TIMSK1 form an ISR.
    char sreg = SREG;
    sei();
    TIMSK1 = (1<<ICIE1);                         // Enable edge interrupts.

    // Wait for rising edge of the zcd pulse.
    g_calibration_mode = true;
    start_timer1_capture_rising();
    while(!g_event_captured);
    g_event_captured = false;
    ATOMIC{ TCNT1 = 0; }   // Clear timer1.

    // Measure pulse width.
    ATOMIC{ start_timer1_capture_falling(); }
    while(!g_event_captured);
    volatile time_t count;
    ATOMIC{ count = ICR1; } // Width of the zcd pulse, in CPU cycles.
    stop_timer1();

    // Estimate offset from measured to real zc.
    time_t rising_to_zc = count / 2;             // In CPU cycles.

    SREG = sreg;
    return rising_to_zc;
}


void zcd_set(proc_val_t setpoint)
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
    TIMSK1 = (1<<ICIE1) | (1<<OCIE1A);           // Enable level and CTC interruprs.
    DDR_HEATER |= (1<<PIN_HEATER);

    g_calibration_mode = false;
    ATOMIC{ g_calibration = calibration; }
    start_timer1_capture_rising();
}


zcd_proc_val_t zcd_get(void)
{
    zcd_proc_val_t ret;
    ATOMIC { ret = g_setpoint; }
    return ret;
}
