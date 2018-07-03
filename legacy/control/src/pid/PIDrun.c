//Author//
//Miroslav Vitkov 2012
//sir.vorac(at)gmail(dot)com

//License//
//Beerware

#include "atmel/pid.h"
#include "PIDcontroller.h"

void PIDrun(PID_o *obj){
	processValue_t processValue, control, setpoint;

	setpoint = obj->config.setpoint();
	processValue = obj->config.getter();
	control = pid_Controller(setpoint, processValue, &obj->data.algData);
	obj->config.supervisor(&control);
	obj->config.setter(control);
}
