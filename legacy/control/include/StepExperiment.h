//Author//
//Miroslav Vitkov 2012
//sir.vorac(at)gmail(dot)com

//License//
//Beerware

//RECOM: run in interrupt-free enviroment for time cosntants in the us and ms range

#ifndef LIB_CONTROL_STEP_EXPERIMENT_H_
#define LIB_CONTROL_STEP_EXPERIMENT_H_

//Includes//
#include "Types.h"
#include "PreprocConfig.h"
#include <stdlib.h>

//Typedefs//

//For the meanings of thiese parameters, as well as advices as to how the write the appropriate
//callback functions, please refer to PIDcontroller.h
typedef struct{
	void (*setter)(processValue_t);
	processValue_t (*getter)(void);
	timeUs_t (*getTimeUs)(void);
	processValue_t maxSafePlantOutput;
}stepConfig_s;

typedef struct{
	timeUs_t pureDelayUs;			// == time from step to increase of process output to  5% of setpoint
	timeUs_t riseTimeUs;			// == time from step to increase of process output to 95% of setpoint
	timeUs_t settingTimeUs;			// == time from step to when process output is stable to within 5% of setpoint
	float_t overshoot;			// == (max(responses[]) / setpoint) - 1
	processValue_t bias; 			// == process output when input is zero
	processValue_t settingPoint;		//plant output after transients have passed
	processValue_t stepSize;		//
	memAddr_t memFootprint;
}stepReport_s;

typedef struct{
}stepPrivateData_s;

typedef struct{
	const stepConfig_s config;
	stepReport_s report;
	stepPrivateData_s data;
	memAddr_t memFotprint;
}step_o;

//Functions//
error_t stepCreate(step_o **obj, const stepConfig_s *config);
inline void stepDestroy(step_o * obj);

//measures if the system is stable and if so, what is the setpoint
error_t stepBasicRun(step_o * obj, processValue_t stepSize);	

//performs detailed measurement, based on the data of the primary run
error_t stepSecondaryRun(step_o * obj);	

//Block until process extremums deviate less than 5% from the setting point.
error_t wait2settle(const step_o *const obj);

//////////////////////////// Implementation of inline functions ///////////////////////////////////////

inline void stepDestroy(step_o *const obj){
	free(obj);
}

#endif	//LIB_CONTROL_STEP_EXPERIMENT_H_
