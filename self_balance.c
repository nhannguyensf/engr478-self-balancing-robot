#include "self_balance.h"
#include "imu.h"
#include "motor.h"
#include "systick_timer.h"
#include <math.h>

// PID Constants (Adjust These for Stability)
static float Kp = 4.0f;
static float Ki = 0.02f;
static float Kd = 0.8f;

// PID State Variables
static float prev_error = 0.0f;
static float integral = 0.0f;

// Control Loop Time Step
#define DT 0.005f

// Gyro Bias Variable (Calculated Once at Startup)
static float gyro_x_bias = 0.0f;

// ------------------------------------------------------------
// Calibrate Gyroscope Bias (Call This After initIMU())
// ------------------------------------------------------------
void calibrateGyro(void)
{
    int samples = 500;
    float sum = 0.0f;

    int i;
    for (i = 0; i < samples; i++)
    {
        readIMU_AllRaw();
        sum += imu_data.gyro_x;
        // delay_ms(2);
    }

    gyro_x_bias = sum / samples;
}

// ------------------------------------------------------------
// Balance Control Loop (Called at 200 Hz from TIM6 ISR)
// ------------------------------------------------------------
void balanceLoop(void)
{
    float error, derivative, output;
    float pitch;

    // 1. Read IMU Data
    readIMU_AllRaw();

    // 2. Estimate Pitch Angle Using Complementary Filter
    float acc_angle = atan2f(imu_data.acc_y, imu_data.acc_z) * (180.0f / 3.14159265f);
    static float angle = 0.0f;

    // Compensate Gyro Bias and Apply Complementary Filter
    float gyro_x_corrected = imu_data.gyro_x - gyro_x_bias;
    angle = 0.98f * (angle + gyro_x_corrected * DT) + 0.02f * acc_angle;
    pitch = angle;

    // 3. PID Calculations
    error = pitch; // Target pitch is 0 degrees (upright)
    integral += error * DT;
    derivative = (error - prev_error) / DT;
    output = Kp * error + Ki * integral + Kd * derivative;
    prev_error = error;

    // 4. Limit Output to Motor Control Range (-1000 to 1000)
    if (output > 1000)
        output = 1000;
    if (output < -1000)
        output = -1000;

    // 5. Control Motors to Correct Balance
    driveMotorLeft(-output);
    driveMotorRight(-output);
}
