#include "pid_tune.h"
#include "usart.h"

#include "../libs/ds18x20/ds18x20.h"
#include "../libs/onewire/onewire.h"
#include "../libs/pid/pid.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <util/atomic.h>
#include <util/delay.h>


#define ATOMIC ATOMIC_BLOCK(ATOMIC_FORCEON)
typedef enum pid_state_e
{
    SETTLED,
    OSCILLATING,
    UNSTABLE,
} pid_state_t;


typedef struct
{
    const bool is_min;
    pid_inout_t val;
    clock_seconds_t when;
} extremum_t;
typedef struct
{
    bool ready;
    extremum_t min, max;
    extremum_t *curr, *prev;
    pid_coeff_t p, i, d;
} pid_tune_t;


pid_inout_t pid_onoff_controller(pid_inout_t proc_val, pid_inout_t sp, pid_inout_t ampl);
pid_state_t pid_wait_to_settle(pid_inout_t proc_val, pid_inout_t critical, pid_inout_t treshold, clock_seconds_t now);


pid_inout_t g_setpoint = 0;
struct PID_DATA g_pid;
pid_tune_t g_pid_tune =
{
    .ready = false,
    .min = {.is_min=true, .val=PID_INOUT_MAX, .when=0},
    .max = {.is_min=false, .val=PID_INOUT_MIN, .when=0},
    .curr = &g_pid_tune.min,
    .prev = &g_pid_tune.max,
    .p = 0,
    .i = 0,
    .d = 0,
};

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


bool pid_tune_finished(void)
{
    return g_pid_tune.ready;
}


void pid_get_coeffs(pid_coeff_t *p, pid_coeff_t *i, pid_coeff_t *d)
{
    assert(g_pid_tune.ready);
    *p = g_pid_tune.p;
    *i = g_pid_tune.i;
    *d = g_pid_tune.d;
}


// Astrom-Hagglund
// Second method of Ziegler-Nichols
pid_inout_t pid_tune_Ziegler_Nichols(pid_inout_t proc_val, clock_seconds_t now)
{
printf("zn"NEWLINE);
    assert(!g_pid_tune.ready);

    pid_inout_t ctrl;

    // Evaluate relay control only each PID_CONTROL_LOOP_SECONDS.
    const pid_inout_t relay_ampl = 400;
    static unsigned loop_counter = 0;
    static pid_inout_t prev_ctrl = 0;
    if(loop_counter++ % PID_CONTROL_LOOP_SECONDS)
    {
        ctrl = prev_ctrl;
    }
    else
    {
        ctrl = pid_onoff_controller(proc_val, 300, relay_ampl);
        prev_ctrl = ctrl;
    }

    pid_state_t osc = pid_wait_to_settle(proc_val, 600, 10, now);
    assert(osc == OSCILLATING);

    clock_seconds_t patience = 600;
    if(now > patience)
    {
        g_pid_tune.ready = true;
        pid_inout_t a = g_pid_tune.max.val - g_pid_tune.min.val;
        pid_inout_t pi = 31;  // decicelsius?
        pid_inout_t Ku = (4 * relay_ampl) / (pi * a);
        clock_seconds_t Tu = 2 * (g_pid_tune.curr->when - g_pid_tune.prev->when);
        g_pid_tune.p = 6 * Ku;  // decicelsius?
        g_pid_tune.i = g_pid_tune.p / (5 * Tu);
        g_pid_tune.d = g_pid_tune.p / (1 * Tu);
    }

    return ctrl;
}


pid_inout_t pid_onoff_controller(pid_inout_t proc_val, pid_inout_t sp, pid_inout_t ampl)
{
    if(proc_val > sp)
    {
        return 0;
    }
    else
    {
        return ampl;
    }
}


pid_state_t pid_wait_to_settle(pid_inout_t proc_val, pid_inout_t critical, pid_inout_t treshold, clock_seconds_t now)
{
    extremum_t *min = &g_pid_tune.min;
    extremum_t *max = &g_pid_tune.max;
    extremum_t **curr = &g_pid_tune.curr;
    extremum_t **prev = &g_pid_tune.prev;

    if(*prev == min)
    {
        // Look for a maximum.
        if(proc_val > (*max).val)
        {
            (*max).val = proc_val;
            (*max).when = now;
            *curr = max;
            *prev = min;
        }
    }
    else
    {
        if(proc_val < (*min).val)
        {
            (*min).val = proc_val;
            (*min).when = now;
            *curr = min;
            *prev = max;
        }
    }

    if(proc_val >= critical)  return UNSTABLE;
    else if((*max).val - (*min).val < treshold)  return SETTLED;
    else return OSCILLATING;
}
