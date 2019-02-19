#include "fuses_bits.h"
#include <stdio.h>
#include <string.h>
#include "timer.h"
#include "timecount.h"
#include "task.h"
#include "usart.h"
#include "soft_uart.h"
#include "osc.h"
#include "interrupt.h"
#include "timecount.h"
#include "motiondriver.h"
#include "mpu6050.h"
#include "i2c.h"
#include "app.h"
#include "esp8266.h"
#include "eeprom.h"
#include "adc.h"
#include "soft_i2c.h"


#define _XTAL_FREQ 16000000

/* INSTANCIA CLASSE TIMER 0 USADA PARA BASE DE TEMPO DE 1ms*/
Timer0 Timer;
Time TimerCount;

Task BattteryStatus;
Task LedBlink_2;
Task DMP;
Task ConfigMode;


Soft_Serial Debug;
MPU MPU_Sensor;


/* SOFT SERIAL DEFINES START*/
#define SOFT_TXD_DIRECTION TRISBbits.RB7
#define SOFT_TXD PORTBbits.RB7
/* SOFT SERIAL DEFINES END */

char printfMode;

char configModeStatus = _FALSE_;
char CalibrateMode = _FALSE_;


#define _VERSION_ "V4.03D"

char printfMode;

#define AT_MODE 0
#define DEBUG_MODE 1



char mpuAddr;

/* MPU6050 */
#define MPU_CH_DIRECTION TRISBbits.RB5
#define MPU_CH LATBbits.LATB5
/* */

/* WIFI */
#define ESP_CH_DIRECTION TRISCbits.RC5
#define ESP_CH LATCbits.LATC5

#define SIZE_OF_WIFI_BUFFER 256
char Wifi_Buffer[SIZE_OF_WIFI_BUFFER];
char Wifi_Buffer_Pos;

#define ESP8266_RESET_DIRECTION TRISCbits.RC2
#define ESP8266_RESET   LATCbits.LC2
/* WIFI */

#define IP "192.168.2.102"
#define PORT 5550

#define page "<html><title>Configuração Wifi</title><table> <tr> <td width=\"300px\"> <form name=\"WI_F\" method=\"GET\"  enctype=\"text/plain\" action=\"wifi.html\"> SSID: <input type=\"text\" name=\"SSID\"> <br><br>Pass: <input type=\"password\" name=\"PASS\"> <br><br></form> <button onclick=\"sendWI()\" type=\"submit\">Configurar</button> </td><td width=\"300px\"> <form name=\"IP_F\" method=\"GET\" action=\"ip.html\">IP: <input type=\"text\" name=\"IP\"> <br><br>Port: <input type=\"text\" name=\"PORT\"> <br><br></form> <button onclick=\"sendIP()\" type=\"submit\">Enviar</button> </td></tr></table></html><script>function sendWI(){SSID=document.getElementsByName(\"SSID\")[0].value; PASS=document.getElementsByName(\"PASS\")[0].value; if (SSID !=\"\" && PASS !=\"\"){document.getElementsByName(\"WI_F\")[0].submit();}else{alert(\"Error\");}}function sendIP(){IP=document.getElementsByName(\"IP\")[0].value; PORT=document.getElementsByName(\"PORT\")[0].value; status=true; if (IP !=\"\" && PORT !=\"\"){IP=IP.split(\".\"); if (IP.length==4){if (!isNaN(IP[0]) && !isNaN(IP[1]) && !isNaN(IP[2]) && !isNaN(IP[3]) && !isNaN(PORT)){document.getElementsByName(\"IP_F\")[0].submit();}else{alert(\"Error\");}}else{alert(\"Error\");}}else{alert(\"Error\");}}</script>"
#define result "<html><title>Wifi</title>Configurado com sucesso!</html>\r"

typedef struct 
{
    char IP_ADDR[17];
    unsigned int PORT_NUMBER;
    char ValidData;
}IP_CONFIG;

typedef struct
{
    char WIFI_SSID[64];
    char WIFI_PASS[64];
    char ValidData;
}WIFI_CONFIG;

typedef struct
{
    int Ax, Ay, Az, Gx, Gy, Gz;
}MPU_BIAS;

IP_CONFIG CONFIG_IP;
WIFI_CONFIG CONFIG_WIFI;
MPU_BIAS BIAS_MPU;

#define ADDR_IP 0
#define ADDR_WIFI sizeof(IP_CONFIG)+1
#define ADDR_MPU ADDR_WIFI+sizeof(WIFI_CONFIG)+1

char resetCause;

#define SOFTWARE_RESET 0
#define WDT_RESET 1
#define POWER_DOWN_RESET 2
#define BROWN_OUT_RESET 3

bit ResetConfig;


#define LED_GREEN LATBbits.LATB4
#define LED_BLUE LATBbits.LATB3
#define LED_RED LATBbits.LATB2

#define LED_GREEN_D TRISBbits.RB4
#define LED_BLUE_D TRISBbits.RB3
#define LED_RED_D TRISBbits.RB2

#define SDA_PIN PORTCbits.RC4
#define SCL_PIN PORTCbits.RC3	
#define SDA_DIR TRISCbits.RC4
#define SCL_DIR TRISCbits.RC3
