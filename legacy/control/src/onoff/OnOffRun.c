//Author//
//Miroslav Vitkov 2013
//sir.vorac(at)gmail(dot)com

//License//
//Beerware

#include "OnOffController.h"

void ONOFFrun(ONOFF_o *obj){
	processValue_t procVal, control, setpoint;

	setpoint = obj->config.setpoint();
	procVal = obj->config.getter();
	
	processValue_t overshoot = procVal - setpoint;
	if(overshoot > obj->config.hyst){
		control = 0;
	}else if(overshoot < -obj->config.hyst){
		control = obj->config.maxSafePlantOutput;
	}else{
		control = obj->data.previousOutput;
	}
	
	obj->config.supervisor(&control);
	obj->config.setter(control);
	obj->data.previousOutput = control;
}
