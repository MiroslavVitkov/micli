#include "config.h"
#include "usart.h"
#include "commands.h"


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
