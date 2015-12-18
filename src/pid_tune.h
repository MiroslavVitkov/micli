#ifndef _PID_TUNE_H_
#define _PID_TUNE_H_


#include "PIDcontroller.h"


enum{
        //SUCCESS = 0,
        ERROR_NO_DEVICE_FOUND = 1,
        ERROR_START_MEASUREMENT,
        ERROR_READ_TEMPERATURE,
};


PIDgains_s pid_tune_Zeigler_Nichols(void);
void pid_run(void);


#endif  // defined(_PID_TUNE_H_)
