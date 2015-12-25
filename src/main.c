#include "config.h"
#include "clock.h"
#include "commands.h"
#include "pid_tune.h"
#include "tempr.h"
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
    static void *pid = NULL;
    if(pid == NULL)  pid = pid_create(to_pid_coeff(1), to_pid_coeff(0), to_pid_coeff(0));
    assert(pid != NULL);
    pid_inout_t ctrl =  pid_run(pid);

    // Cast [0, 640] decicelsius to [2^0, 2^16].
    zcd_proc_val_t cast = (zcd_proc_val_t)(ctrl * (ZCD_PROC_VAL_MAX / 640));
    zcd_adjust_setpoint(cast);
}


void task_report(void)
{
    clock_seconds_t time = clock_get();
    decicelsius_t temperature = tempr_get();
    zcd_proc_val_t triac = zcd_get();
    printf("%lu %i %u" NEWLINE, time, temperature, triac);
}


void task_tempr(void)
{
    tempr_measure();
}


void main(void)
{
    usart_init();
    clock_init();
    tempr_init();

    printf("Program start." NEWLINE);

    for( ; ; clock_sleep_until_next_second() )
    {
        task_parse_cmd();
        task_tempr();
        task_pid_run();
        task_report();
    }
}
