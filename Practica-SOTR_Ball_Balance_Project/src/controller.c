
//Standard C + POSIX API
#include <stdio.h>
#include <posix/unistd.h>
#include <posix/pthread.h>
//#include <sched.h>
#include <time.h>
#include <errno.h>

// HAL API
#include <stm32f7xx_hal.h>
#include <stm32f7xx_hal_adc.h>

//Modules
#include <misc/error_checks.h>
#include <misc/timespec_operations.h>
#include "gui.h"
#include "stepper_motor.h"
#include "controller.h"

#define DIST_READ_MARGIN 0
#define DIST_MIN 500

#define KP -0.08
#define KI 0 // -0.00001

/* ADC handler declaration */
static ADC_HandleTypeDef AdcHandle;

//ADC FUNCTIONS:
static void ADC_Config(void);
static void Error_Handler(void);
static void SystemClock_Config(void);
static void ADC_Select_CH1(void);

static void SystemClock_Config(void)
{
	__HAL_RCC_ADC1_CLK_ENABLE(); // Habilita el reloj ADC1
}

static void ADC_Select_CH1(void)
{
	ADC_ChannelConfTypeDef sConfig = { 0 };
	/* Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
	sConfig.Channel = ADC_CHANNEL_4;
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

static uint16_t dist_input_gui;
static uint16_t distance = (CONTROLLER_DIST_NEAR - CONTROLLER_DIST_FAR) / 2;
static int angle;
static int integral = 0;

static int distance_mean()
{
#define NUM_MEASUREMENTS 3
	uint32_t sum = 0;
	uint32_t measure = 0;
	for (int i = 0; i < NUM_MEASUREMENTS; i++) {
		do {
			// Select ADC_CH1
			ADC_Select_CH1();
			HAL_ADC_Start(&AdcHandle);
			HAL_ADC_PollForConversion(&AdcHandle, 1000);
			measure = HAL_ADC_GetValue(&AdcHandle);
			HAL_ADC_Stop(&AdcHandle);
		} while (measure < CONTROLLER_DIST_FAR - DIST_READ_MARGIN ||
			 measure > CONTROLLER_DIST_NEAR + DIST_READ_MARGIN);
		sum += measure;
	}
	uint32_t dist = sum / NUM_MEASUREMENTS;
	//printf("x:%d\n", dist);
	return dist;
	/*
  printf("x:%d\n", dist);
  //  x  -> y
  // 45  -> 97 mm
  // 390 -> 65 mm

  // 290  -> 95 mm
  // 650  -> 35 mm
  //const double a = (45.0 - 390.0) / (97.0 - 65.0);
  const double a = (290.0 - 650.0) / (95.0 - 35.0);
  const double b = 3936;
     return (int) (a * dist + b);
  */
}

/*
double controller_get_ball_pos_rel() {
  return 1.0 - (((double) distance - DIST_FAR) / (DIST_NEAR - DIST_FAR));
   }
 */

uint16_t controller_get_ball_pos()
{
	return distance;
}

void controller_status(uint16_t *dist_objective, uint16_t *dist_current, int *current_angle,
		       int *integral_factor)
{
	*dist_objective = dist_input_gui;
	*dist_current = distance;
	*current_angle = angle;
	*integral_factor = integral;
}

struct timespec thread_period;

static void *controller_thread_body(void *arg)
{
	//int dist_prev = analogRead(ANALOG_PIN);
	integral = 0;

	//Configure sharp sensor with HAL API

	SystemClock_Config();

	/* Configure the ADC peripheral*/
	ADC_Config();

	// Hal API
	GPIO_InitTypeDef GPIO_InitStruct;

	// Pin A1 is in PA4, so we need to set it up!
	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	while (true) {
		dist_input_gui = gui_get_real_ball_x();
		//uint16_t dist_input_gui = (CONTROLLER_DIST_FAR + CONTROLLER_DIST_NEAR) / 2 - 100;
		distance = distance_mean();

		integral += dist_input_gui - distance;
		angle = (dist_input_gui - distance) * KP + integral * KI;
		//printf("d:%d o:%d angle:%d \n", dist_current, dist_input_gui, angle);
		stepper_motor_move_to_step(angle);

		long period_ms = timespec2msec(&thread_period);
		k_msleep(period_ms);
	}
}

pthread_t controller_initialize(int prio, struct timespec period)
{
	pthread_attr_t attr;
	pthread_t th;
	struct sched_param sch_param;

	thread_period = period;

	CHK(pthread_attr_init(&attr));
	//CHK(pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED)); //FIXME:
	CHK(pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE));
	CHK(pthread_attr_setschedpolicy(&attr, SCHED_FIFO));
	sch_param.sched_priority = prio;
	CHK(pthread_attr_setschedparam(&attr, &sch_param));
	CHK(pthread_create(&th, &attr, controller_thread_body, NULL));

	return th;
}
