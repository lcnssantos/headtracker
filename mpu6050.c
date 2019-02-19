#include "mpu6050.h"
#include "main.h"

#define I2C_GET_REG

#define MPU_DEBUG

#ifdef I2C_GET_REG
#define GET_REG I2C_Get_Reg
#else
#define GET_REG
#endif

#ifdef MPU_DEBUG
#define _DEBUG Debug.Send
#define _FORMAT sprintf
#else
#define _DEBUG
#define _FORMAT
#endif

void MPU_Construct(MPU * MPU)
{
    MPU->Init = MPU_Init;
}

void MPU_Init()
{
    Debug.Send("Inicializando MPU6050\r");
    GET_REG(mpuAddr,MPU6050_RA_WHO_AM_I);
    MPU_Set_Clock_Source(mpuAddr, MPU6050_CLOCK_PLL_XGYRO);
    MPU_Set_Full_Scale_Gyro_Range(mpuAddr, MPU6050_GYRO_FS_250);
    MPU_Set_Full_Scale_Accel_Range(mpuAddr, MPU6050_ACCEL_FS_2);
    MPU_Set_Sleep_Enable(mpuAddr, _FALSE_);
}

void MPU_Set_Clock_Source(char addr, char value)
{
    char data;

    GET_REG(addr, MPU6050_RA_PWR_MGMT_1);

    data = I2C_Read_Reg(addr, MPU6050_RA_PWR_MGMT_1);
    /* CLEAN CLKSEL [2:0]*/
    data = data & 0xF8;
    /* SET CLKSEL [2:0]*/
    data = data | value;

    /* WRITE VALUE ON REGISTER */
    I2C_Write_Reg(addr, MPU6050_RA_PWR_MGMT_1, data);

    GET_REG(addr, MPU6050_RA_PWR_MGMT_1);
}

void MPU_Set_Full_Scale_Gyro_Range(char addr, char value)
{
    char data;

    GET_REG(addr, MPU6050_RA_GYRO_CONFIG);

    data = I2C_Read_Reg(addr, MPU6050_RA_GYRO_CONFIG);
    /* CLEAN FS_SEL [1:0]*/
    data = data & 0xE7;
    /* SET FSE_SEL [1:0]*/
    data = data | value << 3;

    /* WRITE VALUE ON REGISTER */
    I2C_Write_Reg(addr, MPU6050_RA_GYRO_CONFIG, data);
    GET_REG(addr, MPU6050_RA_GYRO_CONFIG);
}

void MPU_Set_Full_Scale_Accel_Range(char addr, char value)
{
    char data;

    GET_REG(addr, MPU6050_RA_ACCEL_CONFIG);

    data = I2C_Read_Reg(addr, MPU6050_RA_ACCEL_CONFIG);
    /* CLEAN FS_SEL [1:0]*/
    data = data & 0xE7;
    /* SET FSE_SEL [1:0]*/
    data = data | value << 3;
    /* WRITE VALUE ON REGISTER */
    I2C_Write_Reg(addr, MPU6050_RA_ACCEL_CONFIG, data);
    GET_REG(addr, MPU6050_RA_ACCEL_CONFIG);
}

void MPU_Set_Sleep_Enable(char addr, char value)
{
    char data;

    GET_REG(addr, MPU6050_RA_PWR_MGMT_1);

    data = I2C_Read_Reg(addr, MPU6050_RA_PWR_MGMT_1);
    /* CLEAN SLEEP BIT*/
    data = data & 0xBF;
    /* SET SLEEP BIT*/
    data = data | value << 6;
    /* WRITE VALUE ON REGISTER */
    I2C_Write_Reg(addr, MPU6050_RA_PWR_MGMT_1, data);
    GET_REG(addr, MPU6050_RA_PWR_MGMT_1);
}

void MPU_Reset(char addr)
{
    char data;
    GET_REG(addr, MPU6050_RA_PWR_MGMT_1);
    data = I2C_Read_Reg(addr, MPU6050_RA_PWR_MGMT_1);
    /* SET RESET BIT*/
    data = data | 0x80;
    /* WRITE VALUE ON REGISTER */
    I2C_Write_Reg(addr, MPU6050_RA_PWR_MGMT_1, data);
    GET_REG(addr, MPU6050_RA_PWR_MGMT_1);
}

