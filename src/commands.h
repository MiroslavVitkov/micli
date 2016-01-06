// Used hardware: none.
// Used interrupts: none.

#ifndef _COMMANDS_H_
#define _COMMANDS_H_


#include <stdbool.h>


#define MAX_CMD_LEN 32                           // In bytes, includeing whitespaces and parameters.
bool listen_for_command(char cmd_buff[], int *bytes);
int execute_command(char cmd_buff[], int bytes);


#endif  // ifndef _COMMANDS_H_
