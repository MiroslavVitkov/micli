//Author//
//Miroslav Vitkov 2012
//sir.vorac(at)gmail(dot)com

//License//
//Beerware

#include "PreprocConfig.h"
#include "StepExperiment.h"

typedef enum{
	minimum = 0,
	maximum = 1,
}extremumType_e;
typedef int8_t extremumType_t;

typedef struct{
	extremumType_t type;
	processValue_t value;
}extremum_s;

//When direction of change switches, this is an extremum. Compare it's value with the prevous extremum recorded.
//If difference is within 2 * fivePercent(setpoint), output is considered to have settled.
error_t wait2settle(const step_o *const obj){

	//Initial values. Set a safe value for a fictional previous extremum.

	CONTROL_DEBUG_TAG(CONTROL_DEBUG_TRACE_STEP_WAIT_TO_SETTLE_0);
	processValue_t processOutput, processPreviousOutput;
	extremum_s lastExtremum;
	lastExtremum.type = minimum;
	lastExtremum.value = 0;
	processPreviousOutput = 0;

	//Main loop. Each iteration consists of:
	//	* read process output
	//	* check if it is becoming unbounted
	//	* check for an extremum of the opposite type of the previously cncountered 
	CONTROL_DEBUG_TAG(CONTROL_DEBUG_TRACE_STEP_WAIT_TO_SETTLE_1);
	do{
		processOutput = obj->config.getter();
		if(processOutput > obj->config.maxSafePlantOutput)
			return ERROR_PROCESS_OUTPUT_HIGHER_THAN_ALLOWED;

		if(lastExtremum.type == minimum){	//look for a maximum
			if(processOutput < processPreviousOutput){

				//A new extremum has been detected. If it is close to the previous extremum 
				//(of opposite type), the output has settled. The function is successful.
				// Otherways just record it.
				//WARN: It is assumed the signal is noise-free (preconditioned).
				CONTROL_DEBUG_TAG(CONTROL_DEBUG_TRACE_STEP_WAIT_TO_SETTLE_2);
				if(processOutput - lastExtremum.value < fivePercent(obj->report.settingPoint)){

					//Extremums close to one another - samll assumed amplitude.
					// Function returns successfully.
					CONTROL_DEBUG_TAG(CONTROL_DEBUG_TRACE_STEP_WAIT_TO_SETTLE_3);
					return SUCCESS;
				}
				else{
					//Large amplitude. Rememeber extremum type and value and 
					//continue to wait for next extremum.
					CONTROL_DEBUG_TAG(CONTROL_DEBUG_TRACE_STEP_WAIT_TO_SETTLE_4);
					lastExtremum.value = (processPreviousOutput + processOutput) / 2;
					lastExtremum.type = maximum;
				}
			}
		}else{			//look for a minimum
			if(processOutput > processPreviousOutput){

				//Found a minimum
				CONTROL_DEBUG_TAG(CONTROL_DEBUG_TRACE_STEP_WAIT_TO_SETTLE_5);
				if(lastExtremum.value - processOutput < fivePercent(obj->report.settingPoint)){
					
					//Small amplitude
					CONTROL_DEBUG_TAG(CONTROL_DEBUG_TRACE_STEP_WAIT_TO_SETTLE_6);
					return SUCCESS;
				}
				else{
					//Large amplitude
					CONTROL_DEBUG_TAG(CONTROL_DEBUG_TRACE_STEP_WAIT_TO_SETTLE_7);
					lastExtremum.value = (processPreviousOutput + processOutput) / 2;
					lastExtremum.type = minimum;
				}
			}
		}
		processPreviousOutput = processOutput;
	}while(1);
}
