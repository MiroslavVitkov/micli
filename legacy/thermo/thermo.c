#include "config.h"	//must be first include

#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <util/atomic.h>
#include <avr/wdt.h>
#include <string.h>
#include <avr/cpufunc.h>

#include "libs/onewire/onewire.h"
#include "libs/ds18x20/ds18x20.h"

#include "usart.h"
#include "Types.h"
#include "OnOffController.h"

//make stdin, stdout and stderr point to the debug usart
static int putChar(char c, FILE *out);
static int getChar(FILE *in);
static FILE fileInOutErr = FDEV_SETUP_STREAM(putChar, getChar, _FDEV_SETUP_RW);

enum{
	//SUCCESS = 0,
	ERROR_NO_DEVICE_FOUND = 1,
	ERROR_START_MEASUREMENT,
	ERROR_READ_TEMPERATURE,
};

//provide hardware capabilities for the temperature controller
void setter(processValue_t in);
processValue_t getter(void);
processValue_t setpoint(void);
void supervisor(processValue_t*);

void handleError(error_t err);

void setHeater(processValue_t in);
processValue_t getTemp(void);



int8_t getRelayState(void){
	return 1<<(HEATER1_PIN) & HEATER1_PORT;
}


//Shared memory
struct{
	processValue_t heaterControl;
	processValue_t temperature;
}SHARED_MEMORY;


void main(void){
	error_t err;

	//setup serial interface
	USARTinit();
	stdin = &fileInOutErr;
	stdout = &fileInOutErr;

	//setup hearthbeat dump
	HEARTHBEAT_DDR |= (1<<HEARTHBEAT_PIN);

	//setup caveman's error reporting
	ERROR_DDR |= (1<<ERROR_PIN);

	//setup output
	HEATER1_DDR |= (1<<HEATER1_PIN);

	//setup watchdog
	wdt_enable(WDTO_2S);

	{
		uint8_t id;
		err = ow_rom_search(OW_SEARCH_FIRST, &id);
		if(err == OW_PRESENCE_ERR || err == OW_DATA_ERR){
			handleError(ERROR_NO_DEVICE_FOUND);
		}
	}

	//create a controller
	ONOFFconfig_s confThermo;
	confThermo.setter = setter;
	confThermo.getter = getter;
	confThermo.setpoint = setpoint;
	confThermo.supervisor = supervisor;
	confThermo.maxSafePlantOutput = 1;
	confThermo.hyst = 10;
	ONOFF_o *pOnOffController;
	err = ONOFFcreate(&pOnOffController, &confThermo);
	if(err){
		handleError(err);
	}
	printf(NEWLINE "Power on!" NEWLINE);
	printf("SensorID[1-255] Setpoint[00.0C] Temperature[00.0C] RelayState[00.0]" NEWLINE);

//scheduler
while(FOREVER){
	//Shared memory
	processValue_t heaterControl = 0, temperature;

	//Task 0: Idle task
	//still alive!: issue hearthbeat, feed dog
	HEARTHBEAT_PINPORT |= (1<<HEARTHBEAT_PIN);	//toggle
	wdt_reset();

	//Task 1: Measure inputs
	//start temperature measurement
	err = DS18X20_start_meas(DS18X20_POWER_EXTERN, NULL);
	if(err != DS18X20_OK){
		handleError(ERROR_START_MEASUREMENT);
	}
	_delay_ms( DS18B20_TCONV_12BIT );
	//read temperature measurement
	err = DS18X20_read_decicelsius_single(DS18S20_FAMILY_CODE, &SHARED_MEMORY.temperature);
	if(err != DS18X20_OK){
		handleError(ERROR_READ_TEMPERATURE);
	}

	//Task 2: Calculate control outputs
	ONOFFrun(pOnOffController);

	//Task 3: Apply control outputs
	setHeater(SHARED_MEMORY.heaterControl);

	//Task4: Clean up and report
	printf("1 %i %i %i" NEWLINE, setpoint(), SHARED_MEMORY.temperature, SHARED_MEMORY.heaterControl);
}	//Over the hills and far away
}	//For ten long years he'll mourn the day

///////////////////////////////////////////////////////////////////////////
// HARDWARE BINDINGS
//uart
static int putChar(char c, FILE *out){
	USARTtransmit(c);
	return 0;	//SUCCESS
}

static int getChar(FILE *in){
	unsigned char c;
	USARTreceive(&c);
	return (int)c;	//SUCCESS
}

//heater
void setHeater(processValue_t in){
	if(in){
		HEATER1_PORT |= (1<<HEATER1_PIN);
	} else {
		HEATER1_PORT &= ~(1<<HEATER1_PIN);
	}
}

////////////////////////////////////////////////////////////////////////
// CONTROLLER INTERFACE 

void setter(processValue_t in){
	SHARED_MEMORY.heaterControl = in;
}

processValue_t getter(void){				//00.0 degrees centigrade
	return SHARED_MEMORY.temperature;
}

processValue_t setpoint(void){
	processValue_t setpoint = 220;
	return setpoint;
}

void supervisor(processValue_t *pVal){
}
//////////////////////////////////////////////////////////////////////
// ERROR HANDLERS
void handleError(error_t err){
	//failsafe
    HEATER1_PORT &= ~(1<<HEATER1_PIN);

	switch(err){
	case ERROR_NO_DEVICE_FOUND:
		printf("Error: no device found!\r");
		while(FOREVER){	//activate watchdog for release configuration
			//error: no device found!
			ERROR_PORT |= (1<<ERROR_PIN);   //turn led on
		}
		break;

	case ERROR_START_MEASUREMENT:
		printf("Error: starting measurement FAILED!\r");
        while(FOREVER){
            ERROR_PINPORT |= (1<<ERROR_PIN);   //flash led slowly
			_delay_ms(1000);
        }
		break;

	case ERROR_READ_TEMPERATURE:
		printf("reading measurement FAILED\r");
		while(FOREVER){
            ERROR_PINPORT |= (1<<ERROR_PIN);   //flash led rapidly
            _delay_ms(100);
        }
		break;
	}

	//just to make sure we do not return from this procedure
	while(FOREVER);					//don't feed watchdog
}
