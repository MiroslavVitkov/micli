#ifndef _PID_TUNE_H_
#define _PID_TUNE_H_


#include "tempr.h"

#include <stdint.h>


typedef int16_t pid_coeff_t;
typedef decicelsius_t pid_inout_t;

void* pid_create(pid_coeff_t p, pid_coeff_t i, pid_coeff_t d);
pid_inout_t pid_run(void *obj);
void pid_tune_Zeigler_Nichols(void);


#endif  // defined(_PID_TUNE_H_)
