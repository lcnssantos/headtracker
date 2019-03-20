/*
This file is the main core of the code. All application code is here
*/


#include "app.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include "eeprom.h"


/* 
@purpose: Setup the System Timer with High Interrupt Priority with 1ms overflow period
@parameters: void
@return: void
@version: 0.1
*/

void
InitTimerCount()
{
    Timer0_Construct(&Timer);
    TimeCountConstruct(&TimerCount);
    Timer.ClkSrc = INTERNAL_CLOCK;
    Timer.Mode = _8_BIT;
    Timer.Opr = YES;
    Timer.PSA = PRESCALER_ON;
    Timer.PSAConfig = PRESCALER_16;
    Timer.Preload = 6;
    Timer.PreloadOn = YES;
    Timer.IntConfig(INTERRUPT_HIGH_PRIORITY);
    Timer.IntProc = TimerCount.Update;
    Timer.Init(&Timer);
}

/*
@purpose: Setup the interrupts of CPU
@parameters: void
@return: void
@version: 0.1
*/


void
InitInterrupt()
{
    RCONbits.IPEN = 1;
    INTCONbits.GIEH = 1;
    INTCONbits.PEIE = 1;

    IPR1bits.RC1IP = 1;
    PIE1bits.RC1IE = 1;

    INTCON2bits.INTEDG0 = 0;
    INTCONbits.INT0IE = 1;

    INTCON2bits.INTEDG1 = 1;

    INTCON3bits.INT1IP = 1;
}

/*
@purpose: Configure IO pins
@parameters: void
@return: void
@version: 0.1
*/

void
InitIO()
{
    LED_BLUE_D = 0;
    LED_GREEN_D = 0;
    LED_RED_D = 0;
    LED_BLUE = 1;
    LED_GREEN = 1;
    LED_RED = 1;

    ANSELC = 0;

    ANSELAbits.ANSA2 = 1;
    TRISAbits.TRISA2 = 1;


}


/*
@purpose: Send the Battery Status Package
@parameters: void
@return: void
@version: 0.1
*/


void
Battery_Status()
{
    /* buffer for debug */
    char buffer[50];

    /* the package */
    char BatteryPackage[4];
    
    unsigned short adcValue;
    
    /* Get the ADC value */
    adcValue = ADC_Read(2);

    /* if the device isn't in the configuration mode */
    if (!configModeStatus) 
    {
        /* send the adc value for debug serial port */
        sprintf(buffer, "Battery ADC %u\n", adcValue);
        Debug.Send(buffer);
        /* */

        /* mount the package */
        BatteryPackage[0] = 0x55;
        BatteryPackage[1] = adcValue;
        BatteryPackage[2] = adcValue >> 8;
        BatteryPackage[3] = BatteryPackage[0] + BatteryPackage[1] + BatteryPackage[2];
        /* */
        
        /* send the package via wifi */
        ESP_Send_Transparent_Data(BatteryPackage, 4);
    }
}

/*
@purpose: Blink Led - unused
@parameters: void
@return: void
@version: 0.1
*/

void
Led_Blink2()
{
    //PORTDbits.RD1 = !PORTDbits.RD1;
}


/*
@purpose: Configure the tasks for the Task Schedule
@parameters: void
@return: void
@verson: V0.1
*/
void
Task_Init()
{
    BattteryStatus.Execute = Battery_Status;
    BattteryStatus.interval = 5000;
    BattteryStatus.active = YES;
    TaskScheduler(BattteryStatus);

    LedBlink_2.Execute = Led_Blink2;
    LedBlink_2.interval = 500;
    LedBlink_2.active = YES;

    TaskScheduler(LedBlink_2);

    DMP.Execute = Frame_Update;
    DMP.active = YES;
    DMP.interval = 10;

    TaskScheduler(DMP);

    ConfigMode.Execute = configMode;
    ConfigMode.active = YES;
    ConfigMode.interval = 1000;
    TaskScheduler(ConfigMode);
}

/*
@purpose: Start Serial Port
@parameters: void
@return: void
@version: V0.1
*/

void
Comm_Init()
{
    Debug.Send("Init Serial Port at 57600bps\r");
    UART_Init(57600);
}

/*
@purpose: Send the Version to debug
@parameters: void
@return: void
@version: V0.1
*/

void
Send_Version()
{
    Soft_UART_Construct(&Debug);
    Debug.Send(_VERSION_);
    Debug.Send("\n\r");
}

