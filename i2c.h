void I2C_Master_Init();
void I2C_Master_Wait();
void I2C_Master_Start();
void I2C_Master_Stop();
char I2C_Master_Write(char d); 
char I2C_Master_Read(unsigned char a);
char I2C_Write_Reg(char mpuAddress, char registerAddress, char registerValue);
char I2C_Read_Reg(char mpuAddress, char registerAddress);
void I2C_Get_Reg(char mpuAddress,char registerAddr);