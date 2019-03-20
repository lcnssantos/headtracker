#include "main.h"
#include "eeprom.h"

/* 
@purpose: Write a byte to specific address of the EEPROM
@parameters: address: Address of the memory; data: The byte to be writed
@return: void;
@version: V0.1
*/

void EEPROM_Write_Byte(unsigned char address, unsigned char data)
{
    EEADR = address;
    EEDATA = data;

    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS = 0;
    EECON1bits.WREN = 1;

    EECON2 = 0x55;
    EECON2 = 0x0AA;

    EECON1bits.WR = 1;

    while (EECON1bits.WR || EECON1bits.WRERR);
    EECON1bits.WREN = 0;
    EEDATA = 0;
}

/*
@purpose: Read a specific address of EEPROM
@parameters: address: address of memory
@return: The byte readed
@version: V0.1
*/

char EEPROM_Read_Byte(unsigned char address)
{
    EEADR = address;
    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS = 0;
    EECON1bits.RD = 1;
    while (EECON1bits.RD);

    return EEDATA;
}

/*
@purpose: Put a object to an address of the EEPROM
@paramaters: object: pointer to the object, length: the length of the object, address: the address of memory
@return: void
@version: V0.1
*/

void EEPROM_Put_Object(char * object, unsigned char length, unsigned int address)
{
    unsigned int i;
    for (i = address; (i - address) < length; i++) {
        EEPROM_Write_Byte(i, *object);
        object++;
    }
}

/*
@purpose: Get a object from specific address of the EEPROM
@paramaters: object: pointer to the object, length: the length of the object, address: the address of memory
@return: void
@version: V0.1
*/
void EEPROM_Get_Object(char * object, unsigned char length, unsigned int address)
{
    unsigned int i;
    for (i = address; (i - address) < length; i++) {
        *object = EEPROM_Read_Byte(i);
        object++;
    }
}
