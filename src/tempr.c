#include "error.h"
#include "tempr.h"

#include "../libs/onewire/onewire.h"
#include "../libs/ds18x20/ds18x20.h"

#include <util/delay.h>


#define MAX_INIT_RETRIES (20)


// Because the output waveform is computed synchronously with the mains zero-crossing,
// but temperature measurement and pid calculation happen synchronously with the cpu,
// there is a need to synchronise between those processes.
// This variable stores the latest measured value of the temperature.
static decicelsius_t g_temperature;


// Start temperature measurement.
// Before reading, we need to wait at least DS18B20_TCONV_12BIT == 750ms.
void start_meas(void)
{
    error_t err = DS18X20_start_meas(DS18X20_POWER_EXTERN, NULL);
    if(err != DS18X20_OK)  handle_error(ERROR_START_MEASUREMENT);
}


// Read temperature measurement.
void read_tempr(void)
{
    decicelsius_t temperature;
    error_t err = DS18X20_read_decicelsius_single(DS18S20_FAMILY_CODE, &temperature);
    if(err != DS18X20_OK)  handle_error(ERROR_READ_TEMPERATURE);
    g_temperature = temperature;
}


void tempr_init(void)
{
    // Initialize the temperature sensor.
    uint8_t id;
    error_t err;
    for(int i = 0, err = 0;
        i < MAX_INIT_RETRIES && err == OW_PRESENCE_ERR;
        ++i)
    {
        err = ow_rom_search(OW_SEARCH_FIRST, &id);
    };
    if(err == OW_PRESENCE_ERR || err == OW_DATA_ERR)
    {
        handle_error(ERROR_NO_DEVICE_FOUND);
    }

    start_meas();
    _delay_ms( DS18B20_TCONV_12BIT );
}


void tempr_measure(void)
{
    // We assume more than 750ms have elapsed since measurement was started.
    // We read to the global variable, then start the next measurement.
    read_tempr();
    start_meas();
}


decicelsius_t tempr_get(void)
{
    return g_temperature;
}
