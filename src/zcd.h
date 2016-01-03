// Used hardware: ICP1 pin. timer1.
// Used interrupts: TIMER1_CAPT_vect, TIMER1_COMPA_vect.

#ifndef _ZCD_H_
#define _ZCD_H_


#include <stdint.h>


#define ZCD_PROC_VAL_MAX (UINT16_MAX)


typedef uint16_t zcd_time_t;
typedef uint16_t zcd_proc_val_t;

// Interrupts are enabled upon return.
void zcd_init(void);

// Values are in range [0, 2^16] and map to [0, 100]% .
void zcd_set(zcd_proc_val_t setpoint);

// Get current setpoint in [0, 2^16] range.
zcd_proc_val_t zcd_get(void);

#endif  // ifndef _ZCD_H_
