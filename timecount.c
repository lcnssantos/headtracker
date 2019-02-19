#include "timecount.h"
#include "main.h"

void TimeCountConstruct(Time * Time)
{
    Time->Update = TimeCount_Update;
    Time->miliseconds = 0;
}

void TimeCount_Update(Time * Time)
{
    if (INTCONbits.TMR0IF) {
        Time->miliseconds = Time->miliseconds + 1;
        if (Timer.PreloadOn) {
            TMR0 = Timer.Preload;
        }
        INTCONbits.T0IF = 0;
    }
}

