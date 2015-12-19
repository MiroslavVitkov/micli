#include "config.h"
#include "clock.h"
#include "commands.h"
#include "usart.h"
#include "zcd.h"


void task_parse_cmd(void)
{
    int len;
    char cmd_buff[MAX_CMD_LEN];
    if(listen_for_command(cmd_buff, &len))
    {
        int err = execute_command(cmd_buff, len);
        if(err)
        {
            cmd_buff[len] = '\0';
            printf("Unknown command: %s." NEWLINE, cmd_buff);
        }
    }
}


void task_calc_pid(void);


void main(void)
{
    usart_init();
//    clock_init();

    printf("Program start." NEWLINE);

    while(1)
    {
        task_parse_cmd();
    }
}