void MPU_Set_Memory_Bank(char addr, unsigned short int bank, char prefetchEnabled, char userBank)
{
    GET_REG(addr, MPU6050_RA_BANK_SEL);
    bank &= 0x1F;
    if (userBank)
        bank |= 0x20;
    if (prefetchEnabled)
        bank |= 0x40;
    I2C_Write_Reg(addr, MPU6050_RA_BANK_SEL, bank);
    GET_REG(addr, MPU6050_RA_BANK_SEL);
}

void MPU_Set_Mem_Addr(char addr, char address)
{
    I2C_Write_Reg(addr, MPU6050_RA_MEM_START_ADDR, address);
    //GET_REG(addr,MPU6050_RA_MEM_START_ADDR);
}

char MPU_Read_Memory_Byte(char addr)
{
    char data = I2C_Read_Reg(mpuAddr, MPU6050_RA_MEM_R_W);
    return data;
}

char MPU_Get_OTP_Bank_Valid(char addr)
{
    char data = I2C_Read_Reg(mpuAddr, MPU6050_RA_XG_OFFS_TC);
    data = data & 0x01;
    return data;
}

short int MPU_Get_X_Gyro_Offset_TC(char addr)
{
    char data = I2C_Read_Reg(addr, MPU6050_RA_XG_OFFS_TC);
    data = data & 0x7E;
    data = data / 2;
    return data;
}

short int MPU_Get_Y_Gyro_Offset_TC(char addr)
{
    char data = I2C_Read_Reg(addr, MPU6050_RA_YG_OFFS_TC);
    data = data & 0x7E;
    data = data / 2;
    return data;
}

short int MPU_Get_Z_Gyro_Offset_TC(char addr)
{
    char data = I2C_Read_Reg(addr, MPU6050_RA_ZG_OFFS_TC);
    data = data & 0x7E;
    data = data / 2;
    return data;
}

void MPU_Set_Slave_Address(char addr, char num, char address)
{
    GET_REG(addr, MPU6050_RA_I2C_SLV0_ADDR);
    if (num > 3)
        return;
    I2C_Write_Reg(addr, MPU6050_RA_I2C_SLV0_ADDR + num * 3, address);
    GET_REG(addr, MPU6050_RA_I2C_SLV0_ADDR);
}

void MPU_I2C_Set_Master_Mode_Enable(char addr, char value)
{
    char data;
    GET_REG(addr, MPU6050_RA_USER_CTRL);
    data = I2C_Read_Reg(addr, MPU6050_RA_USER_CTRL);
    /* CLEAN BIT I2C_MST_EN*/
    data = data & 0xDF;
    /* SET BIT I2C_MST_EN*/
    data = data | value << 5;

    /* WRITE VALUE ON REGISTER */
    I2C_Write_Reg(addr, MPU6050_RA_USER_CTRL, data);
    GET_REG(addr, MPU6050_RA_USER_CTRL);
}

void MPU_Reset_I2C_Master(char addr)
{
    char data;
    GET_REG(addr, MPU6050_RA_USER_CTRL);
    data = I2C_Read_Reg(addr, MPU6050_RA_USER_CTRL);
    /* CLEAN BIT I2C_MST_EN*/
    data = data & 0xFD;
    /* SET BIT I2C_MST_EN*/
    data = data | 2;

    /* WRITE VALUE ON REGISTER */
    I2C_Write_Reg(addr, MPU6050_RA_USER_CTRL, data);
    GET_REG(addr, MPU6050_RA_USER_CTRL);
}

void MPU_Set_Memory_Start_Address(char addr, char value)
{
    char data;
    /* WRITE VALUE ON REGISTER */
    I2C_Write_Reg(addr, MPU6050_RA_MEM_START_ADDR, value);
}

char MPU_Write_Prog_DMP_Configuration_Set(char addr, const unsigned short int * data, unsigned int dataSize)
{
    return MPU_Write_DMP_Configuration_Set(addr, data, dataSize);
}

