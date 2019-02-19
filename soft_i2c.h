/*
 * File Name: soft_i2c.h
 * Author: Siddharth Chandrasekaran
 * Website	: http://embedjournnal.com/
 * Created on July 27, 2012, 12:12 PM
*/



#ifndef SOFT_I2C_H
#define SOFT_I2C_H



unsigned char i2cReadByte(void);
void i2cWriteByte(unsigned char);
void i2cNack(void);
void i2cAck(void);
void i2cStart(void);
void i2cStop(void);
void i2cHighSda(void);
void i2cLowSda(void);
void i2cHighScl(void);
void i2cLowScl(void);

#endif