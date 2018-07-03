//Author//
//Miroslav Vitkov 2013
//sir.vorac(at)gmail(dot)com

//License//
//Beerware

#ifndef LIB_CONTROL_ONOFF_CONTOLLER_H_
#define LIB_CONTROL_ONOFF_CONTOLLER_H_

//Includes//
#include "Types.h"
#include "PreprocConfig.h"

#include <stdlib.h>

//Typedefs//
typedef struct{
	//This sets the controller output (which is input to the plant).
	void (*setter)(processValue_t);

	//getter() returns the output of the plant, realising closed-loop feedback.
	processValue_t (*getter)(void);

	//setpoint() is provided as a function as to make the control tracking, not just stabilizing.
	//Note, however, that calibration is done for what this function returns at the time of calibration.
	//Therefore, if the setpoint deviates significantly (whaever that means), calibration should be
	//repeated.
	processValue_t (*setpoint)(void);

	//The supervisor() examines the output of the contoller, before that is applied. Possible uses include
	//clipping, rate limitting.
	void (*supervisor)(processValue_t*);

	//Slightly duplicating the functionality of supervisor(), maxSafePlantOutput is intended as the absolute
	//maximum persmissible plant output for even a moment, any higher could lead to damage.
	processValue_t maxSafePlantOutput;
	
	//The hysteresis parameter.
	processValue_t hyst;
}ONOFFconfig_s;

typedef struct{
	//This is the approximate heap footprint of the object plus all of it's child objects.
	memAddr_t memFootprint;
}ONOFFreport_s;

typedef struct{
	processValue_t previousOutput;
}ONOFFprivateData_s;

typedef struct{
	const ONOFFconfig_s config;
	ONOFFreport_s report;
	ONOFFprivateData_s data;
}ONOFF_o;

//Functions//
error_t ONOFFcreate(ONOFF_o **obj, const ONOFFconfig_s *const config);
void ONOFFrun(ONOFF_o *obj);					//do one onoff iteration and set system output accordingly
inline void ONOFFdestroy(ONOFF_o *obj);

///////////////////// Implementation of inlined functions ////////////////////////////////////////////////
inline void  ONOFFdestroy(ONOFF_o *obj){
	free(obj);
}

#endif //LIB_CONTROL_ONOFF_CONTOLLER_H_
