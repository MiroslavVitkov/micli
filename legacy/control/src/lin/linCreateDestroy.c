//Author//
//Miroslav Vitkov 2012
//sir.vorac(at)gmail(dot)com

//License//
//Beerware

#include "LinearModel.h"

#include <stdlib.h>
#include <string.h>

error_t linCreate(lin_o **obj, const linConfig_s *const config){
	if(!config)
		return ERROR_NULL_POINTER;

	*obj = (lin_o*)malloc(sizeof(lin_o));
	if(*obj == NULL)
		return ERROR_ALLOCATION_FAILURE;

	memcpy((void*)&(**obj).config, (void*)config, sizeof(linConfig_s));
	(**obj).report.memFootprint = sizeof(lin_o);

	return SUCCESS;
}


