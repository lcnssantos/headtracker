void EEPROM_Write_Byte(unsigned char address, unsigned char data);
char EEPROM_Read_Byte(unsigned char address);
void EEPROM_Put_Object(char * object, unsigned char length,unsigned int address);
void EEPROM_Get_Object(char * object, unsigned char length,unsigned int address);