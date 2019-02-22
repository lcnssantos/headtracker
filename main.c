#include "main.h"

void main(void)
{

    GetResetCause();
    mpuAddr = 0x68;
    /* */

    EEPROM_Get_Object(&CONFIG_IP, sizeof (IP_CONFIG), ADDR_IP);
    EEPROM_Get_Object(&CONFIG_WIFI, sizeof (WIFI_CONFIG), ADDR_WIFI);
    EEPROM_Get_Object(&BIAS_MPU, sizeof (MPU_BIAS), ADDR_MPU);


    /* TASK COUNT */
    TaskCount = 0;
    /* */

    /* CLOCK CONFIG */
    OscConfig();
    /* */


    /* INIT TIMER COUNT SYSTEM*/
    InitTimerCount();
    /* */

    /* INIT IO PINS */
    InitIO();
    /* */

    /* SEND VERSION DEBUG*/
    Send_Version();
    /* */

    ADC_Setup();
    /* INIT MPU*/
    //MPU_DMP_Init();
    /* */

    /* INIT COMM PORTS*/
    Comm_Init();
    /* */

    /* INIT INTERRUPT CONFIG */
    InitInterrupt();
    /* */

    printResetCause();

    /* STANDBY FOR ESP RESET*/


    //__delay_ms(2000);

    /* */

    //ESP_Reset();

    /* STANDBY FOR ESP RESET*/
    //__delay_ms(2000);

    /* */

    //AT_Command("AT+UART_DEF=57600,8,1,0,0");
    LED_GREEN = 0;
    LED_RED = 0;
    LED_BLUE = 0;

    if (CONFIG_IP.ValidData == 0xAA && CONFIG_WIFI.ValidData == 0xAA) {
        /* WIFI INIT */
        Wifi_Normal_Init();
        /* */
        /* INIT MPU*/
        MPU_DMP_Init();
        /* */
    }
    else 
    {
        MPU_DMP_Init();
        for(char i = 0; i < 50; i++)
        {
            LED_BLUE = !LED_BLUE;
            __delay_ms(30);
        }
    }


    INTCON3bits.INT1IE = 1;
    //PORTD = 0;

    ResetConfig = _FALSE_;
    while (1) {
        TaskExecute();
        if (CalibrateMode == _TRUE_) {
            INTCONbits.TMR0IE = 0;
            MPU_Calibrate();
        }

    }
}
