// self_balance.c - Core control loop for self-balancing robot

#include "self_balance.h"
#include "imu.h"
#include "motor.h"
#include "systick_timer.h"
#include <math.h>

static float Kp = 50.0f;
static float Ki = 0.0f;
static float Kd = 0.0f;

static float prev_error = 0.0f;
static float integral = 0.0f;

void balanceLoop(void)
{
    float error, derivative, output;
    float pitch; // Approximated from accel and gyro

    readIMU_AllRaw();

    // Estimate pitch using complementary filter (simple version)
    float acc_angle = atan2f(imu_data.acc_y, imu_data.acc_z) * 180.0f / 3.14159265f;
    static float angle = 0.0f;
    angle = 0.98f * (angle + imu_data.gyro_x * 0.01f) + 0.02f * acc_angle;
    pitch = angle;

    // PID control
    error = pitch;
    integral += error * 0.01f;
    derivative = (error - prev_error) / 0.01f;
    output = Kp * error + Ki * integral + Kd * derivative;
    prev_error = error;

    // Drive motors
    float pwm = (float)(output);
    driveMotor(MOTOR_LEFT, -pwm);
    driveMotor(MOTOR_RIGHT, -pwm);

    delay(10); // 10ms loop
}
