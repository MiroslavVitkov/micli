// Used hardware: none.
// Used interrupts: none.
// Used software modules: tempr, usart, zcd.

#ifndef _PID_TUNE_H_
#define _PID_TUNE_H_


#include "clock.h"
#include "tempr.h"

#include <stdint.h>


#define PID_COEFF_MAX (INT16_MAX)
#define PID_INOUT_MAX (INT16_MAX)
#define PID_INOUT_MIN (INT16_MIN)


typedef int16_t pid_coeff_t;                     // 9s6 format i.e. 128 == 1.0
typedef int16_t pid_inout_t;
typedef struct {pid_coeff_t p, i, d;} pid_coeffs_t;

void* pid_create(pid_coeff_t p, pid_coeff_t i, pid_coeff_t d);
void pid_destroy(void *pid);
void pid_setpoint(pid_inout_t sp);
pid_inout_t pid_run(void *obj);
pid_coeff_t to_pid_coeff(int8_t coeff);
int pid_wait_to_settle(pid_inout_t i, pid_inout_t critical, pid_inout_t treshold, clock_seconds_t now);
pid_coeffs_t  pid_tune_Zeigler_Nichols(void);


#endif  // defined(_PID_TUNE_H_)
