// Zephyr API
#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>

//Standard C + POSIX API
#include <stdio.h>
#include <posix/pthread.h>

// Hal API
#include <stm32f7xx_hal.h>
#include <stm32f7xx_hal_adc.h>

//Modules
#include "main.h"
#include "light_sensor.h"

void SystemClock_Config(void)
{
	__HAL_RCC_ADC1_CLK_ENABLE(); // Habilita el reloj ADC1
}

void ADC_Select_CH0(void)
{
	ADC_ChannelConfTypeDef sConfig = { 0 };
	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
	sConfig.Channel = ADC_CHANNEL_6;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_56CYCLES;
	sConfig.Offset = 0;
	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK) {
		Error_Handler();
	}
}

/**
  * @brief  Configure the ADC.
  * @param  None
  * @retval None
  */
static void ADC_Config(void)
{
	/* Configure the ADC peripheral */
	AdcHandle.Instance = ADC1;

	AdcHandle.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV4;
	AdcHandle.Init.Resolution = ADC_RESOLUTION_12B;
	AdcHandle.Init.ScanConvMode =
		ENABLE; /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
	AdcHandle.Init.ContinuousConvMode =
		ENABLE; /* Continuous mode enabled to have continuous conversion */
	AdcHandle.Init.DiscontinuousConvMode =
		DISABLE; /* Parameter discarded because sequencer is disabled */
	AdcHandle.Init.NbrOfDiscConversion = 0;
	AdcHandle.Init.ExternalTrigConvEdge =
		ADC_EXTERNALTRIGCONVEDGE_NONE; /* Conversion start not trigged by an external event */
	AdcHandle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	AdcHandle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	AdcHandle.Init.NbrOfConversion = 1;
	AdcHandle.Init.DMAContinuousRequests = DISABLE;
	AdcHandle.Init.EOCSelection = DISABLE;

	if (HAL_ADC_Init(&AdcHandle) != HAL_OK) {
		/* ADC initialization Error */
		Error_Handler();
	}
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
	while (1) {
		printf("Error...");
		HAL_Delay(20);
	}
}

void light_sensor(void *ptr_result)
{
	extern pthread_mutex_t mutex_result;
	extern pthread_cond_t cond_result;
	extern bool new_result;

	printf("Light sensor thread started\n");

	/* Variable used to get converted value */
	__IO int32_t converted_light = 0;
	int light_normalized = 0;

	SystemClock_Config();

	/*##-2- Configure the ADC peripheral*/
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
		/* Insert a delay define on LIGHT_REFRESH_PERIOD */
		HAL_Delay(LIGHT_REFRESH_PERIOD);

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

		// Notify the supervisor thread
		new_result = true;
		pthread_cond_broadcast(&cond_result);
		pthread_mutex_unlock(&mutex_result);
	}
}