/*
@purpose: Update Frame of device
@parameters: void
@return: void
@version: V0.1
*/
void
Frame_Update()
{
    /* buffer for debug */
    char buffer[50];
    
    /* fifo buffer count */
    char fifocount = 0;
    
    /* fifo buffer */
    char fifobuffer[42];
    
    /* the quartenion */
    int quartenion[4];

    /* if MPU6050 data is available */
    if (MPU_Sensor.DataReady) 
    {
        /* Get MPU6050 Interrupt Status */
        MPU_Sensor.IntStatus = MPU_Get_Int_Status(mpuAddr);
        
        /* if FIFO Overflow occurred reset the FIFO */
        if (MPU_Sensor.IntStatus & 0x10) 
        {
            MPU_Reset_FIFO(mpuAddr);
            Debug.Send("Fifo Overflow\r");
        }
        
        /* else If data is available */
        else if (MPU_Sensor.IntStatus & 0x02) 
        {
            /* wait for a complete package in fifo buffer */
            while (fifocount < MPU_Sensor.PacketSize) 
            {
                fifocount = MPU_Get_FIFO_Count(mpuAddr);
            }

            /* adjust fifocount to the actual fifo size*/
            fifocount = fifocount - MPU_Sensor.PacketSize;
            
            /* read the fifo package */
            MPU_Get_Fifo_Bytes(mpuAddr, fifobuffer, MPU_Sensor.PacketSize);
            
            /* send the fifo package via WiFi*/
            ESP_Send_Transparent_Data(fifobuffer, MPU_Sensor.PacketSize);
        }
        
        /* else print the Int status in the debug */
        else 
        {
            sprintf(buffer, "Int Status: 0x%2X\r", MPU_Sensor.IntStatus);
            Debug.Send(buffer);
        }
        /* clear dataready flag */
        MPU_Sensor.DataReady = _FALSE_;
    }
}


/*
@purpose: Start ESP8266 in client mode
@parameters: void
@return: void
@version: V0.1
*/

void
Wifi_Normal_Init()
{

    /* if isn't in the configuration mode, test ESP8266 Startup */
    if (!configModeStatus)
        while (!ESP_Test_Startup() && !configModeStatus) {
            __delay_ms(2000);
        }
    
    /* if isn't in the configuration mode, put the ESP8266 in the Station mode */
    if (!configModeStatus)
        while (!ESP_Wifi_Setup(ESP8266_STATION_MODE) && !configModeStatus) {
            __delay_ms(2000);
        }
    
    /* if isn't in the configuration mode, connect to the network seted in the EEPROM */
    if (!configModeStatus)
        while (!ESP_Connect_Ap(CONFIG_WIFI.WIFI_SSID, CONFIG_WIFI.WIFI_PASS) && !configModeStatus) {
            __delay_ms(2000);
        }
    
    /* if isn't in the configuration mode, get TCP IP status */
    if (!configModeStatus)
        while (!ESP_TCPIP_Status() && !configModeStatus) {
            __delay_ms(2000);
        }
    
    /* if isn't in the configuration mode, connect to the UDP server seted in the EEPROM */
    if (!configModeStatus)
        while (!ESP_TCPIP_Connect("UDP", CONFIG_IP.IP_ADDR, CONFIG_IP.PORT_NUMBER) && !configModeStatus) {
            __delay_ms(2000);
        }
    
    /* if isn't in the configuration mode, put the ESP8266 in the transparent mode */
    if (!configModeStatus)
        while (!ESP_Transparent_Mode() && !configModeStatus) {
            __delay_ms(2000);
        }
    
    /* if isn't in the configuration mode, start the transparent transmission */
    if (!configModeStatus)
        while (!ESP_Start_Transparent_Transmission() && !configModeStatus) {
            __delay_ms(2000);
        }
    
    /* send "WiFi ready" to de debug */
    Debug.Send("Wifi Ready\r");
}


/*
@purpose: Start MPU6050 Digital Motion Processor
@parameters: void
@return: void
@version: V0.1
*/


