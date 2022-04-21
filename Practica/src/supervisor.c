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
	//Sincronization variables:
	extern pthread_mutex_t mutex_result;
	extern pthread_cond_t cond_supervisor;
	extern bool new_result;

	printf("Supervisor thread started\n");

	thread_result_t last_result;

	// Infinite loop
	while (true) {
		// Wait for a new result
		pthread_mutex_lock(&mutex_result);

		// Wait for the signal from the sensor thread
		while (!new_result) {
			pthread_cond_wait(&cond_supervisor, &mutex_result);
		}

		// Copy the result
		last_result.type = ((thread_result_t *)ptr_result)->type;
		last_result.value = ((thread_result_t *)ptr_result)->value;
		new_result = false;

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
