// imu.h - MPU6050 IMU driver header (register-level I2C)
#ifndef IMU_H
#define IMU_H

#include <stdint.h>

extern int16_t Accel_X_RAW, Accel_Y_RAW, Accel_Z_RAW;
extern int16_t Gyro_X_RAW, Gyro_Y_RAW, Gyro_Z_RAW;
extern float Ax, Ay, Az;
extern float Gx, Gy, Gz;

void initIMU(void);
void readAccelRaw(void);
void readGyroRaw(void);

#endif // IMU_H
