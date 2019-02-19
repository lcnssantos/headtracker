#include <xc.h>
#include "timer.h"

void Timer0_Construct(Timer0 * Timer)
{
    Timer->Init = Timer0_Init;
    Timer->Write = Timer0_Write;
    Timer->Read = Timer0_Read;
    Timer->IntConfig = Timer0_Interrupt_Config;
}

void Timer0_Init(Timer0 * Timer)
{
    T0CON = 0x00;
    T0CONbits.TMR0ON = Timer->Opr;
    T0CONbits.T08BIT = Timer->Mode;
    T0CONbits.T0CS = Timer->ClkSrc;
    T0CONbits.T0SE = Timer->SrcEdg;
    T0CONbits.PSA = Timer->PSA;
    T0CON += Timer->PSAConfig;
}

void Timer0_Write(int Value)
{
    TMR0 = 0xAAAA;
}

int Timer0_Read(void)
{
    return TMR0;
}

void Timer0_Interrupt_Config(char IntPr)
{
    INTCONbits.TMR0IE = 1;
    INTCONbits.TMR0IF = 0;
    INTCON2bits.TMR0IP = IntPr;
}
