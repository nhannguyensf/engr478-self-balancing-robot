// self_balance.c
#include "self_balance.h"
#include "imu.h"
#include "motor.h"
#include "systick_timer.h"
#include <math.h>

#ifndef M_PI
/**
 * @brief Pi constant if not defined in math.h
 */
#define M_PI 3.14159265358979323846f
#endif

// PID Constants (externally tunable)
float Kp = 4.5f;
float Ki = 0.015f;
float Kd = 1.0f;

// PID State Variables (internal)
static float prev_error = 0.0f;
static float integral = 0.0f;

// Sampling interval
#define DT 0.005f

// Gyro bias (internal)
static float gyro_x_bias = 0.0f;

// Upright angle offset (externally tunable)
float pitch_offset = 2.0f;

void calibrateGyro(void)
{
    int samples = 500;
    float sum = 0.0f;
    int i;
    for (i = 0; i < samples; i++)
    {
        readIMU_AllRaw();
        sum += imu_data.gyro_x;
    }
    gyro_x_bias = sum / samples;
}

void balanceLoop(void)
{
    float error, derivative, output;
    float pitch;

    // 1) Read IMU
    readIMU_AllRaw();

    // 2) Complementary filter for pitch angle
    float acc_angle = atan2f(imu_data.acc_y, imu_data.acc_z) * (180.0f / M_PI);
    static float angle = 0.0f;
    float gyro_corr = imu_data.gyro_x - gyro_x_bias;
    angle = 0.98f * (angle + gyro_corr * DT) + 0.02f * acc_angle;
    pitch = angle;

    // 3) PID with externally set Kp, Ki, Kd and offset
    error = pitch - pitch_offset;
    integral += error * DT;
    derivative = (error - prev_error) / DT;
    output = Kp * error + Ki * integral + Kd * derivative;
    prev_error = error;

    // 4) Limit output
    if (output > 1000)
        output = 1000;
    if (output < -1000)
        output = -1000;

    // 5) Drive motors
    driveMotorLeft(-output);
    driveMotorRight(-output);
}
