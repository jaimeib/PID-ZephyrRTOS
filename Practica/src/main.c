#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#include "leds.h"

void light_sensor(void)
{
	printf("Light sensor thread started\n");
}

#define STACKSIZE 1024
#define NUM_THREADS 2
K_THREAD_STACK_ARRAY_DEFINE(stacks, NUM_THREADS, STACKSIZE);

/**
 * 
 */
void main(void)
{
	printf("Starting ... \n");

	//Threads variables:
	pthread_t green_led_thread, light_sensor_thread;
	struct sched_param sch_param;
	pthread_attr_t attr;

	// Set the priority of the main program to max_prio-1
	sch_param.sched_priority = (sched_get_priority_max(SCHED_FIFO) - 1);
	if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &sch_param) != 0) {
		printf("Error while setting main thread's priority\n");
		exit(1);
	}

	// Create the thread attributes object
	if (pthread_attr_init(&attr) != 0) {
		printf("Error while initializing attributes\n");
		exit(1);
	}

	// Thread is created dettached
	if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0) {
		printf("Error in detachstate attribute\n");
		exit(1);
	}

	// The scheduling policy is fixed-priorities, with
	// FIFO ordering for threads of the same priority
	if (pthread_attr_setschedpolicy(&attr, SCHED_FIFO) != 0) {
		printf("Error in schedpolicy attribute\n");
		exit(1);
	}

	// Set the priority of thread 1 to min_prio+5
	sch_param.sched_priority = (sched_get_priority_min(SCHED_FIFO) + 1);
	if (pthread_attr_setschedparam(&attr, &sch_param) != 0) {
		printf("Error en atributo schedparam\n");
		exit(1);
	}

	//Set the stack for Green LED thread
	pthread_attr_setstack(&attr, &stacks[0][0], STACKSIZE);

	// Creating thread that blinks Green LED
	if (pthread_create(&green_led_thread, &attr, (void *)blink_green, NULL) != 0) {
		printf("Error: failed to create Green LED thread\n");
		exit(1);
	}

	//Set the stack for light sensor thread
	pthread_attr_setstack(&attr, &stacks[1][0], STACKSIZE);

	// Creating thread that measures light intensity
	if (pthread_create(&light_sensor_thread, &attr, (void *)light_sensor, NULL) != 0) {
		printf("Error: failed to create light sensor thread\n");
		exit(1);
	}
}
