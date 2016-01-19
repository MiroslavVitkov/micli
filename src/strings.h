#ifndef STRINGS_H_
#define STRINGS_H_


typedef enum
{
    STR_HELP,
    STR_PROGRAM_START,
    STR_UNKNOWN_COMMAND,
    STR_TRIAC_CALIBRATION,
    STR_JUMPING_TO_BOOTLOADER,
    STR_PID_SETPOINT,
    STR_PID_CALIBRATION,
    STR_TRIAC_OUTPUT,
    STR_PROCESS_UNSTABLE,
    STR_COUNT_TOTAL__
} strings_e;

// Returns an SRAM buffer with the required string.
// The buffer is guaranteed to exist unaltered unti
// a new call to strings_get().
const char* strings_get(strings_e);


#endif  // defined(STRINGS_H_)
