#include "PIDcontroller.h"
#include "Types.h"
#include <avr/io.h>
#include <avr/interrupt.h>

//In this simple example, the single job of the controller is to stabilize an unknown process.
//target: atmega644, 8MHz nominal
//output: PB3
//input: PA0

const processValue_t G_maxProcessValue = -1;	//as type is unsigned, this corresponds to the MAX value
const processValue_t G_setpoint = 0x7fff;	//Vcc/2 - set this according to application
const processValue_t G_critical = 0xaaaa;	//Vcc*(2/3) - set this according to application
int8_t g_alarm = 0;
timeUs_t time = 0;

void init(void);
void set(processValue_t in);
processValue_t get(void);
processValue_t setp(void);
void super(processValue_t *inout);
timeUs_t getTime(void);	//dummmy - time is not needed for controller run

int main(int argc, char **argv){
	error_t ret;
	PID_o *pid;
	PIDconfig_s pidConfig;
	
	init();

	pidConfig.setter = &set;
	pidConfig.getter = &get;
	pidConfig.getTimeUs = &getTime;
	pidConfig.setpoint = &setp;
	pidConfig.supervisor = &super;
	pidConfig.maxSafePlantOutput = G_maxProcessValue;	
	ret = PIDcreate(pid, &pidConfig);

	//now if gains are avaible, load them:
	PIDgains_s g;
	g.p = 1;
	g.i = 0;
	g.d = 0;
	PIDloadGains(pid, g);

	//or, if P, I and D gains are unknown ... run calibration. This is what consumes the codespace.
//	ret = PIDcalibrateInitialGuess(pid);

	//Normally, one would set up a timer - but I'm lazy.
	PIDinit(pid);
	while(1){
		PIDrun(pid);
	}

//	PIDdestroy(pid);	//why bother?
	return 0;
}

void init(void){
	//ports
	DDRB = 0xff;	//out
//	DDRA = 0x0;	//in

	//pwm
	TCCR0A = (1<<COM0A1) | (1<<WGM01) | (1<<WGM00);	//fast pwm, no clock prescaling, output on PB3
	TCCR0B = (1<<WGM02) | (1<<CS00);
	OCR0A = 0;	//safe output: 0 volts

	//adc
	ADMUX = (1<<REFS0);	//range [0 - Vcc], free running mode
	ADCSRA = (1<<ADEN) | (1<<ADSC);	//start 

	//time
	TCCR1B = (1<<CS11);	//tick every us
	TIMSK1 = (1<<TOIE1);	//interrupt on oveflow
}	

ISR(TIMER1_OVF_vect){
	time += 0xffff;
}

void set(processValue_t in){
	uint8_t out = (in / G_maxProcessValue) * 255;
	OCR0A = out;
}

processValue_t get(void){
	return ADC<<6;
}

processValue_t setp(){
	return G_setpoint;
}

void super(processValue_t *inout){
	if (*inout > G_critical){
		*inout = 3;
		g_alarm = 1;
	}
}

timeUs_t getTime(){
	return 0;
}

