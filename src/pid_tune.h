#ifndef _PID_TUNE_H_
#define _PID_TUNE_H_


#include "PIDcontroller.h"


typedef processValue_t decicelsius_t;
enum{
        //SUCCESS = 0,
        ERROR_NO_DEVICE_FOUND = 1,
        ERROR_START_MEASUREMENT,
        ERROR_READ_TEMPERATURE,
};


PID_o* pid_create(void);
void pid_run(PID_o*);
decicelsius_t pid_get_tempr(void);
PIDgains_s pid_tune_Zeigler_Nichols(void);


#endif  // defined(_PID_TUNE_H_)
