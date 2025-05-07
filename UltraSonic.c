#include "ultrasonic.h"

extern TIM_HandleTypeDef htim1;  //Use the timer you set up for microsecond timing

void ultrasonic_init(void) {
    HAL_TIM_Base_Start(&htim1);  //Start timer for delay_us()
}

void ultrasonic_delay_us(uint16_t us) {
    __HAL_TIM_SET_COUNTER(&htim1, 0);  //Reset the timer counter
    while (__HAL_TIM_GET_COUNTER(&htim1) < us); // Wait until the timer reaches the required delay
}

float ultrasonic_read_distance(void) {
    uint32_t start = 0, stop = 0;
    uint32_t time_elapsed = 0;

    //Send trigger pulse
    HAL_GPIO_WritePin(ULTRASONIC_TRIG_PORT, ULTRASONIC_TRIG_PIN, GPIO_PIN_RESET);
    ultrasonic_delay_us(2);
    HAL_GPIO_WritePin(ULTRASONIC_TRIG_PORT, ULTRASONIC_TRIG_PIN, GPIO_PIN_SET);
    ultrasonic_delay_us(10);
    HAL_GPIO_WritePin(ULTRASONIC_TRIG_PORT, ULTRASONIC_TRIG_PIN, GPIO_PIN_RESET);

    //Wait for ECHO to go high
    while (HAL_GPIO_ReadPin(ULTRASONIC_ECHO_PORT, ULTRASONIC_ECHO_PIN) == GPIO_PIN_RESET);
    start = __HAL_TIM_GET_COUNTER(&htim1);

    //Wait for ECHO to go low
    while (HAL_GPIO_ReadPin(ULTRASONIC_ECHO_PORT, ULTRASONIC_ECHO_PIN) == GPIO_PIN_SET);
    stop = __HAL_TIM_GET_COUNTER(&htim1);

    //Calculate elapsed time in microseconds
    if (stop >= start)
        time_elapsed = stop - start;
    else
        time_elapsed = (0xFFFF - start + stop);  //Handle counter overflow

    //Convert time to distance (speed of sound = ~34300 cm/s)
    return (float)time_elapsed / 58.0;  //time (us) / 58 = distance (cm)
}
/* //Stuff to include with the main.c
#include "ultrasonic.h"

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_TIM1_Init();

    ultrasonic_init();

    while (1) {
        float distance = ultrasonic_read_distance();
        HAL_Delay(100);  // 100 ms delay between reads
    }


*/