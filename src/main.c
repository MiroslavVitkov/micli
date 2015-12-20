#include "config.h"
#include "clock.h"
#include "commands.h"
#include "pid_tune.h"
#include "usart.h"
#include "zcd.h"

#include <assert.h>


// Quieck hack for assert() to work.
#define abort() printf("CRITICAL ERROR")


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


void task_pid_run(void)
{
    static PID_o *pid = NULL;
    if(pid == NULL)  pid = pid_create();
    assert(pid != NULL);
//    pid_run(pid);
}


void main(void)
{
    usart_init();
    clock_init();

    printf("Program start." NEWLINE);

    while(1)
    {
        task_parse_cmd();
        task_pid_run();
printf("pid done" NEWLINE);
        clock_sleep_until_next_second();
    }
}