char MPU_Write_DMP_Configuration_Set(char addr, const unsigned char * data, unsigned int dataSize)
{
    unsigned char i, j;
    unsigned char Bank = 0, Offset = 0, Length = 0, Byte;


    for (i = 0; i < dataSize;) {
        /* LÊ BANK */
        Bank = data[i];
        i++;

        /* LÊ O PRIMEIRO ENDEREÇO DE GRAVAÇÃO*/
        Offset = data[i];
        i++;

        /* LÊ O NUMERO DE BYTES A SEREM GRAVADOS*/
        Length = data[i];
        i++;

        /* ZERA A VARIÁVEL DE CONTROLE DO NUMERO DE BYTES ATUAIS*/
        j = 0;

        /* SETA O BANK E O ENDEREÇO INICIAL DE ESCRITA (OFFSET)*/
        MPU_Set_Memory_Bank(addr, Bank, _FALSE_, _FALSE_);
        MPU_Set_Memory_Start_Address(addr, Offset);
        /* */


        /* GRAVA OS VALORES NOS REGISTRADORES */
        while (j < Length) {
            Byte = data[i + j];
            I2C_Write_Reg(addr, MPU6050_RA_MEM_R_W, Byte);
            j++;
        }


        /* */


        /* ZERA A VARIÁVEL DE CONTROLE DO NUMERO DE BYTES */
        j = 0;

        /* VERIFICA SE OS BYTES FORAM GRAVADOS ADEQUADAMENTE COMPARANDO O VALOR LIDO NO REG COM O VALOR QUE DEVERIA SER ESCRITO*/
        while (j < Length) {
            /* SELECIONA O OFFSET */
            MPU_Set_Memory_Start_Address(addr, Offset + j);
            /* */

            /* LÊ O BYTE */
            Byte = I2C_Read_Reg(addr, MPU6050_RA_MEM_R_W);
            /* */

            /* COMPARA O BYTE LIDO COM O QUE DEVERIA SER ESCRITO*/
            if (Byte != data[i + j])
                return 0;
            /* */
            j++;
        }
        /* */

        /* PULA PARA O PRÓXIMO BANCO*/
        i = i + j;
        if (Length == 0)
            i++;


    }
    return 1;
}

char MPU_Write_Prog_Memory_Block(char addr, const char * data, unsigned int dataSize)
{
    return MPU_Write_Memory_Block(addr, &data, dataSize);
}

char MPU_Write_Memory_Block(char addr, const char * data, unsigned int dataSize)
{
    char buffer[50];
    char bank = 0;
    char address = 0;
    unsigned int a, i, j = 0, z = 0;
    char byte;

    char FWBlock_Original[256];
    char FWBlock_Lido[256];

    ClearAnyBuffer(FWBlock_Lido, 256);
    ClearAnyBuffer(FWBlock_Original, 256);


    for (i = 0; i <= dataSize; i++) {
        /* ARMAZENA O BANK*/
        byte = data[i];
        FWBlock_Original[j] = byte;
        j++;

        /* ARMAZENA O BANK*/
        if ((z == 0 & i - z >= 255) || (z > 0 & i - z > 255) || (i == dataSize - 1)) {
            /* SELECIONA O BANK E O PRIMEIRO ENDEREÇO DE ESCRITA*/
            MPU_Set_Memory_Bank(addr, bank, _FALSE_, _FALSE_);

            /* */

            /* CARREGA O BANK ATUAL NO VETOR FWBLOCK_ORIGINAL E GRAVA NA MEMÓRIA*/
            for (a = 0; a < j; a++) {
                MPU_Set_Memory_Start_Address(addr, a);
                I2C_Write_Reg(addr, MPU6050_RA_MEM_R_W, FWBlock_Original[a]);
            }
            /* */

            /* SELECIONA O BANK ATUAL */
            MPU_Set_Memory_Bank(addr, bank, _FALSE_, _FALSE_);

            /* LÊ O BANK GRAVADO NA MEMÓRIA DO DMP E ARMAZENA NO VETOR FWBLOCK_LIDO*/



            for (a = 0; a < j; a++) {
                MPU_Set_Memory_Start_Address(addr, a);
                FWBlock_Lido[a] = I2C_Read_Reg(addr, MPU6050_RA_MEM_R_W);

                /* SE UM ÚNICO BYTE LIDO DA MEMÓRIA FOR DIFERENTE DO BYTE DO FW DO DMP, DÁ ERRO NA GRAVAÇÃO*/
                /*if (FWBlock_Lido[a] != FWBlock_Original[a])
                    return 0;*/
                /* */
            }
            /* */

            bank++;
            z = i;
            j = 0;

        }
    }
    return 1;
}

