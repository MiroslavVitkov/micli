#ifndef STRINGS_H_
#define STRINGS_H_


typedef enum
{
    STR_EXAMPLE,
    STR_ASECONDEXAMPLE,
} strings_e;

// Returns an SRAM buffer with the required string.
// The buffer is guaranteed to exist unaltered unti
// a new call to strings_get().
const char* strings_get(strings_e);


#endif  // defined(STRINGS_H_)
