#include "app.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include "eeprom.h"

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

void
Battery_Status()
{
    char buffer[50];

    char BatteryPackage[4];
    unsigned short adcValue;
    adcValue = ADC_Read(2);

    if (!configModeStatus) {
        sprintf(buffer, "Battery ADC %u\n", adcValue);
        Debug.Send(buffer);

        BatteryPackage[0] = 0x55;
        BatteryPackage[1] = adcValue;
        BatteryPackage[2] = adcValue >> 8;
        BatteryPackage[3] = BatteryPackage[0] + BatteryPackage[1] + BatteryPackage[2];

        ESP_Send_Transparent_Data(BatteryPackage, 4);
    }
}

void
Led_Blink2()
{
    //PORTDbits.RD1 = !PORTDbits.RD1;
}

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

void
Comm_Init()
{
    Debug.Send("Init Serial Port at 57600bps\r");
    UART_Init(57600);
}

void
Send_Version()
{
    Soft_UART_Construct(&Debug);
    Debug.Send(_VERSION_);
    Debug.Send("\n\r");
}

void
Frame_Update()
{
    char buffer[50];
    char fifocount = 0;
    char fifobuffer[42];
    int quartenion[4];

    if (MPU_Sensor.DataReady) {
        MPU_Sensor.IntStatus = MPU_Get_Int_Status(mpuAddr);
        if (MPU_Sensor.IntStatus & 0x10) {
            MPU_Reset_FIFO(mpuAddr);
            Debug.Send("Fifo Overflow\r");
        }
        else if (MPU_Sensor.IntStatus & 0x02) {
            while (fifocount < MPU_Sensor.PacketSize) {
                fifocount = MPU_Get_FIFO_Count(mpuAddr);
            }

            fifocount = fifocount - MPU_Sensor.PacketSize;
            MPU_Get_Fifo_Bytes(mpuAddr, fifobuffer, MPU_Sensor.PacketSize);
            ESP_Send_Transparent_Data(fifobuffer, MPU_Sensor.PacketSize);
        }
        else {
            sprintf(buffer, "Int Status: 0x%2X\r", MPU_Sensor.IntStatus);
            Debug.Send(buffer);
        }
        MPU_Sensor.DataReady = _FALSE_;
    }
}

void
Wifi_Normal_Init()
{
    //EEPROM_Get_Object(&CONFIG_IP,sizeof(IP_CONFIG),ADDR_IP);
    if (!configModeStatus)
        while (!ESP_Test_Startup() && !configModeStatus) {
            __delay_ms(2000);
        }
    if (!configModeStatus)
        while (!ESP_Wifi_Setup(ESP8266_STATION_MODE) && !configModeStatus) {
            __delay_ms(2000);
        }
    if (!configModeStatus)
        while (!ESP_Connect_Ap(CONFIG_WIFI.WIFI_SSID, CONFIG_WIFI.WIFI_PASS) && !configModeStatus) {
            __delay_ms(2000);
        }
    if (!configModeStatus)
        while (!ESP_TCPIP_Status() && !configModeStatus) {
            __delay_ms(2000);
        }
    if (!configModeStatus)
        while (!ESP_TCPIP_Connect("UDP", CONFIG_IP.IP_ADDR, CONFIG_IP.PORT_NUMBER) && !configModeStatus) {
            __delay_ms(2000);
        }
    if (!configModeStatus)
        while (!ESP_Transparent_Mode() && !configModeStatus) {
            __delay_ms(2000);
        }
    if (!configModeStatus)
        while (!ESP_Start_Transparent_Transmission() && !configModeStatus) {
            __delay_ms(2000);
        }
    Debug.Send("Wifi Ready\r");
}

void
MPU_DMP_Init()
{
    char buffer[10];
    I2C_Master_Init();

    MPU_CH_DIRECTION = 0;
    MPU_CH = 1;


    MPU_Construct(&MPU_Sensor);
    MPU_Sensor.Init();
    MPU_Sensor.DMPStatus = DMP_Init(&MPU_Sensor);

    if (MPU_Sensor.DMPStatus == _DMP_OK_) {
        MPU_Set_X_Gyro_Offset(BIAS_MPU.Gx, mpuAddr);
        MPU_Set_Y_Gyro_Offset(BIAS_MPU.Gy, mpuAddr);
        MPU_Set_Z_Gyro_Offset(BIAS_MPU.Gz, mpuAddr);
        //MPU_Set_Z_Accel_Offset(1788,mpuAddr);

        MPU_Set_DMP_Enabled(mpuAddr, _TRUE_);
        MPU_Set_FIFO_Enabled(mpuAddr, _TRUE_);
        MPU_Sensor.IntStatus = MPU_Get_Int_Status(mpuAddr);
        Task_Init();
        Debug.Send("DMP Ready\r");
    }
    else {
        Debug.Send("DMP Init failed\r");
        sprintf(buffer, "STS: %d\r", MPU_Sensor.DMPStatus);
        Debug.Send(buffer);
        reset();
    }
}

void
configMode()
{
    char i, j;
    char config[128];
    char configFinal[128];
    char * ponteiro;
    char link;
    if (configModeStatus) {
        MPU_CH = 0;
        Debug.Send("Starting Configurator...\r");
        ClearAnyBuffer(config, 50);
        strcpy(config, "HeadTracker ");
        strcat(config, _VERSION_);
        ClearAnyBuffer(Wifi_Buffer, SIZE_OF_WIFI_BUFFER);
        INTCONbits.TMR0IE = 0;
        __delay_ms(500);
        ESP_Reset();
        __delay_ms(500);
        while (!ESP_Test_Startup()) {
            __delay_ms(1000);
        }
        while (!ESP_Wifi_Setup(ESP8266_AP_MODE)) {
            __delay_ms(1000);
        }
        while (!ESP_AP_Setup(config, "headtracker", 5, ESP8266_ENC_WPA2_PSK)) {
            __delay_ms(1000);
        }
        while (!ESP_TCPIP_Mux(ESP8266_MULTIPLE_CONNECTION)) {
            __delay_ms(1000);
        }
        while (!ESP_Start_Server(80)) {
            __delay_ms(1000);
        }
        if (CONFIG_WIFI.ValidData != 0xAA) {
            LED_GREEN = 1;
        }

        if (CONFIG_IP.ValidData != 0xAA) {
            LED_RED = 1;
        }

        ClearAnyBuffer(Wifi_Buffer, SIZE_OF_WIFI_BUFFER);
        Debug.Send("Configurator ready\r");
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