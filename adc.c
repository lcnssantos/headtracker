/*
 * File:   adc.c
 * Author: Luciano
 *
 * Created on August 23, 2018, 7:48 PM
 */

#include "main.h"

/*
@purpose: This function is used to setup the ADC 
@parameters: void
@return: void
@version: 0.1
*/

void ADC_Setup()
{
    ADCS0 = 0;
    ADCS1 = 0;
    ADCS2 = 0;
    ADON = 1;
    PVCFG0 = 0;
    PVCFG1 = 0;
    NVCFG0 = 0;
    NVCFG1 = 0;
    ADFM = 1;
}


/*
@purpose: This function is used to read a specific ADC channel
@parameters: channel to be readed
@return: ADC value
@version: 0.1
*/

unsigned short ADC_Read(unsigned char channel)
{
    ADCON0bits.CHS = channel;
    while (GODONE);
    GODONE = 1;
    while (GODONE);
    return ADRESH << 8 | ADRESL;
}
