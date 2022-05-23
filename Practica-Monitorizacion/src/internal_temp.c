// Standard C + POSIX API
#include <stdio.h>
#include <posix/time.h>
#include <posix/unistd.h>
#include <posix/pthread.h>

// Hal API
#include <stm32f7xx_hal.h>
#include <stm32f7xx_hal_adc.h>

// Modules
#include "main.h"
#include "internal_temp.h"

/* ADC handler declaration */
static ADC_HandleTypeDef AdcHandle;

// ADC FUNCTIONS:
static void ADC_Config(void);
static void Error_Handler(void);
static void SystemClock_Config(void);
static void ADC_Select_CHTemp(void);

static void SystemClock_Config(void)
{
	__HAL_RCC_ADC1_CLK_ENABLE();
}

static void ADC_Select_CHTemp(void)
{
	ADC_ChannelConfTypeDef sConfig = { 0 };

	/* Configure ADC Temperature Sensor Channel */
	sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_56CYCLES;
	sConfig.Offset = 0;

	if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK) {
		/* Channel Configuration Error */
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

		// Sleep 20 ms
		HAL_Delay(20);
	}
}

void internal_temp(void *ptr_result)
{
	extern pthread_mutex_t mutex_result;
	extern pthread_cond_t cond_result;
	extern bool new_result_for_supervisor;
	extern bool new_result_for_publisher;

	printf("Internal temperature thread started\n");

	/* Variable used to get converted value */
	__IO int32_t ConvertedValue = 0;
	double Temp = 0.0;

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

		// Compute the Temperature value in degreeC
		Temp = (((((double)ConvertedValue * VREF) / MAX_CONVERTED_VALUE) -
			 VSENS_AT_AMBIENT_TEMP) *
			10 / AVG_SLOPE) +
		       AMBIENT_TEMP;

		// Print the value
		printf("Internal Temperature is %f degrees \n", Temp);

		// Send the temperature to the supervisor thread
		pthread_mutex_lock(&mutex_result);
		((thread_result_t *)ptr_result)->type = INTERNAL_TEMPERATURE;
		((thread_result_t *)ptr_result)->value = Temp;

		// Notify the supervisor & publisher threads
		new_result_for_supervisor = true;
		new_result_for_publisher = true;
		pthread_cond_broadcast(&cond_result);
		pthread_mutex_unlock(&mutex_result);

		// Wait for period (next activation):
		k_msleep(INTERNAL_TEMP_SENSOR_PERIOD_MS);
	}
}
