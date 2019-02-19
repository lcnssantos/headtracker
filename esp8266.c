#include "esp8266.h"
#include "main.h"

//#define ESP_DEBUG

#ifdef ESP_DEBUG
#define _DEBUG Debug.Send
#define _FORMAT sprintf
#else
#define _DEBUG
#define _FORMAT
#endif

void AT_Command(char * str)
{
    char buffer[50];
    _FORMAT(buffer, "Comando AT: %s\r", str);
    _DEBUG(buffer);
    ClearAnyBuffer(Wifi_Buffer, SIZE_OF_WIFI_BUFFER);
    Wifi_Buffer_Pos = 0;
    printfMode = AT_MODE;
    printf("%s%c%c", str, 0x0D, 0x0A);
}

void ClearAnyBuffer(char * str, int len)
{
    int i = 0;
    for (i = 0; i < len; i++) {
        str[i] = 0;
    }
}

char * AT_Wait_Response()
{
    unsigned int i = 0, j;
    char lastPos = 0;
    unsigned const char* strings[7] = {"FAIL", ">", "OK", "no change", "Linked", "ERROR", "SEND OK"};
    unsigned long last = TimerCount.miliseconds;
    while (1) {
        for (j = 0; j < 7; j++) {
            if (strstr(Wifi_Buffer, strings[j])) {
                UART_Flush();
                return strings[j];
            }
        }
        if (TimerCount.miliseconds - last > 5000) {
            return "ERROR";
        }
        if (i == SIZE_OF_WIFI_BUFFER) {
            return "ERROR";
        }
        if (Wifi_Buffer_Pos != lastPos) {
            lastPos = Wifi_Buffer_Pos;
            i++;
        }
    }
}

char ESP_Test_Startup()
{
    char ReturnValue;

    AT_Command("AT");
    if (strcmp(AT_Wait_Response(), "OK") == 0) {
        ReturnValue = 1;
    }
    else {
        ReturnValue = 0;
    }
    Debug.Send("ESP8266 OK\r");
    ClearAnyBuffer(Wifi_Buffer, SIZE_OF_WIFI_BUFFER);
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