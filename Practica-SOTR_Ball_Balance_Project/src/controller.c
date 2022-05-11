
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
#include "HAL_ADC.h"

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
			// Select ADC_CH0
			ADC_Select_CH0();
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
	struct timespec next_activation;

	CHK(clock_gettime(CLOCK_MONOTONIC, &next_activation));

	//int dist_prev = analogRead(ANALOG_PIN);
	integral = 0;

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

	while (true) {
		dist_input_gui = gui_get_real_ball_x();
		//uint16_t dist_input_gui = (CONTROLLER_DIST_FAR + CONTROLLER_DIST_NEAR) / 2 - 100;
		distance = distance_mean();

		integral += dist_input_gui - distance;
		angle = (dist_input_gui - distance) * KP + integral * KI;
		//printf("d:%d o:%d angle:%d \n", dist_current, dist_input_gui, angle);
		stepper_motor_move_to_step(angle);

		incr_timespec(&next_activation, &thread_period);
		CHK(clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_activation,
				    NULL)); //FIXME:
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
