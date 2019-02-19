#include "mpu6050.h"

#define MPU6050_DMP_CODE_SIZE       1929    // dmpMemory[]
#define MPU6050_DMP_CONFIG_SIZE     192     // dmpConfig[]
#define MPU6050_DMP_UPDATES_SIZE    47      // dmpUpdates[]

#define PROGMEM

#define NUMBER_OF_BANKS 8

#define _DMP_RECORD_ERROR_ 99
#define _DMP_CONFIG_ERROR_ 100
#define _DMP_UPDATE_ERROR_ 101
#define _DMP_OK_ 1

short int DMP_Init(MPU * headSensor);
void DMP_Get_Quartenion(int * quartenion, const unsigned char * packet);

