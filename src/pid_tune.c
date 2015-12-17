#include "pid_tune.h"


PIDgains_s pid_tune_Zeigler_Nichols(void)
{
    // Set P, I and D gains to 0.
    // Inkrease P until the output performs sustained oscilaltions.
    // Measure the gain Ku and the period Pu.
    // Consult the table in the literature.
    PIDgains_s result;
    return result;
}