void MPU_Set_Int_Enabled(char addr, unsigned char enabled)
{
    GET_REG(addr, MPU6050_RA_INT_ENABLE);
    I2C_Write_Reg(addr, MPU6050_RA_INT_ENABLE, enabled);
    GET_REG(addr, MPU6050_RA_INT_ENABLE);
}

void MPU_Set_Rate(char addr, unsigned char value)
{
    GET_REG(addr, MPU6050_RA_SMPLRT_DIV);
    I2C_Write_Reg(addr, MPU6050_RA_SMPLRT_DIV, value);
    GET_REG(addr, MPU6050_RA_SMPLRT_DIV);
}

void MPU_Set_External_Frame_Sync(char addr, unsigned char value)
{
    char data;
    GET_REG(addr, MPU6050_RA_CONFIG);
    data = I2C_Read_Reg(addr, MPU6050_RA_CONFIG);
    /* CLEAR EXT_SYNC_SET[2:0] */
    data = data & 0xC7;
    /* SET EXT_SYNC_SET[2:0]*/
    data = data | value << 3;

    /* WRITE VALUE ON REGISTER */
    I2C_Write_Reg(addr, MPU6050_RA_CONFIG, data);
    GET_REG(addr, MPU6050_RA_CONFIG);
}

void MPU_Set_DLPF_Mode(char addr, unsigned char value)
{
    char data;
    GET_REG(addr, MPU6050_RA_CONFIG);
    data = I2C_Read_Reg(addr, MPU6050_RA_CONFIG);
    /* CLEAN DLPF_CFG[2:0]*/
    data = data & 0xF8;
    /* SET DLPF_CFG[2:0]*/
    data = data | value;

    /* WRITE VALUE ON REGISTER */
    I2C_Write_Reg(addr, MPU6050_RA_CONFIG, data);
    GET_REG(addr, MPU6050_RA_CONFIG);
}

void MPU_Set_DMP_Config1(char addr, char value)
{
    GET_REG(addr, MPU6050_RA_DMP_CFG_1);
    I2C_Write_Reg(addr, MPU6050_RA_DMP_CFG_1, value);
    GET_REG(addr, MPU6050_RA_DMP_CFG_1);
}

void MPU_Set_DMP_Config2(char addr, char value)
{
    GET_REG(addr, MPU6050_RA_DMP_CFG_2);
    I2C_Write_Reg(addr, MPU6050_RA_DMP_CFG_2, value);
    GET_REG(addr, MPU6050_RA_DMP_CFG_2);
}

void MPU_Set_OTP_Bank_Valid(char addr, char value)
{
    char data;
    GET_REG(addr, MPU6050_RA_XG_OFFS_TC);
    data = I2C_Read_Reg(addr, MPU6050_RA_XG_OFFS_TC);
    /* CLEAN OTP BNK VLD BIT*/
    data = data & 0xFE;
    /* SET OTP BNK VLD BIT]*/
    data = data | value;

    /* WRITE VALUE ON REGISTER */
    I2C_Write_Reg(addr, MPU6050_RA_XG_OFFS_TC, data);
    GET_REG(addr, MPU6050_RA_XG_OFFS_TC);
}

void MPU_Set_X_Gyro_Offset_TC(char addr, short int value)
{
    char data;
    GET_REG(addr, MPU6050_RA_XG_OFFS_TC);
    data = I2C_Read_Reg(addr, MPU6050_RA_XG_OFFS_TC);
    /* CLEAN OFFSET BITS*/
    data = data & 0xC0;
    /* SET OFFSET BITS */
    data = data | value;

    /* WRITE VALUE ON REGISTER */
    I2C_Write_Reg(addr, MPU6050_RA_XG_OFFS_TC, data);
    GET_REG(addr, MPU6050_RA_XG_OFFS_TC);
}

void MPU_Set_Y_Gyro_Offset_TC(char addr, short int value)
{
    char data;
    GET_REG(addr, MPU6050_RA_YG_OFFS_TC);
    data = I2C_Read_Reg(addr, MPU6050_RA_YG_OFFS_TC);
    /* CLEAN OFFSET BITS*/
    data = data & 0xC0;
    /* SET OFFSET BITS */
    data = data | value;

    /* WRITE VALUE ON REGISTER */
    I2C_Write_Reg(addr, MPU6050_RA_YG_OFFS_TC, data);
    GET_REG(addr, MPU6050_RA_YG_OFFS_TC);
}

