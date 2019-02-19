#include "main.h"
#include "osc.h"

void OscConfig(void)
{
    OSCCONbits.IRCF0 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.OSTS = 0;
    OSCCONbits.SCS1 = 1;
    OSCCONbits.SCS0 = 1;
}