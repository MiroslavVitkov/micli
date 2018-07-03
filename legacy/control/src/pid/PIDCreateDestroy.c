//Author//
//Miroslav Vitkov 2012
//sir.vorac(at)gmail(dot)com

//License//
//Beerware

#include "atmel/pid.h"
#include "PIDcontroller.h"

#include <stdlib.h>
#include <string.h>

error_t PIDcreate(PID_o **obj, const PIDconfig_s *const config){
	if(!config)
		return ERROR_NULL_POINTER;

	*obj = (PID_o*)malloc(sizeof(PID_o));
	if(*obj == NULL)
		return ERROR_ALLOCATION_FAILURE;

	memcpy((void*)&(**obj).config, (void*)config, sizeof(PIDconfig_s));
	(**obj).report.memFootprint = sizeof(PID_o);

	return SUCCESS;
}


