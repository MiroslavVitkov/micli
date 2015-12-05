#include "config.h"
#include "commands.h"
#include "usart.h"
#include "zcd.h"


void main(void)
{
    usart_init();
    printf("Program start." NEWLINE);

    enable_int1();

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
