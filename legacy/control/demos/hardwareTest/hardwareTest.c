#define F_CPU 8000000

#include <avr/version.h>
#if __AVR_LIBC_VERSION__ < 10606UL
#error "please update to avrlibc 1.6.6 or newer, not tested with older versions"
#endif


//#include "onewire.h"
//#include "ds18x20.h"

#include "PIDcontroller.h"
#include "StepExperiment.h"
#include "Types.h"
#include "usart.h"
#include "ascii.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/* In this simple example, the single job of the controller is to determine the coefficients of a single
 * external object. Possibilities: RLC circuit, heater-in-a-box, voltage divider.
 * target: atmega168, 8MHz internal oscilator
 *
 * Precision measurements:
 * Test setup: Vcc reference, no AVCC, no external cap at AREF
 * ADC: precision: 0.01 Vcc
 * accuracy: 0.1 Vcc
 * TODO: compensate for offset
 * TODO: noise canceling - is it needed - rather offset compensation!
 * PWM : 32kHz
*/

//TODO: needs refactoring
const processValue_t G_maxProcessValue = -1;	//as type is unsigned, this corresponds to the MAX value
const processValue_t G_setpoint = 0x7fff;	//Vcc/2 - set this according to application
const processValue_t G_critical = 0xaaaa;	//Vcc*(2/3) - set this according to application

volatile int8_t g_alarm = 0;
volatile timeUs_t time = 0;

void init(void);
void set(processValue_t in);
processValue_t get(void);
processValue_t setp(void);
void super(processValue_t *inout);
timeUs_t getTime(void);
void handleError(error_t e);
processValue_t ReadADC(uint8_t ch);

void commandPidCalibrate(void);
void commandStepRun(void);
void commandAdcGet(void);
void commandTempGet(void);

ISR(TIMER1_OVF_vect){
	time += 0xffff;
}

//general keyboard commands - work within concrete behaviours
#define COMMAND_OK ASCII_CONTROL_CR
#define COMMAND_INCREASE ASCII_PRINTABLE_PLUS
#define COMMAND_DECREASE ASCII_PRINTABLE_MINUS
#define INCREMENT 1000

//behaviours - keyboard kontrol
#define COMMAND_ADC_GET ASCII_PRINTABLE_A
#define COMMAND_TEMPERATURE_GET ASCII_PRINTABLE_T


#define PID_CALIBRATE ASCII_PRINTABLE_Z
#define STEP_RUN ASCII_PRINTABLE_X
#define STEP_HIGHER ASCII_PRINTABLE_C
#define STEP_LOWER ASCII_PRINTABLE_V
#define ERROR_OCCURED ASCII_CONTROL_DEL
#define ACK ASCII_CONTROL_ACK
#define ERROR_UNKNOWN_COMMAND 255

inline void DEBUG_TAG(uint8_t X) {USARTtransmit(X);}
//#define DEBUG_TAG(X) {USARTtransmit(X);}
#define DEBUG_PRINT(X) 	USARTtransmitBlock((unsigned char *)&X, sizeof(X))

//send ENQ
//wait for a command
//execute command
//return the return code
void main(void){
	unsigned char command;

	init();

	//ready - wait for a command
	while(1){
	DEBUG_PRINT("Command?\n");
	handleError(USARTreceive(&command));		//blocking
	DEBUG_PRINT("Acknowadged.");

	switch(command){
	// RELEASE //
	//create a PID controller
	//run calibration
	//stream the report
	case PID_CALIBRATE:
		commandPidCalibrate();
		break;

	// DEBUG //
	// create a step experiment object
	// run one or several experiments
	// destroy object
	case STEP_RUN:
		commandStepRun();
		break;

	// perform a conversion on the ADC
	// maybe correct bias
	// maybe filter it
	// report it
	case COMMAND_ADC_GET:
		commandAdcGet();
		break;

	// read temp sensor and report output
	case COMMAND_TEMPERATURE_GET:
		commandTempGet();
		break;

	default:
		handleError(ERROR_UNKNOWN_COMMAND);
	}	//switch(command)
	};	//while(1);

}

//////////////////////////////// Implementation of local functions //////////////

//////////////////////////////// minHAL ////////////////////////////////////////
void handleError(error_t e){
	if(e){
		DEBUG_TAG((uint8_t)(ERROR_OCCURED));
		DEBUG_TAG((uint8_t)(e));
	}
}

