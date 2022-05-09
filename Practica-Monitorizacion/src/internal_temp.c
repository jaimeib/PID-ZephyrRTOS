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
#include "internal_temp.h"
#include "HAL_ADC.h"

void internal_temp(void *ptr_result)
{
	extern pthread_mutex_t mutex_result;
	extern pthread_cond_t cond_result;
	extern bool new_result_for_supervisor;
	extern bool new_result_for_publisher;

	printf("Internal temperature thread started\n");

	/* Variable used to get converted value */
	__IO int32_t ConvertedValue = 0;
	int32_t JTemp = 0x0;

	// Habilitar el reloj ADC1
	SystemClock_Config();

	/* Configure the ADC peripheral */
	ADC_Config();

	// Infinite loop
	while (true) {
		// Select ADC_CHTemp
		ADC_Select_CHTemp();
		HAL_ADC_Start(&AdcHandle);
		HAL_ADC_PollForConversion(&AdcHandle, 1000);
		ConvertedValue = HAL_ADC_GetValue(&AdcHandle);
		HAL_ADC_Stop(&AdcHandle);

		// Compute the Junction Temperature value in degreeC
		JTemp = ((((ConvertedValue * VREF) / MAX_CONVERTED_VALUE) - VSENS_AT_AMBIENT_TEMP) *
			 10 / AVG_SLOPE) +
			AMBIENT_TEMP;

		// Print the value
		printf("Internal Temperature is %d degrees \n", JTemp);

		// Send the temperature to the supervisor thread
		pthread_mutex_lock(&mutex_result);
		((thread_result_t *)ptr_result)->type = INTERNAL_TEMPERATURE;
		((thread_result_t *)ptr_result)->value = JTemp;

		// Notify the supervisor & publisher threads
		new_result_for_supervisor = true;
		new_result_for_publisher = true;
		pthread_cond_broadcast(&cond_result);
		pthread_mutex_unlock(&mutex_result);

		//Wait for period (next activation):
		k_msleep(INTERNAL_TEMP_SENSOR_PERIOD_MS);
	}
}
