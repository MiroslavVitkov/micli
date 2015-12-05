#define MAX_CMD_LEN 16                           // In bytes, includeing whitespaces and parameters.
void listen_for_command(char cmd_buff[]);
int execute_command(char cmd_buff[]);
