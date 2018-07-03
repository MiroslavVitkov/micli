//Author//
//Miroslav Vitkov 2012
//sir.vorac(at)gmail(dot)com

//License//
//Beerware

#ifndef INC_LIB_CONTROL_TYPES_H
#define INC_LIB_CONTROL_TYPES_H

#include <stdint.h>

typedef int16_t processValue_t;
typedef int64_t timeUs_t;
typedef uint16_t memAddr_t;

inline processValue_t fivePercent(processValue_t in){return in / 20;}
inline processValue_t ninetyFivePercent(processValue_t in){return (in - in / 20);}
#define LIB_CONTROL_MINUS_LN05 3

#define FOREVER 1
#define NEVER 0

typedef enum{
	SUCCESS = 0,
	ERROR_ALLOCATION_FAILURE = 64,	// -127-63 reserved for application error codes
	ERROR_NULL_POINTER,
	ERROR_PROCESS_OUTPUT_HIGHER_THAN_ALLOWED,
	EROR_PROCESS_OUTPUT_EXCEEDS_MAX_EVEN_FOR_ZERO_INPUT,


	ERROR_BAD_FUNCTION_PARAMETERS,
	ERROR_CONVERSION_OVERFLOW,
	ERROR_BUFFER_OVERFLOW,
	ERROR_BAD_EEPROM_ADDRESS,
	ERROR_NOISY_DATA,
	
	ERROR_NOT_INITIALIZED,
	ERROR_NOT_PROPERLY_DEINITIALIZED,

	ERROR_UNKNOWN,
	ERROR_NOT_IMPLEMENTED,
}error_e;
typedef int8_t error_t;

typedef enum{
	false = 0,
	true,
}bool_e;
typedef int8_t bool_t;

#define LIB_CONTROL_FLOAT_POINT_POSITION 16
typedef int32_t float_t;	//aka S32Q16
inline float_t int2float(processValue_t n){return ((float_t)n << LIB_CONTROL_FLOAT_POINT_POSITION);}
inline processValue_t float2int(float_t f){return (processValue_t)(f >> LIB_CONTROL_FLOAT_POINT_POSITION);}

#define LIB_CONTROL_DOUBLE_POINT_POSITION 16
typedef int64_t double_t;	//aka S64Q16
inline double_t int2double(processValue_t n){return ((double_t)n << LIB_CONTROL_DOUBLE_POINT_POSITION);}
inline processValue_t doublet2int(double_t f){return (processValue_t)(f >> LIB_CONTROL_DOUBLE_POINT_POSITION);}

#endif	//INC_LIB_CONTROL_TYPES_H
