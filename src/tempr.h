#ifndef TEMPR_H_
#define TEMPR_H_


#include <stdint.h>


typedef int16_t decicelsius_t;
enum{
        //SUCCESS = 0,
        ERROR_NO_DEVICE_FOUND = 1,
        ERROR_START_MEASUREMENT,
        ERROR_READ_TEMPERATURE,
};


void tempr_init(void);
void tempr_measure(void);
decicelsius_t tempr_get(void);


#endif  // defined(TEMPR_H_)
