#define ESP8266_ECHO_ON 1
#define ESP8266_ECHO_OFF 0
#define ESP8266_STATION_MODE 1
#define ESP8266_AP_MODE 2
#define ESP8266_STATION_AP_MODE 3
#define ESP8266_SINGLE_CONNECTION 0
#define ESP8266_MULTIPLE_CONNECTION 1

#define ESP8266_ENC_OPEN 0
#define ESP8266_ENC_WPA_PSK 2
#define ESP8266_ENC_WPA2_PSK 3
#define ESP8266_ENC_WPA_WPA2_PSK 4

/* AT COMMANDS */
void AT_Command(char * str);
char * AT_Wait_Response();
void ClearAnyBuffer(char * str, int len);
/* */


/* FUNCTIONS */
char ESP_Test_Startup();
void ESP_Reset();
char ESP_Wifi_Setup(char mode);
char ESP_Connect_Ap(char* ssid, char * pass);
char ESP_TCPIP_Status();
char ESP_TCPIP_Connect(char * type, char * ip, unsigned int port);
char ESP_Transparent_Mode();
char ESP_Start_Transparent_Transmission();
void ESP_Send_Transparent_Data(char * s, char length);
char ESP_AP_Setup(char * ssid, char * pass, char channel, char enc);
char ESP_Start_Server(char port);
char ESP_TCPIP_Mux(char mode);
char ESP_TCPIP_Send(char link, char * string);
char ESP_TCPIP_Close(char linkId);
char ESP_List_Stations();
char ESP_Clear_Wifi_Buffer();
/* */