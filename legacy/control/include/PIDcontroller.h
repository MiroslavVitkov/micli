//Author//
//Miroslav Vitkov 2011
//sir.vorac(at)gmail(dot)com

//License//
//Beerware

#ifndef LIB_CONTROL_PID_CONTOLLER_H_
#define LIB_CONTROL_PID_CONTOLLER_H_

//Includes//
#include "atmel/pid.h" 
#include "Types.h"
#include "PreprocConfig.h"

#include <stdlib.h>

//Typedefs//
typedef struct{
	int16_t p;
	int16_t i;
	int16_t d;
}PIDgains_s;

typedef struct{
	//This sets the controller output (which is input to the plant). processValue_t[min-max] should
	//generally be mapped to [0-Vcc].
	void (*setter)(processValue_t);

	//getter() returns the output of the plant, realising closed-loop feedback. For advice on mapping
	//voltages to variable values, please refer to the comment regarding setter() above.
	processValue_t (*getter)(void);

	timeUs_t (*getTimeUs)(void);

	//setpoint() is provided as a function as to make the control tracking, not just stabilizing.
	//Note, however, that calibration is done for what this function returns at the tiem of calibration.
	//Therefore, if the setpoint deviates significantly (whaever that means) calibration should be
	//repeated.
	processValue_t (*setpoint)(void);

	//The supervisor() examines the output of the contoller, before that is applied. Possible uses include
	//clipping, rate limitting.
	void (*supervisor)(processValue_t*);

	//Slightly duplicating the functionality of supervisor(), maxSafePlantOutput is intended as the absolute
	//maximum persmissible plant output for even a moment, any higher could lead to damage.
	processValue_t maxSafePlantOutput;

	//This is the period of calling the controller. If changed, I and D gains have to be
	//changed either through recalibration, or by:
	// * dividing I by the relative increase of T AND
	// * multiplying D by the relative increase if T.
	timeUs_t T;
}PIDconfig_s;

typedef struct{
	//P, I and D coefficients of the controller. Thiese are acquired through calibration or passed in
	//externally. AFTER that PIDinit() has to be called to push thiese coefficients to the actual
	//controller. After taht, calling PIDinit() at any given time resets the controller and pushes in
	//the CURRENT (maybe changed through calibration or extrenally) coefficients to it. Have fun!
	PIDgains_s gains;

	//This is the approximate heap footprint of the object plus all of it's child objects.
	memAddr_t memFootprint;
}PIDreport_s;

typedef struct{
	pidData_t algData;
}PIDprivateData_s;

typedef struct{
	const PIDconfig_s config;
	PIDreport_s report;
	PIDprivateData_s data;
}PID_o;

//Functions//
error_t PIDcreate(PID_o **obj, const PIDconfig_s *const config);
inline void PIDdestroy(PID_o *obj);

inline void PIDloadGains(PID_o *obj, PIDgains_s gains);
error_t PIDcalibrateInitialGuess(PID_o *const pid);	//fetch useful pid gains

inline void PIDinit(PID_o *obj);			//push loaded gains to the controller
void PIDrun(PID_o *obj);				//do one pid iteration

///////////////////// Implementation of inlined functions ////////////////////////////////////////////////
inline void  PIDinit(PID_o *obj){
	pid_Init(obj->report.gains.p, obj->report.gains.i, obj->report.gains.d, &obj->data.algData);
}

inline void  PIDdestroy(PID_o *obj){
	free(obj);
}

inline void PIDloadGains(PID_o *obj, PIDgains_s g){
	obj->report.gains.p = g.p;
	obj->report.gains.i = g.i;
	obj->report.gains.d = g.d;
}

#endif //LIB_CONTROL_PID_CONTOLLER_H_