void init(void){
	//ports
//	DDRB = 0xff;	//out

	//power led
	DDRD |= (1 << PD6);
	PORTD |= (1 << PD6);
	
	//pwm
	DDRD = 0xff;	//out
	TCCR0A = (1<<WGM01) | (1<<WGM00) | (1<<COM0A1);	//fast pwm, no clock prescaling, output on OC0A
	TCCR0B = (1<<CS00);
	TCCR0B |= (1 << COM0A1);			//clear OC0A on compare match, set at BOTTOM
	OCR0A = 0;					//safe output: 0 volts

	//adc
	ADMUX = (1<<REFS0);		//range [0 - Vcc], single conversion mode
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);	//prescaler 128

	//time
	TCCR1B = (1<<CS11);		//tick every us
	TIMSK1 = (1<<TOIE1);	//interrupt on oveflow

	USARTinit();

	sei();
}

void set(processValue_t in){
	OCR0A = in / 255;
}

processValue_t get(void){
	processValue_t t = ReadADC(0);
	return t<<6;
}

processValue_t setp(){
	return G_setpoint;
}

void super(processValue_t *inout){
	if (*inout > G_critical){
		*inout = 0;
		g_alarm = 1;
	}
}

timeUs_t getTime(){
	return time + TCNT1;
}

processValue_t ReadADC(uint8_t ch)
{
   //Select ADC Channel ch must be 0-7
   ch=ch&0b00000111;
   ADMUX|=ch;

   //Start Single conversion
   ADCSRA|=(1<<ADSC);

   //Wait for conversion to complete
   while(!(ADCSRA & (1<<ADIF)));

   //Clear ADIF by writing one to it
   //Note you may be wondering why we have write one to clear it
   //This is standard way of clearing bits in io as said in datasheets.
   //The code writes '1' but it result in setting bit to '0' !!!

   ADCSRA|=(1<<ADIF);

   return(ADC);
}

//Given some value of type processValue_t:
//+ increases the value
//- decreases the value
//
void handleValue(processValue_t *t){
	unsigned char command;

	while(1){
		DEBUG_PRINT(t);
		handleError(USARTreceive(&command));		//blocking

		switch(command){
		case COMMAND_OK:
			return;
 
		case COMMAND_INCREASE:
			t += INCREMENT;
			break;

		case COMMAND_DECREASE:
			t -= INCREMENT;
			break;
		default:
			handleError(ERROR_UNKNOWN_COMMAND);
		}
	}
}

///////////////////////////// Command handlers ////////////////////////

void commandPidCalibrate(void){
	PID_o *pid;
	PIDconfig_s pidConfig;
	error_t ret;

	pidConfig.setter = &set;
	pidConfig.getter = &get;
	pidConfig.getTimeUs = &getTime;
	pidConfig.setpoint = &setp;
	pidConfig.supervisor = &super;
	pidConfig.maxSafePlantOutput = G_maxProcessValue;
	ret = PIDcreate(pid, &pidConfig);
	handleError(ret);

	//run calibration
	ret = PIDcalibrateInitialGuess(pid);
	handleError(ret);

	//report
	DEBUG_TAG(ASCII_CONTROL_STX);
	DEBUG_PRINT(pid->report);
	DEBUG_TAG(ASCII_CONTROL_ETX);

	PIDdestroy(pid);
}

void commandStepRun(void){
	step_o *step = NULL;
	stepConfig_s stepConfig;
	error_t ret;
	processValue_t size = 30000;

	//Entered function. Report that, wait for conformation.
	//+ increases step, - decreases it, enter proceeds.

	stepConfig.setter = &set;
	stepConfig.getter = &get;
	stepConfig.getTimeUs = &getTime;
	stepConfig.maxSafePlantOutput = G_maxProcessValue;
	handleError(stepCreate(step, &stepConfig));

DEBUG_TAG(201);
	//run experiment
	handleError(stepBasicRun(step, size));
DEBUG_TAG(202);
	handleError(stepSecondaryRun(step));
DEBUG_TAG(203);
	handleError(stepSecondaryRun(step));
DEBUG_TAG(204);
	//report
	DEBUG_TAG(ASCII_CONTROL_STX);
DEBUG_TAG(205);
	DEBUG_PRINT(step->report);
DEBUG_TAG(206);
	DEBUG_TAG(ASCII_CONTROL_ETX);
	stepDestroy(step);
DEBUG_TAG(207);
}

void commandAdcGet(void){
	processValue_t v;
	v = get();
	DEBUG_TAG(ASCII_CONTROL_STX);
	DEBUG_PRINT(v);
	DEBUG_TAG(ASCII_CONTROL_ETX);
}

void commandTempGet(void){
}


