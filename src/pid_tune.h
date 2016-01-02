// Used hardware: none.
// Used interrupts: none.
// Used software modules: tempr, usart, zcd.

#ifndef _PID_TUNE_H_
#define _PID_TUNE_H_


#include "tempr.h"

#include <stdint.h>


typedef int16_t pid_coeff_t;                     // 9s6 format i.e. 128 == 1.0
typedef int16_t pid_inout_t;
typedef struct {pid_coeff_t p, i, d;} pid_coeffs_t;

void* pid_create(pid_coeff_t p, pid_coeff_t i, pid_coeff_t d);
void pid_destroy(void *pid);
pid_inout_t pid_run(pid_inout_t setpoint, void *obj);
pid_coeff_t to_pid_coeff(int8_t coeff);
pid_coeffs_t  pid_tune_Zeigler_Nichols(void);


#endif  // defined(_PID_TUNE_H_)
