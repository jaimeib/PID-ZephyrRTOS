
//Standard C + POSIX API
#include <stdio.h>
#include <posix/unistd.h>
#include <stdlib.h>
#include <posix/pthread.h>

//Modules
#include <misc/error_checks.h>
#include "gui.h"
#include "stepper_motor.h"
#include "controller.h"
#include "screen.h"

const int stepper_prio = 9;
const int controller_prio = 7;
const struct timespec controller_period = { 0, 5000000 };
const int gui_prio = 4;
const struct timespec gui_period = { 0, 300000000 };

// procedimiento principal
int main(void)
{
	pthread_t th_controller;
	printf("Ball balance\n");
	k_msleep(1000);

	// inicializar pantalla
	screen_initialize();

	//inicializar gui
	gui_initialize(gui_prio, gui_period, CONTROLLER_DIST_NEAR, CONTROLLER_DIST_FAR);

	// inicializar stepper motor
	stepper_motor_initialize(stepper_prio);

	// inicializar controlador
	th_controller = controller_initialize(controller_prio, controller_period);
	CHK(pthread_join(th_controller, NULL));

	while (0) {
		for (int i = 0; i < 600; i++) {
			printf("Step Clockwise\n");
			stepper_motor_step_clockwise();
			usleep(20);
		}

		for (int i = 0; i < 600; i++) {
			printf("Step Anticlockwise\n");
			stepper_motor_step_anticlockwise();
			usleep(20);
		}
	}

	const int grads_per_second = 4;
	while (0) {
		printf("Rotate at %d grad/s\n", grads_per_second);
		stepper_motor_setspeed(grads_per_second);
		sleep(5);

		printf("Rotate at %d grad/s\n", grads_per_second);
		stepper_motor_setspeed(-grads_per_second);
		sleep(5);

		printf("Dist:%d\n", analogRead(0));
	}

	while (0) {
		printf("Dist:%d\n", analogRead(0));
		sleep(1);
	}

	return 0;
}
