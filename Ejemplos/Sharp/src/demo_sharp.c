#include <stdio.h>
#include <posix/unistd.h>

// HAL API
#include <stm32f7xx_hal.h>
#include <stm32f7xx_hal_adc.h>

/* ADC handler declaration */
static ADC_HandleTypeDef AdcHandle;

//ADC FUNCTIONS:
static void ADC_Config(void);
static void Error_Handler(void);
static void SystemClock_Config(void);
static void ADC_Select_CH0(void);

static void SystemClock_Config(void)
{
	__HAL_RCC_ADC1_CLK_ENABLE(); // Habilita el reloj ADC1
}

static void ADC_Select_CH0(void)
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

//  x  -> y
// 45  -> 97 mm
// 390 -> 65 mm

// 290  -> 95 mm
// 650  -> 35 mm
// const double a = (45.0 - 390.0) / (97.0 - 65.0);
//const double a = (290.0 - 650.0) / (95.0 - 35.0);
//const double b = 3936;

uint32_t sharp_sensor_dist_mm()
{
	uint32_t measure = 0;
	// Select ADC_CH0
	ADC_Select_CH0();
	HAL_ADC_Start(&AdcHandle);
	HAL_ADC_PollForConversion(&AdcHandle, 1000);
	measure = HAL_ADC_GetValue(&AdcHandle);
	HAL_ADC_Stop(&AdcHandle);
	//uint32_t dist_mm = (uint32_t) (a * measure + b);
	uint32_t dist_mm = ((2914.0 / (measure + 5.0)) - 1.0) * 10;
	printf("adc:%d mm:%d\n", measure, dist_mm);
	return dist_mm;
}

int main(int argc, char **argv)
{
	printf("Demo Sharp\n");
	//uint32_t dist = 0;

	//Configure sharp sensor with HAL API
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

	while (1) {
		sharp_sensor_dist_mm();
		k_msleep(2000);
	}

	return 0;
}
