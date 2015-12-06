// Used hardware: none.
// Used interrupts: none.

#ifndef _COMMANDS_H_
#define _COMMANDS_H_


#define MAX_CMD_LEN 16                           // In bytes, includeing whitespaces and parameters.
void listen_for_command(char cmd_buff[]);
int execute_command(char cmd_buff[]);


#endif  // ifndef _COMMANDS_H_
