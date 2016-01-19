#include "strings.h"
#include "usart.h"

#include <avr/pgmspace.h>


#define MAX_STR_LEN (80)


// Holds a copy of the last string requested.
char g_strings_buff[MAX_STR_LEN];

const char string_1[] PROGMEM =
    "Commands have the format:" NEWLINE
;//    "!command parameters ENTER" NEWLINE
//    "Parameters are separated by spaces." NEWLINE
//    "Total line length must not exceed    characters." NEWLINE;

const char string_2[] PROGMEM = "String 2";
const char string_3[] PROGMEM = "String 3";
const char string_4[] PROGMEM = "String 4";
const char string_5[] PROGMEM = "String 5";

PGM_P const g_strings_table[] PROGMEM =
{
    string_1,
    string_2,
    string_3,
    string_4,
    string_5
};

const char* strings_get(strings_e n)
{
    strcpy_PF( g_strings_buff, pgm_read_word(&(g_strings_table[n])) );
    return g_strings_buff;
}
