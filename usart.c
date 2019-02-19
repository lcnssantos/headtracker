#include "main.h"
#include "usart.h"
#include "soft_uart.h"

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

void UART_Write(unsigned char data)
{
    while (!TXSTA1bits.TRMT1);
    TXREG = data;
}

void putch(char data)
{
    if (printfMode == DEBUG_MODE) {
        Soft_UART_Write(data);
    }
    else if (printfMode == AT_MODE) {
        UART_Write(data);
    }
}

unsigned char UART_TX_Empty()
{
    return TXSTA1bits.TRMT1;
}

void UART_Write_Text(char *text)
{
    int i;
    for (i = 0; text[i] != '\0'; i++)
        UART_Write(text[i]);
}

char UART_Data_Ready()
{
    return (PIR1 & 0x20) == 0x20;
    //return PIR1bits.RCIF;
}

char UART_Read()
{
    UART_Check_Overrun();
    while (!UART_Data_Ready());
    return RCREG;
}

void UART_Read_Text_Bytes(char *Output, unsigned int length)
{
    unsigned int i;
    for (int i = 0; i < length; i++)
        Output[i] = UART_Read();
}

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

void UART_Read_Text_Until(char *Output, char * until, unsigned int maxLength)
{
    unsigned int i;
    for (i = 0; i < maxLength; i++) {
        Output[i] = UART_Read();
        if (strstr(Output, until))
            break;
    }
}

void UART_Flush()
{
    char temp;
    temp = RCREG;
    temp = RCREG;
    temp = RCREG;
    RCREG = 0;
}