void
MPU_DMP_Init()
{
    /* Buffer for debug */
    char buffer[10];
    
    /* Start I2C Port as Master */
    I2C_Master_Init();

    /* Turn on the MPU6050 */
    MPU_CH_DIRECTION = 0;
    MPU_CH = 1;


    /* Start the MPU6050 class */
    MPU_Construct(&MPU_Sensor);
    
    /* Start the initial Configuration of MPU6050 */
    MPU_Sensor.Init();
    
    /* Start Digital Motion Processor */
    MPU_Sensor.DMPStatus = DMP_Init(&MPU_Sensor);

    /* If Digital Motion Processor Startup is ok */
    if (MPU_Sensor.DMPStatus == _DMP_OK_) 
    {
        /* Set the Offsets of Gyroscope */
        MPU_Set_X_Gyro_Offset(BIAS_MPU.Gx, mpuAddr);
        MPU_Set_Y_Gyro_Offset(BIAS_MPU.Gy, mpuAddr);
        MPU_Set_Z_Gyro_Offset(BIAS_MPU.Gz, mpuAddr);
        /* */

        /* Enable the Digital Motion Processor */
        MPU_Set_DMP_Enabled(mpuAddr, _TRUE_);
        /* Enable the FIFO Buffer */
        MPU_Set_FIFO_Enabled(mpuAddr, _TRUE_);
        /* Get Interrupt Status */
        MPU_Sensor.IntStatus = MPU_Get_Int_Status(mpuAddr);
        /* Start the Tasks */
        Task_Init();
        /* Send "DMP Ready to the debug */
        Debug.Send("DMP Ready\r");
    }
    /* if Digital Motion processor startup failed */
    else 
    {
        /* Send the status to the debug serial port */
        Debug.Send("DMP Init failed\r");
        sprintf(buffer, "STS: %d\r", MPU_Sensor.DMPStatus);
        Debug.Send(buffer);
        /* reset the cpu */
        reset();
    }
}


/*
@purpose: Configuration Mode: Input of WiFi network parameters as well as UDP server parameters
@parameters: void
@return: void
@version: V0.1
*/

