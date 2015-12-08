#include "config.h"
#include "usart.h"
#include "commands.h"
#include "zcd.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>


// Quieck hack for assert() to work.
#define abort() printf("CRITICAL ERROR")


// Function declarations.
void cmd_help(void*);
void cmd_reprogram(void*);
void cmd_zcd_run(void*);
void cmd_zcd_set(void*);


// List of user commands.
typedef void (* fn_ptr_t) (void *context);
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
    DECLARE_COMMAND(zcd_run)
    DECLARE_COMMAND(zcd_set)
};


// Command handlers.
void cmd_help(void *context)
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


void cmd_reprogram(void *context)
{
    printf("Jumping to bootloader in 3 seconds." NEWLINE);
    _delay_ms(3000);

    typedef void (* fn_ptr_t) (void);
    fn_ptr_t my_ptr = (fn_ptr_t)BOOTLOAD;
    my_ptr();
}


// Runs the zero-cross detector and, subsequencly, the timer for the tric control.
void cmd_zcd_run(void *context)
{
    zcd_time_t zcd_calibration = zcd_calibrate();
    zcd_adjust_setpoint(ZCD_PROC_VAL_MAX / 3);   // Example!
    zcd_run(zcd_calibration);
}


void cmd_zcd_set(void *context)
{
    zcd_adjust_setpoint(1000);                   // Example!
}


// Listens to stdin until a valid command is received.
// Commands have the format:
// !command parameters ENTER
// buff[] must be of size MAX_CMD_LEN
void listen_for_command(char cmd_buff[])
{
    memset(cmd_buff, 0, MAX_CMD_LEN);

    // Wait for a string of the type "!.........\n" and record it in a buffer.
    while (getchar() != '!');
    for(int i = 0;; ++i)
    {
        char c = getchar();
        if(c == '\r' || c == '\n')
        {
            return;
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
int execute_command(char cmd_buff[])
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
                Commands[i].handler(cmd_buff);
                return 0;
            }
        }
    }
    return -1;                                   // No command matched.
}


// Use this main() to test the functions in the file.
/*
void main(void)
{
    usart_init();
    printf("Program start." NEWLINE);

    char cmd_buff[MAX_CMD_LEN];
    while(1)
    {
        listen_for_command(cmd_buff);
        int err = execute_command(cmd_buff);
        if(err)
        {
            printf("Unknown command." NEWLINE);
        }
    }
}
*/
