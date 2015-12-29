// Used hardware: none.
// Used interrupts: none.
// Used software modules: zcd, usart.

#ifndef _PID_TUNE_H_
#define _PID_TUNE_H_


#include "tempr.h"

#include <stdint.h>


typedef int16_t pid_coeff_t;                     // 9s6 format i.e. 128 == 1.0
typedef decicelsius_t pid_inout_t;

void* pid_create(pid_coeff_t p, pid_coeff_t i, pid_coeff_t d);
pid_inout_t pid_run(void *obj);
void pid_tune_Zeigler_Nichols(void);
pid_coeff_t to_pid_coeff(int8_t coeff);

#endif  // defined(_PID_TUNE_H_)
