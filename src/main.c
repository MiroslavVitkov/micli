#include "config.h"
#include "clock.h"
#include "commands.h"
#include "pid_tune.h"
#include "strings.h"
#include "tempr.h"
#include "usart.h"
#include "zcd.h"

#include <assert.h>


// Quieck hack for assert() to work.
#define abort() printf("CRITICAL ERROR")


decicelsius_t to_deci(zcd_proc_val_t val)
{
    return (((int32_t)val * (MAX_TEMPR-MIN_TEMPR)) / ZCD_PROC_VAL_MAX);
}
zcd_proc_val_t to_zcd(decicelsius_t val)
{
    if (val < 0) return 0;
    else return (((int32_t)val * ZCD_PROC_VAL_MAX) / (MAX_TEMPR-MIN_TEMPR));
}


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
    static unsigned loop_counter = 0;
    if(loop_counter++ % PID_CONTROL_LOOP_SECONDS) return;

    static enum state_e
    {
        TUNING,
        CREATING,
        RUNNING,
    } state = TUNING;

    pid_inout_t proc_val = (pid_inout_t)tempr_get();
    pid_inout_t ctrl = 0;

    switch(state)
    {
        case TUNING:
            ctrl = pid_tune_Ziegler_Nichols( proc_val, clock_get() );
            if( pid_tune_finished() )  state = CREATING;
            break;

        case CREATING:
            ctrl = 0;
            pid_coeff_t p, i, d;
            pid_get_coeffs(&p, &i, &d);
            printf("Configuring pid with p=%i, i=%i, d=%i in 9s6 format." NEWLINE, p, i, d);
            pid_config(p, i, d);
            state = RUNNING;
            break;

        case RUNNING:
            ctrl = pid_run(proc_val);
            break;
    }

    if(proc_val >= PROC_VAL_CRITICAL)
    {
        printf("Process unstable!!!" NEWLINE);
        ctrl = 0;
    }

    // Cast [0, 640] decicelsius to [2^0, 2^16].
    zcd_proc_val_t cast = to_zcd(ctrl);
    zcd_set(cast);
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
    tempr_init();
    zcd_init();

    printf("Program start." NEWLINE);
const char* buff = strings_get(0);
printf("%s" NEWLINE, buff);
buff = strings_get(1);
printf("%s" NEWLINE, buff);
    for( ; ; clock_sleep_until_next_second() )
    {
        task_parse_cmd();
        task_tempr();
        task_pid_run();
        task_report();
    }
}