void MPU_Set_Z_Gyro_Offset_TC(char addr, short int value)
{
    char data;
    GET_REG(addr, MPU6050_RA_ZG_OFFS_TC);
    data = I2C_Read_Reg(addr, MPU6050_RA_ZG_OFFS_TC);
    /* CLEAN OFFSET BITS*/
    data = data & 0xC0;
    /* SET OFFSET BITS */
    data = data | value;

    /* WRITE VALUE ON REGISTER */
    I2C_Write_Reg(addr, MPU6050_RA_ZG_OFFS_TC, data);
    GET_REG(addr, MPU6050_RA_ZG_OFFS_TC);
}

void MPU_Reset_Fifo(char addr)
{
    char data;
    GET_REG(addr, MPU6050_RA_USER_CTRL);
    data = I2C_Read_Reg(addr, MPU6050_RA_USER_CTRL);
    /* CLEAN FIFO RESET BIT*/
    data = data & 0xFB;
    /* SET FIFO RESET BIT */
    data = data | 1 << 2;

    /* WRITE VALUE ON REGISTER */
    I2C_Write_Reg(addr, MPU6050_RA_USER_CTRL, data);
    GET_REG(addr, MPU6050_RA_USER_CTRL);
}

unsigned int MPU_Get_FIFO_Count(char addr)
{
    return I2C_Read_Reg(addr, MPU6050_RA_FIFO_COUNTH) << 8 | I2C_Read_Reg(addr, MPU6050_RA_FIFO_COUNTL);
}

void MPU_Get_Fifo_Bytes(char addr, char * data, char length)
{
    char i = 0;
    if (length > 0) {
        while (length > 0) {
            data[i] = I2C_Read_Reg(addr, MPU6050_RA_FIFO_R_W);
            i++;
            length--;
        }
    }
}

void MPU_Set_Motion_Detection_Threshold(char addr, char value)
{
    GET_REG(addr, MPU6050_RA_MOT_THR);
    I2C_Write_Reg(addr, MPU6050_RA_MOT_THR, value);
    GET_REG(addr, MPU6050_RA_MOT_THR);
}

void MPU_Set_Zero_Motion_Detection_Threshold(char addr, char value)
{
    GET_REG(addr, MPU6050_RA_ZRMOT_THR);
    I2C_Write_Reg(addr, MPU6050_RA_ZRMOT_THR, value);
    GET_REG(addr, MPU6050_RA_ZRMOT_THR);
}

void MPU_Set_Motion_Detection_Duration(char addr, char value)
{
    GET_REG(addr, MPU6050_RA_MOT_DUR);
    I2C_Write_Reg(addr, MPU6050_RA_MOT_DUR, value);
    GET_REG(addr, MPU6050_RA_MOT_DUR);
}

void MPU_Set_Zero_Motion_Detection_Duration(char addr, char value)
{
    GET_REG(addr, MPU6050_RA_ZRMOT_DUR);
    I2C_Write_Reg(addr, MPU6050_RA_ZRMOT_DUR, value);
    GET_REG(addr, MPU6050_RA_ZRMOT_DUR);
}

void MPU_Reset_FIFO(char addr)
{
    GET_REG(addr, MPU6050_RA_USER_CTRL);
    char data = I2C_Read_Reg(addr, MPU6050_RA_USER_CTRL);
    data |= 1 << 2;
    I2C_Write_Reg(mpuAddr, MPU6050_RA_USER_CTRL, data);
    GET_REG(addr, MPU6050_RA_USER_CTRL);
}

void MPU_Set_FIFO_Enabled(char addr, char value)
{
    GET_REG(addr, MPU6050_RA_USER_CTRL);
    char data = I2C_Read_Reg(addr, MPU6050_RA_USER_CTRL);
    data &= ~1 << 6;
    data |= value << 6;
    I2C_Write_Reg(mpuAddr, MPU6050_RA_USER_CTRL, data);
    GET_REG(addr, MPU6050_RA_USER_CTRL);
}

void MPU_Set_DMP_Enabled(char addr, char value)
{
    GET_REG(addr, MPU6050_RA_USER_CTRL);
    char data = I2C_Read_Reg(addr, MPU6050_RA_USER_CTRL);
    data &= 0x7F;
    data |= value << 7;
    I2C_Write_Reg(addr, MPU6050_RA_USER_CTRL, data);
    GET_REG(addr, MPU6050_RA_USER_CTRL);
}

