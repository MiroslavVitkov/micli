#include "error.h"
#include "tempr.h"

#include "../libs/onewire/onewire.h"
#include "../libs/ds18x20/ds18x20.h"

#include <util/delay.h>


decicelsius_t g_temperature;


void tempr_init(void)
{
    // Initialize the temperature sensor.
    uint8_t id;
    int err = ow_rom_search(OW_SEARCH_FIRST, &id);
    if(err == OW_PRESENCE_ERR || err == OW_DATA_ERR)
    {
        handle_error(ERROR_NO_DEVICE_FOUND);
    }
}


void tempr_measure(void)
{
    decicelsius_t temperature;

    // Start temperature measurement.
    error_t err = DS18X20_start_meas(DS18X20_POWER_EXTERN, NULL);
    if(err != DS18X20_OK)
    {
        handle_error(ERROR_START_MEASUREMENT);
    }
    _delay_ms( DS18B20_TCONV_12BIT );  // 750ms
    // Read temperature measurement.
    err = DS18X20_read_decicelsius_single(DS18S20_FAMILY_CODE, &temperature);
    if(err != DS18X20_OK)
    {
        handle_error(ERROR_READ_TEMPERATURE);
    }
    g_temperature = temperature;
}


decicelsius_t tempr_get(void)
{
    return g_temperature;
}
