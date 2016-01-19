#include "strings.h"
#include "usart.h"

#include <avr/pgmspace.h>


#define MAX_STR_LEN (80)


// Holds a copy of the last string requested.
char g_strings_buff[MAX_STR_LEN];

const char string_1[] PROGMEM =
    "!command parameters ENTER" NEWLINE
    "Do not exceed %u characters." NEWLINE;

const char string_2[] PROGMEM =
    "Program start!" NEWLINE;

const char string_3[] PROGMEM =
    "Unknown command: %s." NEWLINE;

const char string_4[] PROGMEM =
    "Running triac with calibration of %li us." NEWLINE;

const char string_5[] PROGMEM =
    "Jumping to bootloader in 3 seconds." NEWLINE;

const char string_6[] =
    "PID setpoint is now %u decicelsius." NEWLINE NEWLINE;

const char string_7[] =
    "Configuring pid with p=%i, i=%i, d=%i in 9s6 format." NEWLINE;

const char string_8[] =
    "Triac output configuread at %u from %u." NEWLINE NEWLINE;

const char string_9[] =
    "Process unstable!!!" NEWLINE;


PGM_P const g_strings_table[STR_COUNT_TOTAL__] PROGMEM =
{
    string_1,
    string_2,
    string_3,
    string_4,
    string_5,
    string_6,
    string_7,
    string_8,
    string_9,
};

const char* strings_get(strings_e n)
{
    strcpy_PF( g_strings_buff, pgm_read_word(&(g_strings_table[n])) );
    return g_strings_buff;
}
