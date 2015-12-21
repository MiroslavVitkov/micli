#include "pid_tune.h"
#include "usart.h"
#include "zcd.h"

#include "../libs/onewire/onewire.h"
#include "../libs/ds18x20/ds18x20.h"

#include <assert.h>
#include <stdbool.h>
#include <util/atomic.h>
#include <util/delay.h>


#define ATOMIC ATOMIC_BLOCK(ATOMIC_FORCEON)


// Because the output waveform is computed synchronously with the mains zero-crossing,
// but temperature measurement and pid calculation happen synchronously with the cpu,
// there is a need to synchronise between those processes.
// This variable stores the latest measured value of the temperature.
processValue_t g_temperature;


PIDgains_s pid_tune_Zeigler_Nichols(void)
{
    // Set P, I and D gains to 0.
    // Inkrease P until the output performs sustained oscilaltions.
    // Measure the gain Ku and the period Pu.
    // Consult the table in the literature.
    PIDgains_s result;
    return result;
}


void pid_setter(processValue_t val)
{
    zcd_proc_val_t cast = (zcd_proc_val_t)val;
    zcd_adjust_setpoint(cast);
}
decicelsius_t pid_get_tempr(void)
{
    processValue_t temp;
    ATOMIC{ temp = g_temperature; }
    return temp;
}
timeUs_t get_time_microsec(void)
{
    assert(false);
}
processValue_t pid_setpoint(void)
{
    return PROCESS_VALUE_MAX / 2;
}
void handle_error(error_t err)
{
    printf("Error %i" NEWLINE, err);
}

PID_o* pid_create(void)
{
    // Initialize the triac control.
    zcd_time_t zcd_calibration = zcd_calibrate();
    zcd_adjust_setpoint(0);
    zcd_run(zcd_calibration);

    // Initialize the pid algorithm.
    PID_o *pid;
    PIDconfig_s conf = {
                       .setter = pid_setter,
                       .getter = pid_get_tempr,
                       .getTimeUs = get_time_microsec,
                       .setpoint = pid_setpoint,
                       .supervisor = NULL,
                       .maxSafePlantOutput = PROCESS_VALUE_MAX,
                       .T = 1000000,  // 1s
                       };
    PIDcreate(&pid, &conf);
    return pid;
}

void pid_run(PID_o *pid){
    // Calculate control outputs.
//    PIDrun(pid);
}