void MPU_Reset_DMP(char addr)
{
    GET_REG(addr, MPU6050_RA_USER_CTRL);
    char data = I2C_Read_Reg(addr, MPU6050_RA_USER_CTRL);
    data |= 1 << 3;
    I2C_Write_Reg(addr, MPU6050_RA_USER_CTRL, data);
    GET_REG(addr, MPU6050_RA_USER_CTRL);
}

char MPU_Get_Int_Status(char addr)
{
    return I2C_Read_Reg(addr, MPU6050_RA_INT_STATUS);
}

void MPU_Write_Package(char * package, char length)
{
    char buffer[50];
    Debug.Send("Package: \r");
    for (char i = 0; i < length; i++) {
        sprintf(buffer, "0x%2X ", package[i]);
        Debug.Send(buffer);
        if ((i + 1) % 7 == 0)
            Debug.Send("\r");
    }
    Debug.Send("\r");
}

void MPU_Set_X_Gyro_Offset(int value, char addr)
{
    char buffer[50];
    _FORMAT(buffer, "Set Gyro X Offset to %d\r", value);
    _DEBUG(buffer);

    I2C_Write_Reg(addr, MPU6050_RA_XG_OFFS_USRH, value / 256);
    I2C_Write_Reg(addr, MPU6050_RA_XG_OFFS_USRL, value);
}

void MPU_Set_Y_Gyro_Offset(int value, char addr)
{
    char buffer[50];
    _FORMAT(buffer, "Set Gyro Y Offset to %d\r", value);
    _DEBUG(buffer);

    I2C_Write_Reg(addr, MPU6050_RA_YG_OFFS_USRH, value / 256);
    I2C_Write_Reg(addr, MPU6050_RA_YG_OFFS_USRL, value);
}

void MPU_Set_Z_Gyro_Offset(int value, char addr)
{
    char buffer[50];
    _FORMAT(buffer, "Set Gyro Z Offset to %d\r", value);
    _DEBUG(buffer);


    I2C_Write_Reg(addr, MPU6050_RA_ZG_OFFS_USRH, value / 256);
    I2C_Write_Reg(addr, MPU6050_RA_ZG_OFFS_USRL, value);
}

void MPU_Set_Z_Accel_Offset(int value, char addr)
{
    char buffer[50];
    _FORMAT(buffer, "Set Accel Z Offset to %d\r", value);
    _DEBUG(buffer);

    I2C_Write_Reg(addr, MPU6050_RA_ZA_OFFS_H, value / 256);
    I2C_Write_Reg(addr, MPU6050_RA_ZA_OFFS_L_TC, value);
}

int MPU_Read_Accel_X(char addr)
{
    char high = I2C_Read_Reg(addr, MPU6050_RA_ACCEL_XOUT_H);
    char low = I2C_Read_Reg(addr, MPU6050_RA_ACCEL_XOUT_L);
    return high << 8 | low;
}

int MPU_Read_Accel_Y(char addr)
{
    char high = I2C_Read_Reg(addr, MPU6050_RA_ACCEL_YOUT_H);
    char low = I2C_Read_Reg(addr, MPU6050_RA_ACCEL_YOUT_L);
    return high << 8 | low;
}

int MPU_Read_Accel_Z(char addr)
{
    char high = I2C_Read_Reg(addr, MPU6050_RA_ACCEL_ZOUT_H);
    char low = I2C_Read_Reg(addr, MPU6050_RA_ACCEL_ZOUT_L);
    return high << 8 | low;
}

int MPU_Read_Gyro_X(char addr)
{
    char high = I2C_Read_Reg(addr, MPU6050_RA_GYRO_XOUT_H);
    char low = I2C_Read_Reg(addr, MPU6050_RA_GYRO_XOUT_L);
    return high << 8 | low;
}

int MPU_Read_Gyro_Y(char addr)
{
    char high = I2C_Read_Reg(addr, MPU6050_RA_GYRO_YOUT_H);
    char low = I2C_Read_Reg(addr, MPU6050_RA_GYRO_YOUT_L);
    return high << 8 | low;
}

