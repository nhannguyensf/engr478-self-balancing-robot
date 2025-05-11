// self_balance.c - Core control loop for self-balancing robot

#include "self_balance.h"
#include "imu.h"
#include "motor.h"
#include "systick_timer.h"
#include <math.h>

// PID constants - adjust for tuning robot stability
static float Kp = 0.8f;
static float Ki = 0.0f;
static float Kd = 0.0f;

// PID control variables
static float prev_error = 0.0f;
static float integral = 0.0f;

// #define STABLE_ZONE_ANGLE 80.0f     // Degrees, robot doesn't react within this range
// #define FALL_THRESHOLD_ANGLE 150.0f // Degrees, robot stops if pitch exceeds this value

// Main balance control loop, called from SysTick interrupt
void balanceLoop(void)
{
    float error, derivative, output;
    float pitch; // Estimated tilt angle (degrees)

    readIMU_AllRaw(); // Read current IMU sensor values

    // Estimate pitch using a simple complementary filter
    float acc_angle = atan2f(imu_data.acc_y, imu_data.acc_z) * 180.0f / 3.14159265f;
    static float angle = 0.0f;
    angle = 0.70f * (angle + imu_data.gyro_x * 0.01f) + 0.30f * acc_angle;
    pitch = angle;

    // PID control calculations
    error = pitch;
    integral += error * 0.01f;                             // Integrate over time
    derivative = (error - prev_error) / 0.01f;             // Rate of change of error
    output = Kp * error + Ki * integral + Kd * derivative; // PID output
    prev_error = error;

    // Control motors based on PID output
    float pwm = output;
    driveMotor(MOTOR_LEFT, -pwm);
    driveMotor(MOTOR_RIGHT, -pwm);
}
