#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>

// The main loop runs in 1 second.
// As the pid control loop period determines a lot of the math,
// it can be run at any integer number of seconds, from 1 to UINT16_MAX.
#define PID_CONTROL_LOOP_SECONDS 60

// CPU nominal frequency, Hz.
#define F_CPU 8000000

// USART baud rate.
#define BAUD 38400

// This macro is passed from the makefile.
// It contains the byte address of the start of the bootloader section.
// This depends upon device and the BOOTSZ[1, 0] bits.
// YOU MUST SET THIS MANUALLY IN THE MAKEFILE.
// #define BOOTLOAD <byte_address>

// Maximum temperature in 'decicelsius', that should not be exceeded.
// Note that due to overshoot this can be exceeded by as much as 100 decicelsius.
#define PROC_VAL_CRITICAL (550)

// Control output is scaled to (MAX_TEMP-MIN_TEMP) and reported.
// This way the pid algorithm operates in a consistend unit of measure:
// 'decicelsius_t'.
#define MIN_TEMPR (0)
#define MAX_TEMPR (640)


// Heater hardware
#define PIN_HEATER  PD6
#define PORT_HEATER PORTD
#define DDR_HEATER  DDRD


// Thermometer configuration.
#define DS18X20_EEPROMSUPPORT     0
#define DS18X20_DECICELSIUS       1
#define DS18X20_MAX_RESOLUTION    0
#define DS18X20_VERBOSE           0

#define PORT_OW PORTB
#define PIN_OW  PB6
#define DDR_OW  DDRB
#define PINPORT_OW PINB

#define OW_ONE_BUS
#define OW_PIN PIN_OW
#define OW_IN PINPORT_OW
#define OW_OUT PORT_OW
#define OW_DDR DDR_OW


#endif	//#ifndef _CONFIG_H_
