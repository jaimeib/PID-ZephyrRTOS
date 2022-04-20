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

		switch (last_result.type) {
		case LIGHT:
			printf("SUPERVISOR: Light sensor last value: %d\n", last_result.value);
			break;
		case INTERNAL_TEMPERATURE:
			printf("SUPERVISOR: Internal temperature last value: %d\n",
			       last_result.value);
			break;
		default:
			printf("SUPERVISOR: Unknown sensor type\n");
			exit(EXIT_FAILURE);
		}
	}
}
