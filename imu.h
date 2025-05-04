#ifndef IMU_H
#define IMU_H

#include <stdint.h>

typedef struct
{
    int16_t acc_x_raw;
    int16_t acc_y_raw;
    int16_t acc_z_raw;
    int16_t gyro_x_raw;
    int16_t gyro_y_raw;
    int16_t gyro_z_raw;
    float acc_x;
    float acc_y;
    float acc_z;
    float gyro_x;
    float gyro_y;
    float gyro_z;
} IMU_Data;

extern IMU_Data imu_data;

void initIMU(void);
void readIMU_AllRaw(void);

#endif
