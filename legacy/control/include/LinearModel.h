//Author//
//Miroslav Vitkov 2012
//sir.vorac(at)gmail(dot)com

//License//
//Beerware

#ifndef LIB_CONTROL_LINEAR_MODEL_H_
#define LIB_CONTROL_LINEAR_MODEL_H_

//Includes//
#include "Types.h"
#include "PreprocConfig.h"
#include <stdlib.h>

//Typedefs//
typedef struct{
}linConfig_s;

typedef struct{
	float_t gain;		// == setpoint / magnitude
	timeUs_t timeConstant;

	float_t resonantFreq;
	float_t dampRatio;

	memAddr_t memFootprint;
}linReport_s;

typedef struct{
}linPrivateData_s;

typedef struct{
	const linConfig_s config;
	linReport_s report;
	linPrivateData_s data;
	memAddr_t memFotprint;
}lin_o;

//Functions//
error_t linCreate(lin_o **obj, const linConfig_s *const config);
inline void linDestroy(lin_o *obj);

//linFirstOrder() computes gain and timeConstant according to a firt order process model:
//y = 1 - exp(- t / T)
//Constant control input is applied to the plant. The it is waited for the transients to suside.
//settingPoint - the plant output after a long duration of constant input
//bias - plant output when control input is zero
//control - the long maintained control input
//settingTimeFivePercent - the time needed the system to reach from rest (zero input) the 0.95 of settingPoint
error_t linFirstOrder(processValue_t settingPoint, processValue_t bias, processValue_t control, timeUs_t settingTimeFivePercent, lin_o *obj);

error_t linSecondOrder(void);

///////////////////// Implementation of inlined functions ////////////////////////////////////////////////
inline void  linDestroy(lin_o *obj){
	free(obj);
}


#endif	//LIB_CONTROL_LINEAR_MODEL_H_
