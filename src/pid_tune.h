// Used hardware: none.
// Used interrupts: none.

#ifndef _PID_TUNE_H_
#define _PID_TUNE_H_


#include "clock.h"
#include "tempr.h"

#include <stdbool.h>
#include <stdint.h>


#define PID_COEFF_MAX (INT16_MAX)
#define PID_INOUT_MAX (INT16_MAX)
#define PID_INOUT_MIN (INT16_MIN)
typedef enum pid_state_e
{
    PID_SETTLED,
    PID_OSCILLATING,
    PID_UNSTABLE,
} pid_state_t;

typedef int16_t pid_coeff_t;                     // 9s6 format i.e. 128 == 1.0
typedef int16_t pid_inout_t;


pid_coeff_t to_pid_coeff(int8_t coeff);
void pid_config(pid_coeff_t p, pid_coeff_t i, pid_coeff_t d);
void pid_setpoint(pid_inout_t sp);
pid_inout_t pid_run(pid_inout_t proc_val);

pid_inout_t pid_tune_Ziegler_Nichols(pid_inout_t proc_val, clock_seconds_t now);
bool pid_tune_finished(void);
void pid_get_coeffs( pid_coeff_t *p, pid_coeff_t *i, pid_coeff_t *d);


#endif  // defined(_PID_TUNE_H_)
