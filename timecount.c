#include "timecount.h"
#include "main.h"

/*
@purpose: Time Count Construction
@parameters: Time -> a pointer to the time count class
@return: void
@version: V0.1
*/

void TimeCountConstruct(Time * Time)
{
    Time->Update = TimeCount_Update;
    Time->miliseconds = 0;
}

/*
@purpose: Update the Time Count 
@paramaeters: Time -> a pointer to the time count class
@return: void
@version: v0.1
*/
void TimeCount_Update(Time * Time)
{
    if (INTCONbits.TMR0IF) 
    {
        Time->miliseconds = Time->miliseconds + 1;
        if (Timer.PreloadOn) 
        {
            TMR0 = Timer.Preload;
        }
        INTCONbits.T0IF = 0;
    }
}

