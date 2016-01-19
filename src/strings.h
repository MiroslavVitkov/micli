#ifndef STRINGS_H_
#define STRINGS_H_


typedef enum
{
    STR_HELP                    = 0,
    STR_PROGRAM_START           = 1,
    STR_UNKNOWN_COMMAND         = 2,
    STR_TRIAC_CALIBRATION       = 3,
    STR_JUMPING_TO_BOOTLOADER   = 4,
    STR_PID_SETPOINT            = 5,
    STR_PID_CALIBRATION         = 6,
    STR_TRIAC_OUTPUT            = 7,
    STR_PROCESS_UNSTABLE        = 8,
    STR_COUNT_TOTAL__
} strings_e;

// Returns an SRAM buffer with the required string.
// The buffer is guaranteed to exist unaltered unti
// a new call to strings_get().
const char* strings_get(strings_e);


#endif  // defined(STRINGS_H_)
