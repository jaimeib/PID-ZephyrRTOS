#include <stdio.h>
#include <posix/unistd.h>
#include <posix/pthread.h>
#include <sched.h>
#include <time.h>
#include <misc/error_checks.h>
#include <misc/timespec_operations.h>
#include <Arduino.h>
#include "stepper_motor.h"

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

static void motor_rotate_step()
{
	digitalWrite(motor_pin1, bitRead(steps_lookup[step_index()], 0)); //TODO:
	digitalWrite(motor_pin2, bitRead(steps_lookup[step_index()], 1));
	digitalWrite(motor_pin3, bitRead(steps_lookup[step_index()], 2));
	digitalWrite(motor_pin4, bitRead(steps_lookup[step_index()], 3));
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
	pinMode(motor_pin1, OUTPUT); //TODO:
	pinMode(motor_pin2, OUTPUT);
	pinMode(motor_pin3, OUTPUT);
	pinMode(motor_pin4, OUTPUT);

	stepper_motor_setspeed(20);

	// Create thread
	pthread_attr_t attr;
	pthread_t th;
	struct sched_param sch_param;
	CHK(pthread_attr_init(&attr));
	CHK(pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED)); //FIXME: ??
	CHK(pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE));
	CHK(pthread_attr_setschedpolicy(&attr, SCHED_FIFO));
	sch_param.sched_priority = prio;
	CHK(pthread_attr_setschedparam(&attr, &sch_param));
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
	struct timespec next_activation;

	CHKE(clock_gettime(CLOCK_MONOTONIC, &next_activation));

	while (1) {
		if (step_counter < step_counter_objective) {
			stepper_motor_step_clockwise();
		} else if (step_counter > step_counter_objective) {
			stepper_motor_step_anticlockwise();
		}

		incr_timespec(&next_activation, &thread_period);
		CHK(clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_activation,
				    NULL)); //FIXME:
	}
}
