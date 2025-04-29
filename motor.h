// motor.h - Header for Basic Motor PWM Control
#ifndef MOTOR_H
#define MOTOR_H

#define MOTOR_LEFT 0
#define MOTOR_RIGHT 1

void initMotors(void);
void driveMotor(int side, int speed);
void forward(int speed);
void backward(int speed);
void turnLeft(int speed);
void turnRight(int speed);
void motorTest(void);

#endif
