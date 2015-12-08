#include "config.h"
#include "commands.h"
#include "usart.h"
#include "zcd.h"


void main(void)
{
    usart_init();
    printf("Program start." NEWLINE);

    char cmd_buff[MAX_CMD_LEN];
    while(1)
    {
        int len;
        listen_for_command(cmd_buff, &len);
        int err = execute_command(cmd_buff, len);
        if(err)
        {
            cmd_buff[len] = '\0';
            printf("Unknown command: %s." NEWLINE, cmd_buff);
        }
    }
}
