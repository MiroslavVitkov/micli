#include "commands.h"
#include "config.h"
#include "pid_tune.h"
#include "strings.h"
#include "usart.h"
#include "zcd.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <util/delay.h>


// Quieck hack for assert() to work.
#define abort() printf("CRITICAL ERROR")


// Function declarations.
void cmd_help(char*, int);
void cmd_reprogram(char*, int);
void cmd_pid_coeffs(char*, int);
void cmd_pid_setpoint(char*, int);
void cmd_zcd_set(char*, int);


// List of user commands.
typedef void (* fn_ptr_t) (char *cmdline, int bytes);
typedef struct command
{
    const fn_ptr_t handler;
    const char *msg;
    const unsigned len;                          // Without the terminating '\0'.
} command_t;
#define DECLARE_COMMAND(name) {cmd_##name, #name, sizeof(#name)-1},
const command_t Commands[] =
{
    DECLARE_COMMAND(help)
    DECLARE_COMMAND(reprogram)
    DECLARE_COMMAND(pid_coeffs)
    DECLARE_COMMAND(pid_setpoint)
    DECLARE_COMMAND(zcd_set)
};


// Command handlers.
void cmd_help(char *cmdline, int bytes)
{
    printf( strings_get(STR_HELP), MAX_CMD_LEN );

    const command_t *it = Commands;
    const command_t *it_end = Commands + (sizeof(Commands) / sizeof(command_t));
    for(; it < it_end ; ++it)
    {
        printf("%s, ", it->msg);
    }
    printf(NEWLINE NEWLINE);                     // One blank line after output.
}


void cmd_reprogram(char *cmdline, int bytes)
{
    printf( strings_get(STR_JUMPING_TO_BOOTLOADER) );
    _delay_ms(3000);

    typedef void (* fn_ptr_t) (void);
    fn_ptr_t my_ptr = (fn_ptr_t)BOOTLOAD;
    my_ptr();
}


void cmd_pid_coeffs(char *cmdline, int bytes)
{
    // Parse 'pid_coeffs <p> <i> <d>'.
    cmdline[bytes] = '\0';
    char *it = cmdline+sizeof("pid_coeffs");
    int num[3];
    for(int i = 0; i < 3; ++i)
    {
        num[i] = atoi(it);
        it += (num[i] / 10) + 2;                 // Skip all digits and a whitespace.
        if(num[i] < 0)  ++it;
    }

    // Unsofrunately, we support only integer gains currently.
    pid_config( to_pid_coeff(num[0]), to_pid_coeff(num[1]), to_pid_coeff(num[2]) );
    printf(NEWLINE NEWLINE);
}


void cmd_pid_setpoint(char *cmdline, int bytes)
{
    cmdline[bytes] = '\0';
    int number = atoi(cmdline+sizeof("pid_setpoint"));
    pid_setpoint(number);
    printf( strings_get(STR_PID_SETPOINT) , number);
}


void cmd_zcd_set(char *cmdline, int bytes)
{
    cmdline[bytes] = '\0';
    int number = atoi(cmdline+sizeof("zcd_run"));
    zcd_set(number);
    printf( strings_get(STR_TRIAC_OUTPUT), number, ZCD_PROC_VAL_MAX);
}


// Listens to stdin until a valid command is received.
// Commands have the format:
// !command parameters ENTER
// buff[] must be of size MAX_CMD_LEN
bool listen_for_command(char cmd_buff[], int *bytes)
{
    memset(cmd_buff, 0, MAX_CMD_LEN);
    char c;

    // Wait for a string of the type "!.........\n" and record it in a buffer.
    do
    {
        if( !(UCSR0A & (1<<RXC0)) )
        {
            return false;                            // End of input buffer.
        }
        c = getchar();
    } while(c != '!');

    for(int i = 0;; ++i)
    {
        c = getchar();
        if(c == '\r' || c == '\n')
        {
            *bytes = i + 1;                      // Without leading '!'.
            return true;
        }
        else
        {
            assert(i < MAX_CMD_LEN);
            cmd_buff[i] = c;
        }
    }
}


// buff[] must be of size MAX_CMD_LEN
// Returns 0 if command handler was called and non-zero in case of error.
int execute_command(char cmd_buff[], int bytes)
{
    // Try to match each known command against the buffer.
    // Stop on first match.
    // Call command handler and return.
    // A more efficient algorithms would be: https://en.wikipedia.org/wiki/Radix_tree
    for(int i = 0; i < sizeof(Commands) / sizeof(command_t); ++i)
    {
        for(int j = 0; j < Commands[i].len; ++j)
        {
            char c = cmd_buff[j];
            if(c != Commands[i].msg[j]) break;   // Continue to test next command.
            if(j == Commands[i].len - 1)         // j is counted from 0, while len is counter from 1.
            {
                Commands[i].handler(cmd_buff, bytes);
                return 0;
            }
        }
    }
    return -1;                                   // No command matched.
}
