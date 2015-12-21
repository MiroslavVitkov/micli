#include "commands.h"
#include "config.h"
#include "pid_tune.h"
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
void cmd_pid_run(char*, int);
void cmd_reprogram(char*, int);
void cmd_zcd_run(char*, int);
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
    DECLARE_COMMAND(pid_run)
    DECLARE_COMMAND(reprogram)
    DECLARE_COMMAND(zcd_run)
    DECLARE_COMMAND(zcd_set)
};


// Command handlers.
void cmd_help(char *cmdline, int bytes)
{
    printf("Commands have the format:" NEWLINE);
    printf("!command parameters ENTER" NEWLINE);
    printf("Parameters are separated by spaces; total line length must not exceed %i characters." NEWLINE, MAX_CMD_LEN);

    const command_t *it = Commands;
    const command_t *it_end = Commands + (sizeof(Commands) / sizeof(command_t));
    for(; it < it_end ; ++it)
    {
        printf("%s, ", it->msg);
    }
    printf(NEWLINE NEWLINE);                     // One blank line after output.
}


void cmd_pid_run(char *cmdline, int bytes)
{
    //PID_o *pid = pid_create();
    //pid_run(pid);                   // Call this repetatively to acieve pid control!
    printf(NEWLINE NEWLINE);
}


void cmd_reprogram(char *cmdline, int bytes)
{
    printf("Jumping to bootloader in 3 seconds." NEWLINE);
    _delay_ms(3000);

    typedef void (* fn_ptr_t) (void);
    fn_ptr_t my_ptr = (fn_ptr_t)BOOTLOAD;
    my_ptr();
}


// Runs the zero-cross detector and, subsequencly, the timer for the tric control.
void cmd_zcd_run(char *cmdline, int bytes)
{
    zcd_time_t zcd_calibration = zcd_calibrate();
    zcd_adjust_setpoint(ZCD_PROC_VAL_MAX / 3);   // Example!
    zcd_run(zcd_calibration);
    printf("Running triac with calibration of %li us.", zcd_calibration / (F_CPU / 1000000));
    printf(NEWLINE NEWLINE);
}


void cmd_zcd_set(char *cmdline, int bytes)
{
    cmdline[bytes] = '\0';
    int number = atoi(cmdline+sizeof("zcd_run"));
    zcd_adjust_setpoint(number);
    printf("Triac output configuread at %u from %u.", number, ZCD_PROC_VAL_MAX);
    printf(NEWLINE NEWLINE);
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
