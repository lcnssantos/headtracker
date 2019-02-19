#include "main.h"
#include "interrupt.h"

void interrupt low_priority LowIsr(void)
{

}

void interrupt high_priority HighIsr(void)
{
    Timer.IntProc(&TimerCount);
    char temp;
    char buffer[10];
    if (PIR1 & 0x20) {

        Wifi_Buffer[Wifi_Buffer_Pos] = RCREG;
        Wifi_Buffer_Pos++;
        if (Wifi_Buffer_Pos > SIZE_OF_WIFI_BUFFER) {
            Wifi_Buffer_Pos = 0;
        }
        PIR1 = PIR1 & 0xDF;
    }
    else if (INTCONbits.INT0IF) {
        INTCONbits.INT0IF = 0;
        temp = 0;
        LED_GREEN = 0;
        LED_BLUE = 0;
        LED_RED = 0;
        while (!PORTBbits.RB0) {
            __delay_ms(100);
            temp++;
            if (temp > 10 && temp < 20) {
                LED_BLUE = 1;
                //PORTAbits.RA7 = 1;
            }
            else if (temp >= 20 && temp < 30) {
                LED_GREEN = 1;
                __delay_ms(200);
                LED_GREEN = 0;
                temp += 2;
            }
            else if (temp >= 40 && temp < 50) {
                LED_GREEN = 1;

                //PORTAbits.RA6 = 1;
            }
            else if (temp >= 50) {
                LED_RED = 1;
                //PORTAbits.RA5 = 1;
            }
        }
        if (temp > 10 && temp < 20) {
            Debug.Send("Reseta\r");
            reset();
        }
        else if (temp >= 20 && temp < 30) {
            ESP_CH = 0;
            CalibrateMode = _TRUE_;
            //MPU_Calibrate();
        }
        else if (temp >= 40 && temp < 50) {
            Debug.Send("Modo Config\r");
            configModeStatus = _TRUE_;
        }
        else if (temp >= 50) {
            Debug.Send("Sleep\r");
            ESP_CH = 0;
            MPU_CH = 0;
            TRISA = 0xFF;
            TRISB = 0xFF;
            TRISC = 0xFF;
            TRISE = 0xFF;
            LED_GREEN = 0;
            LED_RED = 0;
            LED_BLUE = 0;
            asm("SLEEP");
        }
        //PORTD = 0;
    }
    else if (INTCON3bits.INT1IF) {
        INTCON3bits.INT1IF = 0;
        MPU_Sensor.DataReady = _TRUE_;
        //Frame_Update();
    }
}