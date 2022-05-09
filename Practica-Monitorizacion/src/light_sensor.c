// Zephyr API
#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>

// Standard C + POSIX API
#include <stdio.h>
#include <posix/time.h>
#include <posix/unistd.h>
#include <posix/pthread.h>

// Hal API
#include <stm32f7xx_hal.h>
#include <stm32f7xx_hal_adc.h>

//Modules
#include "main.h"
#include "light_sensor.h"
#include "HAL_ADC.h"

void light_sensor(void *ptr_result)
{
	extern pthread_mutex_t mutex_result;
	extern pthread_cond_t cond_result;
	extern bool new_result_for_supervisor;
	extern bool new_result_for_publisher;

	printf("Light sensor thread started\n");

	/* Variable used to get converted value */
	__IO int32_t converted_light = 0;
	int light_normalized = 0;

	SystemClock_Config();

	/* Configure the ADC peripheral*/
	ADC_Config();

	// Hal API
	GPIO_InitTypeDef GPIO_InitStruct;

	// Pin A0 is in PA6, so we need to set it up!
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	// Infinite loop
	while (true) {
		// Select ADC_CH0
		ADC_Select_CH0();
		HAL_ADC_Start(&AdcHandle);
		HAL_ADC_PollForConversion(&AdcHandle, 1000);
		converted_light = HAL_ADC_GetValue(&AdcHandle);
		HAL_ADC_Stop(&AdcHandle);

		// Normalize the light value
		light_normalized = converted_light * MAX_LIGHT_VALUE / MAX_CONVERTED_VALUE;

		// Print the light value
		printf("Light percentage: %d \n", light_normalized);

		// Send the light value to the supervisor thread
		pthread_mutex_lock(&mutex_result);
		((thread_result_t *)ptr_result)->type = LIGHT;
		((thread_result_t *)ptr_result)->value = light_normalized;

		// Notify the supervisor & publisher threads
		new_result_for_supervisor = true;
		new_result_for_publisher = true;
		pthread_cond_broadcast(&cond_result);
		pthread_mutex_unlock(&mutex_result);

		//Wait for period (next activation):
		k_msleep(LIGHT_SENSOR_PERIOD_MS);
	}
}
