// imu.h - Header file for IMU driver
#ifndef IMU_H
#define IMU_H

#include "stm32l476xx.h"

void initIMU(void);
void calibrateGyro(void);
float getTiltAngle(void);

#endif
