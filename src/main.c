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


// TODO: move to pid_tune.
decicelsius_t to_deci(zcd_proc_val_t val) {return (((int32_t)val * 640) / ZCD_PROC_VAL_MAX);}
zcd_proc_val_t to_zcd(decicelsius_t val) {if (val < 0) return 0; else return (((int32_t)val * ZCD_PROC_VAL_MAX) / 640);}


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
    pid_inout_t proc_val = (pid_inout_t)tempr_get();
    pid_inout_t ctrl = pid_run(proc_val);

    // Cast [0, 640] decicelsius to [2^0, 2^16].
    zcd_proc_val_t cast = to_zcd(ctrl);
    zcd_set(cast);
}


void task_pid_tune(void)
{
    // Replace the pid controleler with a relay.
    // The relay has no hysteresis and acts as a sign function.
    // The amplitude of the relay is selected to be 100 decicelsius.
    // The setpoint is selected to be 300 decicelsius.
    // Consistent and symmetrical oscillations should be observed.
    pid_inout_t setpoint = 300;
    pid_inout_t ampl = 400;                      // i.e. 0C - 40C control signal
    decicelsius_t tempr = tempr_get();
    if(tempr > setpoint)
    {
        zcd_set(0);
    }
    else
    {
        zcd_set( to_zcd(ampl) );
    }
    // how do we measure resulting oscillation amplitude and period?
}

void task_report(void)
{
    clock_seconds_t time = clock_get();
    decicelsius_t temperature = tempr_get();
    zcd_proc_val_t triac = to_deci( zcd_get() );
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
    pid_config(0, 0, 0);
    tempr_init();
    zcd_init();

    printf("Program start." NEWLINE);

    for( ; ; clock_sleep_until_next_second() )
    {
        task_parse_cmd();
        task_tempr();
        task_pid_run();
        task_report();
    }
}
