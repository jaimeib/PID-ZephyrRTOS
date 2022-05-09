
#include <stdio.h>
#include <posix/unistd.h>

// HAL API
#include <stm32f7xx_hal.h>
#include <stm32f7xx_hal_adc.h>
#include "HAL_ADC.h"

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
		sleep(2);
	}

	return 0;
}
