#include "pid_tune.h"
#include "usart.h"

#include "../libs/ds18x20/ds18x20.h"
#include "../libs/onewire/onewire.h"
#include "../libs/pid/pid.h"

#include <stdbool.h>
#include <stdlib.h>
#include <util/atomic.h>
#include <util/delay.h>


#define ATOMIC ATOMIC_BLOCK(ATOMIC_FORCEON)


pid_inout_t g_setpoint = 0;
struct PID_DATA g_pid;


void pid_config(pid_coeff_t p, pid_coeff_t i, pid_coeff_t d)
{
    pid_Init(p, i, d, &g_pid);
}


void pid_setpoint(pid_inout_t sp)
{
    g_setpoint = sp;
}


pid_inout_t pid_run(pid_inout_t proc_val)
{
    pid_inout_t control = pid_Controller(g_setpoint, proc_val, &g_pid);
    return control;
}


pid_coeff_t to_pid_coeff(int8_t coeff)
{
    return coeff * SCALING_FACTOR;
}


pid_coeffs_t pid_tune_Zeigler_Nichols(void)
{
    // Replace the pid controlelr with a relay.
    // The relay has no hysteresis and acts as a sign function.
    // The amplitude of the relay is selected to be 100 decicelsius.
    // The setpoint is selected to be 300 decicelsius.
    // Consistent and symmetrical oscillations should be observed.

    // Set P, I and D gains to 0.
    // Increase P until the output performs sustained oscilaltions.
//    void *pid = pid_create(0, 0, 0);
//    pid_destroy(pid);
    // Measure the gain Ku and the period Pu.
    // Consult the table in the literature.
    pid_coeffs_t result;
    return result;
}


int pid_wait_to_settle(pid_inout_t proc_val, pid_inout_t critical, pid_inout_t treshold, clock_seconds_t now)
{
    typedef struct
    {
        const bool is_min;
        pid_inout_t val;
        clock_seconds_t when;
    } extremum_t;

    static extremum_t min = {.is_min=true, .val=PID_INOUT_MAX, .when=0};   // TODO: those will need to te globals
    static extremum_t max = {.is_min=false, .val=PID_INOUT_MIN, .when=0};  // so that another function can reset them
    static extremum_t *prev = &min;

    if(prev->is_min)
    {
        // Look for a maximum.
        if(proc_val > max.val)
        {
            max.val = proc_val;
            max.when = now;
        }
    }
    else
    {
        if(proc_val < min.val)
        {
            min.val = proc_val;
            min.when = now;
        }
    }

    if(proc_val >= critical)  return PID_UNSTABLE;
    else if(max.val - min.val < treshold)  return PID_SETTLED;
    else return PID_OSCILLATING;
}

