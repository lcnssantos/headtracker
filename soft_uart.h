typedef struct
{
    char  * TX_PIN;
    char  * TX_PIN_D;
    void (*Send)(char *);
}Soft_Serial;

void Soft_UART_Construct(Soft_Serial * Port);
void Soft_UART_Init();
void Soft_UART_Write(char data);
void Soft_Debug_Send(char * string);

