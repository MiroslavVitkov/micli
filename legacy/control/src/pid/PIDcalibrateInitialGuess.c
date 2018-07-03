//Author//
//Miroslav Vitkov 2012
//sir.vorac(at)gmail(dot)com

//License//
//Beerware

//Summary//
/* First, we estimate the first order model gain of the plant near the setpoint. Next, a P controller
 * is selected that satisfies kb < 1, where k - process gain, b - controller gain. Lastly,
 * I and D gains are selected according to an inverted Ziegler–Nichols table.
 *
 * This method is not reliable (actually I just made it up) and further refinement of the coefficients
 * is recommended. Stability is not guaranteed, neither is quality of transient processes.
 */

#include "PIDcontroller.h"
#include "StepExperiment.h"
#include "LinearModel.h"
#include "atmel/pid.h"
#include "PreprocConfig.h"

//SCALING_FACTOR defined in atmel/pid.h
static inline int16_t PIDf2int16(float_t f){
	return (float2int(f) * (SCALING_FACTOR));
}

error_t PIDcalibrateInitialGuess(PID_o *const pid){

	error_t ret;
	processValue_t magnitude;
	step_o *step = NULL;
	stepConfig_s cStep;
	cStep.setter = pid->config.setter;
	cStep.getter = pid->config.getter;
	cStep.getTimeUs = pid->config.getTimeUs;
	cStep.maxSafePlantOutput = pid->config.maxSafePlantOutput;
	ret = stepCreate(step, &cStep);
	if(ret)
		return ret;
	pid->report.memFootprint += step->report.memFootprint;

	//1. Find linear gain at the setpoint. Use binary search.
	for(magnitude = 1; step->report.overshoot <= 0; magnitude *= 2){
		ret = stepBasicRun(step, magnitude);//magnitude is increased by a power of two, overshoot is observed
		if(ret != SUCCESS){
			//handle error
			return ret;
		}
		ret = stepSecondaryRun(step);
		if(ret != SUCCESS){
			//handle error
			return ret;
		}
	}
	
	//2. Pass the collected data to the model formulator and kill stepExperiment.
	linConfig_s zeroLinConf;
	lin_o *model;
	linCreate(model, &zeroLinConf);
	ret = linFirstOrder(step->report.settingPoint, step->report.bias, step->report.stepSize, step->report.settingTimeUs, model);

	pid->report.memFootprint -= step->report.memFootprint;
	stepDestroy(step);

	//3. Produce a P controller that drives the plant into the setpoint.
	//(feedthroughGain) * (feedback gain) 	> 1	:	isntability
	//					=	:	sustaindef oscillation
	//					< 1	:	stability
	processValue_t temp;
	temp = LIB_CONTROL_PID_CALIBRATION_INITIALGUESS_RELATIVE_GAIN / float2int(model->report.gain * SCALING_FACTOR);
	pid->report.gains.p = float2int(temp * SCALING_FACTOR * SCALING_FACTOR);

	//4. Using modified Zeigler-Nichols table, select I and D gains.
	timeUs_t criticalPeriod = 3 * model->report.timeConstant;	//very approximately - three oscillations
	double_t precise;
	precise = ((pid->report.gains.p * 2 * pid->config.T) / criticalPeriod);
	pid->report.gains.i = precise;
	precise = (pid->report.gains.p * criticalPeriod) / (3 * pid->config.T);
	pid->report.gains.d = precise;

	model->report.memFootprint -= model->report.memFootprint;
	linDestroy(model);

	return SUCCESS;
}