void
configMode()
{
    /* loop control variables */
    char i, j;
    
    /* configuration buffers */
    char config[128];
    char configFinal[128];
    
    /* */
    
    /* general purpose pointer */
    char * ponteiro;
    
    /* variable to address the ESP8266 connection */
    char link;
    
    /* if in the configuration mode */
    if (configModeStatus) 
    {
        /* turn off the MPU6050 */
        MPU_CH = 0;
        
        /* str to debug */
        Debug.Send("Starting Configurator...\r");
        
        /* Clear the Config buffer*/
        ClearAnyBuffer(config, 128);
        
        /* Setup the WiFi Acess point name */
        strcpy(config, "HeadTracker ");
        strcat(config, _VERSION_);
        
        /* Clear the WiFi Buffer */
        ClearAnyBuffer(Wifi_Buffer, SIZE_OF_WIFI_BUFFER);
        
        /* turn off the system clock */
        INTCONbits.TMR0IE = 0;
        
        /* reset the ESP8266 */
        __delay_ms(500);
        
        ESP_Reset();
        
        __delay_ms(500);
        
        /* */
        
        /* Test the startup of ESP8266 */
        while (!ESP_Test_Startup()) {
            __delay_ms(1000);
        }
        
        /* Set ESP8266 as Acess Point */
        while (!ESP_Wifi_Setup(ESP8266_AP_MODE)) {
            __delay_ms(1000);
        }
        
        /* Setup the network */
        while (!ESP_AP_Setup(config, "headtracker", 5, ESP8266_ENC_WPA2_PSK)) {
            __delay_ms(1000);
        }
        
        /* Accept multiple connections */
        while (!ESP_TCPIP_Mux(ESP8266_MULTIPLE_CONNECTION)) {
            __delay_ms(1000);
        }
        
        /* Start server at port 80 */
        while (!ESP_Start_Server(80)) {
            __delay_ms(1000);
        }
        
        /* If WiFi configuration is not valid, turn on the green led */
        if (CONFIG_WIFI.ValidData != 0xAA) 
        {
            LED_GREEN = 1;
        }

        /* if server configuration is not valid, turn on the red led */
        if (CONFIG_IP.ValidData != 0xAA) {
            LED_RED = 1;
        }

        /* clear the wifi buffer */
        ClearAnyBuffer(Wifi_Buffer, SIZE_OF_WIFI_BUFFER);
        /* send Configuration Ready */
        Debug.Send("Configurator ready\r");
        
        /* while device is in the configuration mode */
        while (configModeStatus) {
            //PORTD = 255;
            if (strstr(Wifi_Buffer, "+IPD")) {
                ponteiro = strstr(Wifi_Buffer, "+IPD");
                link = ponteiro[5];
                if (link >= '0' && link <= '3') {
                    if (strstr(Wifi_Buffer, "ip.html")) {

                        Debug.Send("IP\r");
                        ponteiro = strstr(Wifi_Buffer, "?");
                        ponteiro = ponteiro + 1;
                        for (i = 0; *ponteiro != 'H'; i++) {
                            /*   config[i] = * ponteiro;
                               ponteiro++;*/
                            config[i] = *ponteiro;
                            ponteiro++;
                        }
                        Debug.Send(config);
                        Debug.Send("\r");

                        char * ip_start, * ip_end, * port_start, * port_end;
                        char port[5];

                        ip_start = strstr(config, "=") + 1;
                        ip_end = strstr(config, "&") - 1;

                        port_start = strstr(config, "PORT=") + 5;
                        port_end = port_start;

                        while (*port_end != 0x20)
                            port_end++;

                        ClearAnyBuffer(CONFIG_IP.IP_ADDR, sizeof (CONFIG_IP.IP_ADDR));
                        memcpy(CONFIG_IP.IP_ADDR, ip_start, ip_end - ip_start + 1);
                        ClearAnyBuffer(port, 5);
                        memcpy(port, port_start, port_end - port_start);

                        CONFIG_IP.PORT_NUMBER = atoi(port);
                        CONFIG_IP.ValidData = 0xAA;
                        EEPROM_Put_Object(&CONFIG_IP, sizeof (IP_CONFIG), ADDR_IP);

                        ESP_TCPIP_Send(link, result);
                        ESP_TCPIP_Close(link - 48);
                        configModeStatus = _FALSE_;
                        Debug.Send("Gravou IP\r");
                    }
                    else if (strstr(Wifi_Buffer, "wifi.html")) {
                        Debug.Send("Wifi\r");
                        ClearAnyBuffer(config, 128);
                        ClearAnyBuffer(configFinal, 128);
                        ClearAnyBuffer(CONFIG_WIFI.WIFI_PASS, sizeof (CONFIG_WIFI.WIFI_PASS));
                        ClearAnyBuffer(CONFIG_WIFI.WIFI_SSID, sizeof (CONFIG_WIFI.WIFI_SSID));
                        while (!strstr(Wifi_Buffer, "SSID"));
                        ponteiro = strstr(Wifi_Buffer, "?");
                        ponteiro = ponteiro + 1;
                        for (i = 0; *ponteiro != 'H'; i++) {
                            config[i] = *ponteiro;
                            ponteiro++;
                        }
                        ponteiro = 0x00;


                        for (i = 0, j = 0; j < 128; j++) {
                            if (config[j] != '%') {
                                configFinal[i] = config[j];
                                i++;
                            }
                            else {
                                ponteiro = &config[j];
                                ponteiro++;
                                char c[2];
                                c[0] = *ponteiro;
                                ponteiro++;
                                c[1] = *ponteiro;

                                long int caractere = strtol(c, NULL, 16);
                                configFinal[i] = caractere;
                                j = j + 2;
                                i++;
                            }
                        }

                        char * SSID_START = strstr(configFinal, "SSID=") + 5;
                        char * SSID_FINAL = strstr(configFinal, "&PASS=");

                        memcpy(CONFIG_WIFI.WIFI_SSID, SSID_START, SSID_FINAL - SSID_START);

                        char * PASS_START = strstr(configFinal, "PASS=") + 5;
                        char * PASS_END = PASS_START;

                        while (*PASS_END != 0x20)
                            PASS_END++;

                        memcpy(CONFIG_WIFI.WIFI_PASS, PASS_START, PASS_END - PASS_START);

                        CONFIG_WIFI.ValidData = 0xAA;

                        EEPROM_Put_Object(&CONFIG_WIFI, sizeof (WIFI_CONFIG), ADDR_WIFI);

                        Debug.Send(configFinal);
                        Debug.Send("\r");

                        ESP_TCPIP_Send(link, result);
                        ESP_TCPIP_Close(link - 48);
                        configModeStatus = _FALSE_;
                        Debug.Send("Gravou Wifi\r");
                    }
                    else {
                        ESP_TCPIP_Send(link, page);
                        __delay_ms(500);
                        ESP_TCPIP_Close(link - 48);
                    }
                }
                else
                    Debug.Send("Invalid Link!");
                for (i = 0; i < 4; i++) {
                    ESP_TCPIP_Close(i);
                }
            }
        }
        reset();
    }
}

void
reset()
{
    Debug.Send("Reseta\r");
    asm("RESET");
}

void
GetResetCause()
{
    if (!RCONbits.RI)
        resetCause = SOFTWARE_RESET;
    else if (!RCONbits.TO)
        resetCause = WDT_RESET;
    else if (!RCONbits.PD)
        resetCause = POWER_DOWN_RESET;
    else if (!RCONbits.BOR)
        resetCause = BROWN_OUT_RESET;
    RCON = 0;
}

void
printResetCause()
{
    unsigned const char* strings[4] = {"SOFT", "WDT", "PWRD", "BROWN"};
    char buffer[30];
    sprintf(buffer, "Cause: %s\r", strings[resetCause]);
    Debug.Send(buffer);
}
