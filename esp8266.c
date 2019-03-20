#include "esp8266.h"
#include "main.h"

//#define ESP_DEBUG

/* Pre-compile directives to activate/desactivate ESP Debug */
#ifdef ESP_DEBUG
#define _DEBUG Debug.Send
#define _FORMAT sprintf
#else
#define _DEBUG
#define _FORMAT
#endif

/* */

/*
@purpose: Send an AT command to ESP8266
@parameters: str: The AT command
@return: void
@version: V0.1
*/

void AT_Command(char * str)
{
    /* buffer for debug */
    char buffer[50];
    
    /* debug the actual at command */
    _FORMAT(buffer, "Comando AT: %s\r", str);
    _DEBUG(buffer);
    /* */
    
    /* Clear the wifi buffer and return Wifi_Buffer_pos to 0 */
    ClearAnyBuffer(Wifi_Buffer, SIZE_OF_WIFI_BUFFER);
    Wifi_Buffer_Pos = 0;
    
    /* change printf to AT_MODE */
    printfMode = AT_MODE;
    /* send the AT command */
    printf("%s%c%c", str, 0x0D, 0x0A);
}


/*
@purpose: Clear any buffer
@paramaters: str: a pointer to the buffer; len: length of the buffer
@return: void
@version: V0.1

*/
void ClearAnyBuffer(char * str, int len)
{
    int i = 0;
    for (i = 0; i < len; i++) {
        str[i] = 0;
    }
}

/*
@purpose: Wait AT Answer
@paramaters: void
@return: a pointer to a string representing the result of the AT command
@verson: V0.1
*/
char * AT_Wait_Response()
{
    /* loop control variables */
    unsigned int i = 0, j;
    char lastPos = 0;
    
    /* array to available answers of at commands */
    unsigned const char* strings[7] = {"FAIL", ">", "OK", "no change", "Linked", "ERROR", "SEND OK"};
    
    /* get the actual time in order to get timeout */
    unsigned long last = TimerCount.miliseconds;
    
    /* infinite loop */
    while (1)
    {
        /* check if the Wifi Buffer there an AT answer */
        for (j = 0; j < 7; j++) 
        {
            if (strstr(Wifi_Buffer, strings[j])) 
            {
                /* Flush the UART port */
                UART_Flush();
                /* return the answer */
                return strings[j];
            }
        }
        
        /* if the time inside the infinite loop is greater than 5000 ms, return error (Timeout)*/
        if (TimerCount.miliseconds - last > 5000) 
        {
            return "ERROR";
        }
        
        /* possible Wifi Buffer overflow, return error*/
        if (i == SIZE_OF_WIFI_BUFFER) 
        {
            return "ERROR";
        }
        
        /* Get last position of the Wifi Buffer*/
        if (Wifi_Buffer_Pos != lastPos) 
        {
            lastPos = Wifi_Buffer_Pos;
            i++;
        }
    }
}


/*
@purpose: Test ESP8266
@paramaters: void
@return: 1 for ESP8266 OK and 0 for error 
@version: V0.1
*/


char ESP_Test_Startup()
{
    char ReturnValue;

    /* send the AT command */
    AT_Command("AT");
    
    /* if the answer is "OK", return 1*/
    if (strcmp(AT_Wait_Response(), "OK") == 0) 
    {
        ReturnValue = 1;
    }
    /* else return 0 */
    else 
    {
        ReturnValue = 0;
    }
    
    /* Debug ESP8266 OK */
    Debug.Send("ESP8266 OK\r");
    /* clear wifi buffer */
    ClearAnyBuffer(Wifi_Buffer, SIZE_OF_WIFI_BUFFER);
    /* return the value */
    return ReturnValue;
}

void ESP_Reset()
{
    ESP_CH_DIRECTION = 0;
    ESP_CH = 1;
    ESP8266_RESET_DIRECTION = 0;
    ESP8266_RESET = 0;
    __delay_ms(1000);
    ESP8266_RESET = 1;
}

char ESP_Wifi_Setup(char mode)
{
    char ReturnValue;
    char at[30];
    sprintf(at, "AT+CWMODE_CUR=%d", mode);
    AT_Command(at);
    if (strcmp(AT_Wait_Response(), "OK") == 0) {
        ReturnValue = 1;
    }
    else {
        ReturnValue = 0;
    }
    ClearAnyBuffer(Wifi_Buffer, SIZE_OF_WIFI_BUFFER);
    return ReturnValue;
}

char ESP_Connect_Ap(char* ssid, char * pass)
{
    char at[100];
    sprintf(at, "Trying to connect to %s\r", ssid);
    Debug.Send(at);
    sprintf(at, "AT+CWJAP_CUR=\"%s\",\"%s\"", ssid, pass);
    AT_Command(at);
    char ReturnValue;
    if (strcmp(AT_Wait_Response(), "OK") == 0) {
        ReturnValue = 1;
    }
    else {
        ReturnValue = 0;
    }
    ClearAnyBuffer(Wifi_Buffer, SIZE_OF_WIFI_BUFFER);
    return ReturnValue;
}

