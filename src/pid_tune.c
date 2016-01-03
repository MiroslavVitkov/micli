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


pid_inout_t pid_run(pid_inout_t setpoint, void *obj)
{
    struct PID_DATA *pid = (struct PID_DATA*)obj;
    int16_t proc_val;
    ATOMIC
    {
        proc_val = tempr_get();
    }
    pid_inout_t control = pid_Controller(setpoint, proc_val, pid);
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
