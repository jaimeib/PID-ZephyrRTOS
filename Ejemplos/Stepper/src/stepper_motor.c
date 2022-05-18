//Standard C + POSIX API
#include <stdio.h>
#include <posix/unistd.h>
#include <posix/pthread.h>
//#include <sched.h>
#include <time.h>
#include <math.h>

// HAL API
#include <stm32f7xx_hal.h>

//Modules
#include <misc/error_checks.h>
#include <misc/timespec_operations.h>
#include "stepper_motor.h"

//Threads stacks declaration
#define STACKSIZE 1024
K_THREAD_STACK_ARRAY_DEFINE(stacks, 1, STACKSIZE);

static int motor_pin1; // 28BYJ48 In1
static int motor_pin2; // 28BYJ48 In2
static int motor_pin3; // 28BYJ48 In3
static int motor_pin4; // 28BYJ48 In4

static int step_counter = 0; // contador para los pasos
static int step_counter_objective = 0;

#define STEPS_PER_GRAD 11.322
#define MX_GRADS_PER_S 20

//secuencia media fase
#define NUM_STEPS 8
static const char steps_lookup[NUM_STEPS] = { 0b1000, 0b1100, 0b0100, 0b0110,
					      0b0010, 0b0011, 0b0001, 0b1001 };

#define PERIOD_INACTIVE_NS 10000000 // 10 ms
static struct timespec thread_period = { 0, PERIOD_INACTIVE_NS };
static double rotating_speed_grads_per_second = 0.0;

static int step_index()
{
	int mod = step_counter % NUM_STEPS;
	if (mod < 0) {
		mod = mod + NUM_STEPS;
	}
	return mod;
}

static int bitRead(int value, int bit)
{
	return (value >> bit) & 0b0001;
}

static void motor_rotate_step()
{
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, bitRead(steps_lookup[step_index()], 0));
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_3, bitRead(steps_lookup[step_index()], 1));
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, bitRead(steps_lookup[step_index()], 2));
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, bitRead(steps_lookup[step_index()], 3));
}

static void *thread_motor(void *arg);

// Initialice the internal thread to manage a stepper motor.
// Returns the internal thread id.
pthread_t stepper_motor_initialize(int pin1, int pin2, int pin3, int pin4, int prio)
{
	// assign pins
	motor_pin1 = pin1;
	motor_pin2 = pin2;
	motor_pin3 = pin3;
	motor_pin4 = pin4;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOJ_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct;

	//Configure GPIO pins:

	// Pin D5 is in PC8, so we need to set it up!
	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	// Pin D6 is in PF7, so we need to set it up!
	GPIO_InitStruct.Pin = GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

	// Pin D7 is in PJ3, so we need to set it up!
	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);

	// Pin D8 is in PJ4, so we need to set it up!
	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);

	stepper_motor_setspeed(20);

	// Create thread
	pthread_attr_t attr;
	pthread_t th;
	struct sched_param sch_param;
	CHK(pthread_attr_init(&attr));
	//CHK(pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED)); //FIXME:
	CHK(pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE));
	CHK(pthread_attr_setschedpolicy(&attr, SCHED_FIFO));
	sch_param.sched_priority = prio;
	CHK(pthread_attr_setschedparam(&attr, &sch_param));
	CHK(pthread_attr_setstack(&attr, &stacks[0][0], STACKSIZE));
	CHK(pthread_create(&th, &attr, thread_motor, NULL));

	return th;
}

// Set desired speed.
void stepper_motor_setspeed(double grads_per_second)
{
	if (grads_per_second > MX_GRADS_PER_S) {
		grads_per_second = MX_GRADS_PER_S;
	} else if (grads_per_second < -MX_GRADS_PER_S) {
		grads_per_second = -MX_GRADS_PER_S;
	} else if (fabs(grads_per_second) < 0.1) {
		rotating_speed_grads_per_second = 0.0;
		thread_period.tv_nsec = PERIOD_INACTIVE_NS;
		return;
	}

	rotating_speed_grads_per_second = grads_per_second;
	thread_period.tv_nsec = abs((int)(1000000000 / STEPS_PER_GRAD / grads_per_second));
	//printf("thread_period.tv_nsec:%dns\n", thread_period.tv_nsec);
}

// Move to the desired step at the preset speed.
void stepper_motor_move_to_step(int step_objective)
{
	step_counter_objective = step_objective;
}

// Rotate one step clockwise
void stepper_motor_step_clockwise()
{
	step_counter++;
	motor_rotate_step();
}

// Rotate one step anticlockwise
void stepper_motor_step_anticlockwise()
{
	step_counter--;
	motor_rotate_step();
}

int stepper_motor_get_current_step()
{
	return step_counter;
}

int stepper_motor_get_current_step_objective()
{
	return step_counter_objective;
}

void stepper_motor_status(int *step_current, int *step_objective)
{
	*step_current = step_counter;
	*step_objective = step_counter_objective;
}

/*
 * Motor thread.
 */
static void *thread_motor(void *arg)
{
	while (1) {
		if (step_counter < step_counter_objective) {
			stepper_motor_step_clockwise();
		} else if (step_counter > step_counter_objective) {
			stepper_motor_step_anticlockwise();
		}

		long period_ms = timespec2msec(&thread_period);
		k_msleep(period_ms);
	}
}