char ESP_TCPIP_Status()
{
    AT_Command("AT+CIPSTATUS");
    char ReturnValue;
    if (strcmp(AT_Wait_Response(), "OK") == 0) {
        ReturnValue = 1;
    }
    else {
        ReturnValue = 0;
    }
    ClearAnyBuffer(Wifi_Buffer, SIZE_OF_WIFI_BUFFER);
    return ReturnValue;
}

char ESP_TCPIP_Connect(char * type, char * ip, unsigned int port)
{
    char at[100];
    sprintf(at, "Trying to connect to %s, port: %u via %s \r", ip, port, type);
    Debug.Send(at);
    sprintf(at, "AT+CIPSTART=\"%s\",\"%s\",%u", type, ip, port);
    AT_Command(at);
    char ReturnValue;
    if (strcmp(AT_Wait_Response(), "OK") == 0) {
        ReturnValue = 1;
    }
    else {
        ReturnValue = 0;
    }
    ClearAnyBuffer(Wifi_Buffer, SIZE_OF_WIFI_BUFFER);
    return ReturnValue;
}

char ESP_Transparent_Mode()
{
    AT_Command("AT+CIPMODE=1");
    char ReturnValue;
    if (strcmp(AT_Wait_Response(), "OK") == 0) {
        ReturnValue = 1;
    }
    else {
        ReturnValue = 0;
    }
    ClearAnyBuffer(Wifi_Buffer, SIZE_OF_WIFI_BUFFER);
    return ReturnValue;
}

char ESP_Start_Transparent_Transmission()
{
    AT_Command("AT+CIPSEND");
    char ReturnValue;
    /*if(strcmp(AT_Wait_Response(),"OK") == 0)
    {
       ReturnValue = 1;
    }
    else
    {
       ReturnValue = 0;
    }*/
    ReturnValue = 1;
    __delay_ms(500);
    ClearAnyBuffer(Wifi_Buffer, SIZE_OF_WIFI_BUFFER);
    return ReturnValue;
}

void ESP_Send_Transparent_Data(char * s, char length)
{
    char i = 0;
    for (i = 0; i < length; i++) {
        UART_Write(s[i]);
    }
}

char ESP_AP_Setup(char * ssid, char * pass, char channel, char enc)
{
    char ReturnValue;
    char at[50];
    sprintf(at, "AT+CWSAP_CUR=\"%s\",\"%s\",%d,%d", ssid, pass, channel, enc);
    AT_Command(at);
    if (strcmp(AT_Wait_Response(), "OK") == 0) {
        ReturnValue = 1;
    }
    else {
        ReturnValue = 0;
    }
    ClearAnyBuffer(Wifi_Buffer, SIZE_OF_WIFI_BUFFER);
    return ReturnValue;
}

char ESP_Start_Server(char port)
{
    char ReturnValue;
    char at[30];
    sprintf(at, "AT+CIPSERVER=1,%u", port);
    AT_Command(at);
    if (strcmp(AT_Wait_Response(), "OK") == 0) {
        ReturnValue = 1;
    }
    else {
        ReturnValue = 0;
    }
    ClearAnyBuffer(Wifi_Buffer, SIZE_OF_WIFI_BUFFER);
    return ReturnValue;
}

char ESP_TCPIP_Mux(char mode)
{
    char ReturnValue;
    char at[50];
    sprintf(at, "AT+CIPMUX=%d", mode);
    AT_Command(at);
    if (strcmp(AT_Wait_Response(), "OK") == 0) {
        ReturnValue = 1;
    }
    else {
        ReturnValue = 0;
    }
    ClearAnyBuffer(Wifi_Buffer, SIZE_OF_WIFI_BUFFER);
    return ReturnValue;
}

char ESP_TCPIP_Send(char link, char * string)
{
    char ReturnValue;
    char at[50];
    char * response;
    ClearAnyBuffer(at, 50);
    sprintf(at, "AT+CIPSENDBUF=%c,%d", link, strlen(string));
    AT_Command(at);
    __delay_ms(500);
    //PORTD = 0xAA;
    printfMode = AT_MODE;
    printf(string);
    __delay_ms(500);
    ClearAnyBuffer(Wifi_Buffer, SIZE_OF_WIFI_BUFFER);
    ReturnValue = 1;
    return ReturnValue;
}

char ESP_TCPIP_Close(char linkId)
{
    char ReturnValue;
    char at[50];
    char * response;
    sprintf(at, "AT+CIPCLOSE=%d", linkId);
    AT_Command(at);
    __delay_ms(500);
    ClearAnyBuffer(Wifi_Buffer, SIZE_OF_WIFI_BUFFER);
    return ReturnValue;
}

char ESP_List_Stations()
{
    char ReturnValue;
    char at[50];
    char * response;
    sprintf(at, "AT+CWLIF");
    AT_Command(at);
    if (strcmp(AT_Wait_Response(), "OK") == 0) {
        ReturnValue = 1;
    }
    else {
        ReturnValue = 0;
    }
    ClearAnyBuffer(Wifi_Buffer, SIZE_OF_WIFI_BUFFER);
    return ReturnValue;
}

char ESP_Clear_Wifi_Buffer()
{
    Wifi_Buffer_Pos = 0;
    ClearAnyBuffer(Wifi_Buffer, SIZE_OF_WIFI_BUFFER);
}
