/**
 * File Name: soft_i2c.c
 * Author: Siddharth Chandrasekaran
 * Website	: http://embedjournnal.com/
 * Created on July 27, 2012, 12:12 PM
 */

#include "soft_i2c.h"
#include "main.h"

unsigned char i2cReadByte(void)
{
    unsigned char inByte, n;
    i2cHighSda();
    for (n = 0; n < 8; n++) {
        i2cHighScl();

        if (SDA_PIN)
            inByte = (inByte << 1) | 0x01; // msbit first
        else
            inByte = inByte << 1;
        i2cLowScl();
    }
    return (inByte);
}

void i2cWriteByte(unsigned char outByte)
{
    unsigned char n;
    for (n = 0; n < 8; n++) {
        if (outByte & 0x80)
            i2cHighSda();
        else
            i2cLowSda();
        i2cHighScl();
        i2cLowScl();
        outByte = outByte << 1;
    }
    i2cHighSda();
}

void i2cNack(void)
{
    i2cHighScl();
    i2cLowScl(); // bring data high and clock
}

void i2cAck(void)
{
    i2cLowSda();
    i2cHighScl();
    i2cLowScl();
    i2cHighSda(); // bring data low and clock
}

void i2cStart(void)
{
    i2cLowScl();
    i2cHighSda();
    i2cHighScl();
    i2cLowSda();
    i2cLowScl(); // bring SDA low while SCL is high
}

void i2cStop(void)
{
    i2cLowScl();
    i2cLowSda();
    i2cHighScl();
    i2cHighSda(); // bring SDA high while SCL is high
}

void i2cHighSda(void)
{
    SDA_DIR = 1; // bring SDA to high impedance
    __delay_us(5);
}

void i2cLowSda(void)
{
    SDA_PIN = 0;
    SDA_DIR = 0; // output a logic zero
    __delay_us(5);
}

void i2cHighScl(void)
{
    SCL_DIR = 1; // bring SCL to high impedance
    __delay_us(5);
}

void i2cLowScl(void)
{
    SCL_PIN = 0;
    SCL_DIR = 0;
    __delay_us(5);
}