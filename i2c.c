#include "i2c.h"
#include "main.h"
#include "soft_uart.h"
#include "plib.h"

void I2C_Master_Init()
{
    OpenI2C1(MASTER, SLEW_OFF);
    SSPADD = (_XTAL_FREQ / (1600000)) - 1;
    TRISCbits.RC4 = 1;
    TRISCbits.RC3 = 1;
    PORTC = 0;
    ANSELC = 0;
}

void I2C_Master_Wait()
{
    IdleI2C1();
}

void I2C_Master_Start()
{
    I2C_Master_Wait();
    StartI2C1();
}

void I2C_Master_Stop()
{
    I2C_Master_Wait();
    StopI2C1();
}

char I2C_Master_Write(char d)
{
    I2C_Master_Wait();
    SSP1BUF = d;
    if (SSP1CON1bits.WCOL)
        return ( -1);

    while (SSP1STATbits.BF);
    IdleI2C1();
    if (SSP1CON2bits.ACKSTAT)
        return ( -2);
    else return ( 0);
}

char I2C_Master_Read(unsigned char a)
{
    char temp;
    I2C_Master_Wait();
    SSP1CON2bits.RCEN = 1;
    I2C_Master_Wait();
    temp = SSP1BUF;
    
    if (a == 0) {
        AckI2C1();
    }
    else {
        NotAckI2C1();
    }
    return temp;
}

char I2C_Write_Reg(char mpuAddress, char registerAddress, char registerValue)
{
    I2C_Master_Start();
    if (I2C_Master_Write(mpuAddress * 2) == 0) {
        if (I2C_Master_Write(registerAddress) == 0) {
            if (I2C_Master_Write(registerValue) == 0) {
                I2C_Master_Stop();
                return 1;
            }
            I2C_Master_Stop();
            return 0;
        }
        I2C_Master_Stop();
        return 0;
    }
    I2C_Master_Stop();
    return 0;
}

char I2C_Read_Reg(char mpuAddress, char registerAddress)
{
    char data = 0;
    I2C_Master_Start();
    if (I2C_Master_Write(mpuAddress * 2) == 0) {
        if (I2C_Master_Write(registerAddress) == 0) {
            I2C_Master_Start();
            if (I2C_Master_Write(mpuAddress * 2 + 1) == 0) {
                data = I2C_Master_Read(1);
                I2C_Master_Stop();
                return data;
            }
        }
    }
    return 0x00;
}

void
I2C_Get_Reg(char mpuAddress, char registerAddr)
{
    char data;
    char buffer[100];
    data = I2C_Read_Reg(mpuAddress, registerAddr);
    sprintf(buffer, "Register address: %d value: 0x%02X\n\r", registerAddr, data);
    Debug.Send(buffer);
}
