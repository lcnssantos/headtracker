#include "soft_uart.h"
#include "main.h"

/*
@purpose: Software UART constructor
@parameters: Port -> Pointer to the Soft_Serial class
@return: void
@version: v0.1
*/
void Soft_UART_Construct(Soft_Serial * Port)
{
    SOFT_TXD_DIRECTION = 0;
    SOFT_TXD = 1;
    Port->Send = Soft_Debug_Send;
}

/*
@purpose: Start the Soft UART 
@parameters: void
@return: void
@version: v0.1
*/

void Soft_UART_Init()
{
    SOFT_TXD_DIRECTION = 0;
    SOFT_TXD = 1;
}

/* 
@purpose: Send a data via Soft Serial
@parameters: data-> the byte to be sent
@return: void
@version: v0.1
*/
void Soft_UART_Write(char data)
{
    char i;
    INTCONbits.GIEH = 0;
    INTCONbits.PEIE = 0;
    SOFT_TXD = 0;
    __delay_us(104);
    for (i = 0; i < 8; i++) {
        SOFT_TXD = data & 0x01;
        data = data >> 1;
        __delay_us(104);
    }
    SOFT_TXD = 1;
    __delay_us(104);
    INTCONbits.GIEH = 1;
    INTCONbits.PEIE = 1;
}

/*
@purpose: Send a string via Soft serial
@parameters: string -> a pointer to the string
@return: void
@version: v0.1
*/
void Soft_Debug_Send(char * string)
{
    printfMode = DEBUG_MODE;
    printf(string);
}
