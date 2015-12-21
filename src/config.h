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

#define HEATER_PIN  PD6
#define HEATER_PORT PORTD

#define DS18X20_EEPROMSUPPORT     0
#define DS18X20_DECICELSIUS       1
#define DS18X20_MAX_RESOLUTION    0
#define DS18X20_VERBOSE           0

#endif	//#ifndef _CONFIG_H_
