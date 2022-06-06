/**
  ******************************************************************************
  * @file    ADC/ADC_TemperatureSensor/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to use the Temperature Sensor to
  *          calculate the junction temperature of the device.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

// Zephyr API
#include <zephyr.h>
#include <drivers/gpio.h>

// Hal API
#include <stm32f7xx_hal.h>
#include <stm32f7xx_hal_adc.h>

#include <stdio.h>


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TEMP_REFRESH_PERIOD   1000    /* Internal temperature refresh period */
#define MAX_CONVERTED_VALUE   4095    /* Max converted value */
#define AMBIENT_TEMP            25    /* Ambient Temperature */
#define VSENS_AT_AMBIENT_TEMP  760    /* VSENSE value (mv) at ambient temperature */
#define AVG_SLOPE               25    /* Avg_Solpe multiply by 10 */
#define VREF                  3300
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* ADC handler declaration */
static ADC_HandleTypeDef    AdcHandle;


/* Private function prototypes -----------------------------------------------*/
static void ADC_Config(void);
static void Error_Handler(void);
static void SystemClock_Config(void);
static void ADC_Select_CH0 (void);
static void ADC_Select_CHTemp (void);

/* Private functions ---------------------------------------------------------*/
void SystemClock_Config(void)
{
  __HAL_RCC_ADC1_CLK_ENABLE();            // Habilita el reloj ADC1

}

void ADC_Select_CH0 (void)
{
  ADC_ChannelConfTypeDef sConfig = {0};
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_6;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_56CYCLES;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

void ADC_Select_CHTemp (void)
{
  
  ADC_ChannelConfTypeDef sConfig = {0};
  
  /* Configure ADC Temperature Sensor Channel */
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_56CYCLES;
  sConfig.Offset = 0;

  if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
  {
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
  AdcHandle.Instance          = ADC1;

  AdcHandle.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV4;
  AdcHandle.Init.Resolution            = ADC_RESOLUTION_12B;
  AdcHandle.Init.ScanConvMode          = ENABLE;                       /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
  AdcHandle.Init.ContinuousConvMode    = ENABLE;                        /* Continuous mode enabled to have continuous conversion */
  AdcHandle.Init.DiscontinuousConvMode = DISABLE;                       /* Parameter discarded because sequencer is disabled */
  AdcHandle.Init.NbrOfDiscConversion   = 0;
  AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;        /* Conversion start not trigged by an external event */
  AdcHandle.Init.ExternalTrigConv      = ADC_SOFTWARE_START;
  AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  AdcHandle.Init.NbrOfConversion       = 1;
  AdcHandle.Init.DMAContinuousRequests = DISABLE;
  AdcHandle.Init.EOCSelection          = DISABLE;

  if (HAL_ADC_Init(&AdcHandle) != HAL_OK)
  {
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
  while (1)
  {
    printf ("Error...");
    HAL_Delay(20);
  }
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  char desc[50];
  /* Variable used to get converted value */
  __IO int32_t ConvertedValue = 0;
  __IO int32_t ConvertedLight = 0;
  int32_t JTemp = 0x0;
  
  SystemClock_Config();

  /*##-2- Configure the ADC peripheral #########################################*/
  ADC_Config();

  /*##-3- Start the conversion process #######################################*/
  //HAL_ADC_Start_DMA(&AdcHandle, (uint32_t*)&ConvertedValue, 1);
  // DMA is not working in zephyr (clock problem?), going for polling
  
  // Hal API
  // Set up a LED
  //__GPIOJ_CLK_ENABLE(); // Not required, init by Zephyr?
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);
  
  // Pin A0 is in PA6, so we need to set it up!
  //~ __GPIOA_CLK_ENABLE();
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* Infinite loop */
  while (1)
  {
    /* Insert a delay define on TEMP_REFRESH_PERIOD */
    HAL_Delay(TEMP_REFRESH_PERIOD);
    
    // Select ADC_CH0
    ADC_Select_CH0();
    HAL_ADC_Start(&AdcHandle);
    HAL_ADC_PollForConversion(&AdcHandle, 1000);
    ConvertedLight = HAL_ADC_GetValue(&AdcHandle);
    HAL_ADC_Stop(&AdcHandle);
        
    printf ("Raw light: %d\n", ConvertedLight);
    
    // Select ADC_CHTemp
    ADC_Select_CHTemp();
    HAL_ADC_Start(&AdcHandle);
    HAL_ADC_PollForConversion(&AdcHandle, 1000);
    ConvertedValue = HAL_ADC_GetValue(&AdcHandle);
    HAL_ADC_Stop(&AdcHandle);

    /* Compute the Junction Temperature value */
    JTemp = ((((ConvertedValue * VREF)/MAX_CONVERTED_VALUE) - VSENS_AT_AMBIENT_TEMP) * 10 / AVG_SLOPE) + AMBIENT_TEMP;
    /* Display the Temperature Value on the LCD */
    sprintf(desc, "Internal Temperature is %d degrees \n", JTemp);
    printf ("%s", desc);

    /* Toggle LED */
    HAL_GPIO_TogglePin(GPIOJ, GPIO_PIN_5);
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
