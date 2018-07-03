//Author//
//Miroslav Vitkov 2012
//sir.vorac(at)gmail(dot)com

//License//
//Beerware

#include "PreprocConfig.h"
#include "StepExperiment.h"

//Measures if the system is stable and if so, what is the setpoint.
//Thiese are rough "quick and dirty" measurements.
//As we don't know the setting point, but need it in order to detect that the 
//process has settled within five percent, the maximum allowed plant
//output is used in it's place.
error_t stepBasicRun(step_o * obj, processValue_t stepSize){

	//Send a step signal
	CONTROL_DEBUG_TAG(CONTROL_DEBUG_TRACE_STEP_BASIC_RUN_0);
	error_t status; 
	obj->config.setter(stepSize);

	//Set object report to a safe state
	CONTROL_DEBUG_TAG(CONTROL_DEBUG_TRACE_STEP_BASIC_RUN_1);
	obj->report.settingPoint = obj->config.maxSafePlantOutput;	//safer if system unstable
	obj->report.stepSize = stepSize;

	//Wait to see if the system settles, or goes unbound
	CONTROL_DEBUG_TAG(CONTROL_DEBUG_TRACE_STEP_BASIC_RUN_2);
 	status =  wait2settle(obj);
	
	//Probably system unstabke
	CONTROL_DEBUG_TAG(CONTROL_DEBUG_TRACE_STEP_BASIC_RUN_3);
	if(status != SUCCESS)
		return status;

	//Record the result
	CONTROL_DEBUG_TAG(CONTROL_DEBUG_TRACE_STEP_BASIC_RUN_4);
	obj->report.settingPoint = obj->config.getter();

	//Done
	CONTROL_DEBUG_TAG(CONTROL_DEBUG_TRACE_STEP_BASIC_RUN_5);
	return SUCCESS;
}
