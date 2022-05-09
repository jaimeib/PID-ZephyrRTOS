// Hal API
#include <stm32f7xx_hal.h>
#include <stm32f7xx_hal_adc.h>
#include "HAL_ADC.h"

void SystemClock_Config(void)
{
	__HAL_RCC_ADC1_CLK_ENABLE(); // Habilita el reloj ADC1
}

void ADC_Select_CH0(void)
{
	ADC_ChannelConfTypeDef sConfig = { 0 };
	/* Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
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
		//Print error message
		printf("Error...");

		// Sleep 20 ms
		HAL_Delay(20);
	}
}
