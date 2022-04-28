// Zephyr API
#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>

//Standard C + POSIX API
#include <stdio.h>
#include <posix/pthread.h>

//Modules
#include "main.h"
#include "supervisor.h"
#include "leds.h"

//Supervisor FUNCTIONS:
void supervisor(void *ptr_result)
{
	//Sincronization variables for the result:
	extern pthread_mutex_t mutex_result;
	extern pthread_cond_t cond_result;
	extern bool new_result_for_supervisor;

	//Sincronization variables for the supervisor is ready:
	extern pthread_mutex_t mutex_supervisor_ready;
	extern pthread_cond_t cond_supervisor_ready;
	extern bool supervisor_ready;

	printf("Supervisor thread started\n");

	thread_result_t last_result;

	//Signal the main thread that the supervisor thread is ready
	pthread_mutex_lock(&mutex_supervisor_ready);
	supervisor_ready = true;
	pthread_cond_signal(&cond_supervisor_ready);
	pthread_mutex_unlock(&mutex_supervisor_ready);
	printf("Supervisor thread is ready\n");

	// Infinite loop
	while (true) {
		// Wait for a new result
		pthread_mutex_lock(&mutex_result);
		// Wait for the signal from the sensor thread
		while (!new_result_for_supervisor) {
			pthread_cond_wait(&cond_result, &mutex_result);
		}

		// Copy the result
		last_result.type = ((thread_result_t *)ptr_result)->type;
		last_result.value = ((thread_result_t *)ptr_result)->value;
		new_result_for_supervisor = false;

		// Unlock the mutex
		pthread_mutex_unlock(&mutex_result);

		//Turns on/off leds depending on the results value of each sensor:
		switch (last_result.type) {
		case LIGHT:
			if (last_result.value <= LOW_LIGHT_VALUE) {
				// Turn on the Green LED 1
				turn_led(GREEN1, 1);
			} else {
				// Turn off the Green LED 1
				turn_led(GREEN1, 0);
			}
			break;
		case INTERNAL_TEMPERATURE:
			if (last_result.value >= HIGH_TEMP_VALUE) {
				// Turn on the Green LED 2
				turn_led(GREEN2, 1);
			} else {
				// Turn off the Green LED 2
				turn_led(GREEN2, 0);
			}
			break;
		default:
			printf("SUPERVISOR: Unknown sensor type\n");
			exit(EXIT_FAILURE);
		}
	}
}
