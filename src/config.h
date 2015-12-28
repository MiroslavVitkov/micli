#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>

// If undefined, error checking on the XMODEM protocol is disabled.
// Also, the flash writing routine is not protected form overwriting the bootloader section.
// Furthermore, flash memory verification after write is not performed.
#define ERROR_CHECKING

// CPU nominal frequency, Hz.
#define F_CPU 8000000

// USART baud rate.
#define BAUD 38400

// This macro is passed from the makefile.
// It contains the byte address of the start of the bootloader section.
// This depends upon device and the BOOTSZ[1, 0] bits.
// YOU MUST SET THIS MANUALLY IN THE MAKEFILE.
// #define BOOTLOAD <byte_address>

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
