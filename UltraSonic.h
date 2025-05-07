#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include "main.h"
#define ULTRASONIC_TRIG_PIN    GPIO_PIN_9
#define ULTRASONIC_TRIG_PORT   GPIOA
#define ULTRASONIC_ECHO_PIN    GPIO_PIN_8
#define ULTRASONIC_ECHO_PORT   GPIOA

/**
 * @brief Initializes the ultrasonic sensor (starts the timer).
 */
void ultrasonic_init(void);

/**
 * @brief Delays for a specified number of microseconds.
 * @param us Number of microseconds to delay.
 */
void ultrasonic_delay_us(uint16_t us);

/**
 * @brief Measures distance using the ultrasonic sensor.
 * @return Distance in centimeters.
 */
float ultrasonic_read_distance(void);

#endif // ULTRASONIC_H
