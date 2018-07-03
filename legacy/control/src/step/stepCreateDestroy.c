//Author//
//Miroslav Vitkov 2012
//sir.vorac(at)gmail(dot)com

//License//
//Beerware

#include "StepExperiment.h"
#include <stdlib.h>
#include <string.h>

error_t stepCreate(step_o **obj, const stepConfig_s *config){
	if(!config)
		return ERROR_NULL_POINTER;

	*obj = (step_o*)malloc(sizeof(step_o));
	if(*obj == NULL)
		return ERROR_ALLOCATION_FAILURE;

	memcpy(&(**obj).config, config, sizeof(stepConfig_s));
	(**obj).report.memFootprint = sizeof(step_o);

	return SUCCESS;
}
