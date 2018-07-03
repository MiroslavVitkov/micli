//Author//
//Miroslav Vitkov 2012
//sir.vorac(at)gmail(dot)com

//License//
//Beerware

#include "LinearModel.h"

error_t linFirstOrder(processValue_t settingPoint, processValue_t bias, processValue_t control, timeUs_t settingTimeFivePercent, lin_o *obj){
	obj->report.gain = int2float(settingPoint - bias) / int2float(control);
	obj->report.timeConstant = settingTimeFivePercent / LIB_CONTROL_MINUS_LN05;

	return SUCCESS;
}


