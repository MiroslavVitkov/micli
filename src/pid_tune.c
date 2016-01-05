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


void* pid_create(pid_coeff_t p, pid_coeff_t i, pid_coeff_t d)
{
    // Allocate memory here, because the outside world doens't know about PID_DATA.
    struct PID_DATA *pid = malloc(sizeof(struct PID_DATA));

    // Initialize the pid algorithm.
    pid_Init(p, i, d, pid);

    return pid;
}


void pid_destroy(void *pid)
{
    free(pid);
}


void pid_setpoint(pid_inout_t sp)
{
    g_setpoint = sp;
}


pid_inout_t pid_run(void *obj)
{
    struct PID_DATA *pid = (struct PID_DATA*)obj;
    int16_t proc_val;
    ATOMIC
    {
        proc_val = tempr_get();
    }
    pid_inout_t control = pid_Controller(g_setpoint, proc_val, pid);
    return control;
}


pid_coeff_t to_pid_coeff(int8_t coeff)
{
    return coeff * SCALING_FACTOR;
}


pid_coeffs_t pid_tune_Zeigler_Nichols(void)
{
    // Set P, I and D gains to 0.
    // Increase P until the output performs sustained oscilaltions.
    void *pid = pid_create(0, 0, 0);
    pid_destroy(pid);
    // Measure the gain Ku and the period Pu.
    // Consult the table in the literature.
    pid_coeffs_t result;
    return result;
}


enum  // TODO: name the enum, prefix values, move to header
{
    SETTLED,
    OSCILLATING,
    UNSTABLE,
};
int pid_wait_to_settle(pid_inout_t i, pid_inout_t critical, pid_inout_t treshold, clock_seconds_t now)
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
        if(i > max.val)
        {
            max.val = i;
            max.when = now;
        }
    }
    else
    {
        if(i < min.val)
        {
            min.val = i;
            min.when = now;
        }
    }

    if(i >= critical)  return UNSTABLE;
    else if(max.val - min.val < treshold)  return SETTLED;
    else return OSCILLATING;
}

