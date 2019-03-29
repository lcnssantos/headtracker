#include "main.h"
#include "usart.h"
#include "soft_uart.h"

/*
@purpose: Start the Hardware UART of processor
@parameters: baudrate -> the number of bits per second of transmission
@return: void
@version: V0.1

*/
void UART_Init(const unsigned long int baudrate)
{
    unsigned int x;
    x = (_XTAL_FREQ / (baudrate * 4)) - 1; //SPBRG for Low Baud Rate
    SPBRG = x & 0xFF;
    SPBRGH = x >> 8;

    TXSTAbits.BRGH = 1;
    TXSTAbits.SYNC = 0;
    TXSTAbits.TX9 = 0;
    TXSTAbits.TXEN = 1;

    RCSTAbits.SPEN = 1;
    RCSTAbits.RX9 = 0;
    RCSTAbits.CREN = 1;

    BAUDCONbits.BRG16 = 1;

    TRISCbits.RC6 = 1;
    TRISCbits.RC7 = 1;
}

/*
@purpose: Check if there is a overrun and reset the HW UART
@parameters: void
@return: 1 if OVERRUN or 0 if isn't.
@version: V0.1

*/
char UART_Check_Overrun()
{
    if (RCSTAbits.OERR) {
        RCSTAbits.CREN = 0;
        __delay_ms(1);
        RCSTAbits.CREN = 1;
        return 1;
    }
    else return 0;
}

/* 
@purpose: write a byte to the hardware uart
@parameters: data -> the data to be sent
@return: void
@version: V0.1
*/
void UART_Write(unsigned char data)
{
    while (!TXSTA1bits.TRMT1);
    TXREG = data;
}

/*
@purpose: implementation of putch in order to use printf
@parameters: data-> data to be sent
@return: void
@version: V0.1

*/
void putch(char data)
{
    if (printfMode == DEBUG_MODE) {
        Soft_UART_Write(data);
    }
    else if (printfMode == AT_MODE) {
        UART_Write(data);
    }
}

/*
@purpose: check if TX buffer is empty
@parameters: void
@return: 1 if is empty or 0 if isn't empty
@version: V0.1
*/
unsigned char UART_TX_Empty()
{
    return TXSTA1bits.TRMT1;
}

/*
@purpose: Write a string to the Hardware UART
@parameters: text -> pointer to the string
@return: void
@version: V0.1
*/
void UART_Write_Text(char *text)
{
    int i;
    for (i = 0; text[i] != '\0'; i++)
        UART_Write(text[i]);
}

/*
@purpose: Check if there is data available on RX buffer
@parameters: void
@return: 1 if data is available or 0 if data isn't avaialble
@version: V0.1
*/
char UART_Data_Ready()
{
    return (PIR1 & 0x20) == 0x20;
    //return PIR1bits.RCIF;
}

/*
@purpose: Standby for Data on RX buffer of serial and return it
@parameters: void
@return: the data receveid
@version: V0.1
*/
char UART_Read()
{
    UART_Check_Overrun();
    while (!UART_Data_Ready());
    return RCREG;
}

/*
@purpose: Read a number of bytes
@parameters: Output -> a pointer where the data will be stored; length -> the number of bytes to received
@return: void
@version: V0.1
*/
void UART_Read_Text_Bytes(char *Output, unsigned int length)
{
    unsigned int i;
    for (int i = 0; i < length; i++)
        Output[i] = UART_Read();
}

/*
Disregard this function, isn't used and can cause processor bug
*/
void UART_Read_Text(char * output)
{
    unsigned long int i;
    while (!UART_Data_Ready());
    while (UART_Data_Ready()) {
        output[i] = UART_Read();
        i++;
        __delay_us(110);
    }
}

/*
Disregard this function, isn't used and can cause processor bug
*/
void UART_Read_Text_Until(char *Output, char * until, unsigned int maxLength)
{
    unsigned int i;
    for (i = 0; i < maxLength; i++) {
        Output[i] = UART_Read();
        if (strstr(Output, until))
            break;
    }
}

/*
@purpose: Flus the uart
@parameters: void
@return: void
@version: V0.1
*/
void UART_Flush()
{
    char temp;
    temp = RCREG;
    temp = RCREG;
    temp = RCREG;
    RCREG = 0;
}
