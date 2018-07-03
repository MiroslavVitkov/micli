//Author//
//Miroslav Vitkov 2012
//sir.vorac(at)gmail(dot)com

//License//
//Beerware

#ifndef INC_LIB_CONTROL_PREPROC_CONFIG_H
#define INC_LIB_CONTROL_PREPROC_CONFIG_H

#include "Types.h"
#include "../demos/hardwareTest/usart.h"
//PID

//(feedthroughGain) * (feedback gain) = PID_CALIBRATION_INITIAL_GUESS_RELATIVE_GAIN
//				 	> 1	:	isntability
//					=	:	sustaindef oscillation
//					< 1	:	stability
//#define PID_CALIBRATION_INITIAL_GUESS_RELATIVE_GAIN 0.5
#define LIB_CONTROL_PID_CALIBRATION_INITIALGUESS_RELATIVE_GAIN (int2float(1) / 2)

//To capture debug dumps, define a print function and call it in this macro. If not
//defined or defined as empty, no machine instructions are generated.
#define CONTROL_DEBUG_PRINT(X) USARTtransmitBlock((unsigned char *)&X, sizeof(X));
#define CONTROL_DEBUG_TAG(X) {USARTtransmit(X);}
//inline void CONTROL_DEBUG_TAG(uint8_t X) {USARTtransmit(X);}
#ifndef CONTROL_DEBUG_PRINT
#define CONTROL_DEBUG_PRINT(X)
#endif

//Define values to various debugging events. Empty value generates no instrucions.
#define CONTROL_DEBUG_TRACE_STEP_BASIC_RUN_0 210
#define CONTROL_DEBUG_TRACE_STEP_BASIC_RUN_1 211
#define CONTROL_DEBUG_TRACE_STEP_BASIC_RUN_2 212
#define CONTROL_DEBUG_TRACE_STEP_BASIC_RUN_3 213
#define CONTROL_DEBUG_TRACE_STEP_BASIC_RUN_4 214
#define CONTROL_DEBUG_TRACE_STEP_BASIC_RUN_5 215

#define CONTROL_DEBUG_TRACE_STEP_WAIT_TO_SETTLE_0 220
#define CONTROL_DEBUG_TRACE_STEP_WAIT_TO_SETTLE_1 221
#define CONTROL_DEBUG_TRACE_STEP_WAIT_TO_SETTLE_2 222
#define CONTROL_DEBUG_TRACE_STEP_WAIT_TO_SETTLE_3 223
#define CONTROL_DEBUG_TRACE_STEP_WAIT_TO_SETTLE_4 224
#define CONTROL_DEBUG_TRACE_STEP_WAIT_TO_SETTLE_5 225
#define CONTROL_DEBUG_TRACE_STEP_WAIT_TO_SETTLE_6 226
#define CONTROL_DEBUG_TRACE_STEP_WAIT_TO_SETTLE_7 227


#endif	//INC_LIB_CONTROL_PREPROC_CONFIG_H
