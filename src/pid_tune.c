#include "pid_tune.h"
#include "usart.h"
#include "zcd.h"

#include "../libs/ds18x20/ds18x20.h"
#include "../libs/onewire/onewire.h"
#include "../libs/pid/pid.h"

#include <assert.h>
#include <stdbool.h>
#include <util/atomic.h>
#include <util/delay.h>


#define ATOMIC ATOMIC_BLOCK(ATOMIC_FORCEON)


// Because the output waveform is computed synchronously with the mains zero-crossing,
// but temperature measurement and pid calculation happen synchronously with the cpu,
// there is a need to synchronise between those processes.
// This variable stores the latest measured value of the temperature.
decicelsius_t g_temperature;


void pid_tune_Zeigler_Nichols(void)
{
    // Set P, I and D gains to 0.
    // Inkrease P until the output performs sustained oscilaltions.
    // Measure the gain Ku and the period Pu.
    // Consult the table in the literature.
    //PIDgains_s result;
    //return result;
}


void pid_setter(decicelsius_t val)
{
    zcd_proc_val_t cast = (zcd_proc_val_t)val;
    zcd_adjust_setpoint(cast);
}


void* pid_create(pid_coeff_t p, pid_coeff_t i, pid_coeff_t d)
{
    // Initialize the triac control.
    zcd_time_t zcd_calibration = zcd_calibrate();
    zcd_adjust_setpoint(0);
    zcd_run(zcd_calibration);

    // Allocate memory here, because the outside world doens't know about PID_DATA.
    struct PID_DATA pids[1];
    struct PID_DATA *pid = &pids[0];

    // Initialize the pid algorithm.
    pid_Init(p, i, d, pid);

    return pid;
}

pid_inout_t pid_run(void *obj)
{
    struct PID_DATA *pid = (struct PID_DATA*)obj;
    int16_t setpoint = 220, proc_val;
    ATOMIC
    {
        proc_val = tempr_get();
    }
    pid_inout_t control = pid_Controller(setpoint, proc_val, pid);
    return control;
}

