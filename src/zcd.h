// Used hardware: ICP1 pin. timer1.
// Used interrupts: TIMER1_CAPT_vect, TIMER1_COMPA_vect.

#ifndef _ZCD_H_
#define _ZCD_H_


#include <stdint.h>


#define ZCD_PROC_VAL_MAX (UINT16_MAX)


typedef uint16_t zcd_time_t;
typedef uint16_t zcd_proc_val_t;


zcd_time_t zcd_calibrate(void);
void zcd_adjust_setpoint(zcd_proc_val_t sp);
void zcd_run(zcd_time_t calibration);


#endif  // ifndef _ZCD_H_