int MPU_Read_Gyro_Z(char addr)
{
    char high = I2C_Read_Reg(addr, MPU6050_RA_GYRO_ZOUT_H);
    char low = I2C_Read_Reg(addr, MPU6050_RA_GYRO_ZOUT_L);
    return high << 8 | low;
}

void MPU_Calibrate()
{
    long int value;
    unsigned int counter;
    char buffer[50];


    Debug.Send("MPU Calibrate\r");

    MPU_CH = 0;

    //__delay_ms(500);

    MPU_CH = 1;

    //__delay_ms(500);

    /* POWER SETUP */

    I2C_Write_Reg(mpuAddr, MPU6050_RA_PWR_MGMT_1, 2);
    I2C_Write_Reg(mpuAddr, MPU6050_RA_PWR_MGMT_2, 0);

    /* */

    /* SAMPLE RATE SET SAMPLE RATE = GYROSCOPE OUTPUT RATE/ (1+SMPLRT_DIV) */
    I2C_Write_Reg(mpuAddr, MPU6050_RA_SMPLRT_DIV, 0);

    /* CONFIGURATION SET DIGITAL LOW PASS FILTER + FRAME SYNCHRONIZATION */
    /* 0 => 260 and 256 HZ OF BANDWIDTH FOR ACCEL AND GYRO*/
    I2C_Write_Reg(mpuAddr, MPU6050_RA_CONFIG, 0);

    /* GYRO CONFIG +- 250°/s*/
    //I2C_Write_Reg(mpuAddr, MPU6050_RA_GYRO_CONFIG, 0);

    /* ACCEL CONFIG +- 2G*/
    //I2C_Write_Reg(mpuAddr, MPU6050_RA_ACCEL_CONFIG, 0);

    MPU_Set_Full_Scale_Gyro_Range(mpuAddr, MPU6050_GYRO_FS_2000);
    MPU_Set_Full_Scale_Accel_Range(mpuAddr, MPU6050_ACCEL_FS_2);

    _DEBUG("MPU Calibrating process... \r");
    _DEBUG("Get Bias for Accel X...\r");

    /* */
    for (counter = 0, value = 0; counter < 1000; counter++) {
        value += MPU_Read_Accel_X(mpuAddr);
    }
    value = value / 1000;
    BIAS_MPU.Ax = value;

    _FORMAT(buffer, "Bias for AccelX: %d \r", BIAS_MPU.Ax);
    _DEBUG(buffer);
    /**/

    /* */
    for (counter = 0, value = 0; counter < 1000; counter++) {
        value += MPU_Read_Accel_Y(mpuAddr);
    }
    value = value / 1000;
    BIAS_MPU.Ay = value;

    _FORMAT(buffer, "Bias for AccelY %d \r", BIAS_MPU.Ay);
    _DEBUG(buffer);
    /**/

    /* */
    for (counter = 0, value = 0; counter < 1000; counter++) {
        value += MPU_Read_Accel_Z(mpuAddr);
    }
    value = value / 1000;
    BIAS_MPU.Az = 16384 + value;

    _FORMAT(buffer, "Bias for AccelZ: %d \r", BIAS_MPU.Az);
    _DEBUG(buffer);
    /**/

    /* */
    for (counter = 0, value = 0; counter < 1000; counter++) {
        value += MPU_Read_Gyro_X(mpuAddr);
    }
    value = value / 1000;
    BIAS_MPU.Gx = value;

    _FORMAT(buffer, "Bias for GyroX: %d \r", BIAS_MPU.Gx);
    _DEBUG(buffer);
    /**/

    /* */
    for (counter = 0, value = 0; counter < 1000; counter++) {
        value += MPU_Read_Gyro_Y(mpuAddr);
    }
    value = value / 1000;
    BIAS_MPU.Gy = value;

    _FORMAT(buffer, "Bias for GyroY: %d \r", BIAS_MPU.Gy);
    _DEBUG(buffer);
    /**/

    /* */
    for (counter = 0, value = 0; counter < 1000; counter++) {
        value += MPU_Read_Gyro_Z(mpuAddr);
    }
    value = value / 1000;
    BIAS_MPU.Gz = value;

    _FORMAT(buffer, "Bias for GyroZ: %d \r", BIAS_MPU.Gz);
    _DEBUG(buffer);
    /**/

    EEPROM_Put_Object(&BIAS_MPU, sizeof (MPU_BIAS), ADDR_MPU);

    reset();
}


