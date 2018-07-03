//Author//
//Miroslav Vitkov 2013
//sir.vorac(at)gmail(dot)com

//License//
//Beerware

#include "OnOffController.h"

#include <stdlib.h>
#include <string.h>

error_t ONOFFcreate(ONOFF_o **obj, const ONOFFconfig_s *const config){
	if(!config)
		return ERROR_NULL_POINTER;

	*obj = (ONOFF_o*)malloc(sizeof(ONOFF_o));
	if(*obj == NULL)
		return ERROR_ALLOCATION_FAILURE;

	memcpy((void*)&(**obj).config, (void*)config, sizeof(ONOFFconfig_s));
	(**obj).report.memFootprint = sizeof(ONOFF_o);
	(**obj).data.previousOutput = 0;

	return SUCCESS;
}


