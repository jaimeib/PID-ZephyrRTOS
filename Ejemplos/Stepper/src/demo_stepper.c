#include <stdio.h>
#include <posix/unistd.h>
#include "stepper_motor.h"

const int stepper_prio = 9;
#define STEPPER_PIN_1 5
#define STEPPER_PIN_2 6
#define STEPPER_PIN_3 7
#define STEPPER_PIN_4 8

#define STEPS 120

int main(int argc, char **argv)
{
	printf("Demo stepper in pins %d %d %d %d\n", STEPPER_PIN_1, STEPPER_PIN_2, STEPPER_PIN_3,
	       STEPPER_PIN_4);

	stepper_motor_initialize(stepper_prio);
	stepper_motor_setspeed(6);
	printf("s:%d o:%d\n", stepper_motor_get_current_step(),
	       stepper_motor_get_current_step_objective());

	for (int i = 0; i < 3; i++) {
		stepper_motor_move_to_step(STEPS);
		printf("s:%d o:%d\n", stepper_motor_get_current_step(),
		       stepper_motor_get_current_step_objective());

		k_msleep(4000);
		printf("End s:%d o:%d\n", stepper_motor_get_current_step(),
		       stepper_motor_get_current_step_objective());

		stepper_motor_move_to_step(-STEPS);
		printf("s:%d o:%d\n", stepper_motor_get_current_step(),
		       stepper_motor_get_current_step_objective());

		k_msleep(4000);
		printf("End s:%d o:%d\n", stepper_motor_get_current_step(),
		       stepper_motor_get_current_step_objective());
	}

	stepper_motor_move_to_step(0);
	printf("s:%d o:%d\n", stepper_motor_get_current_step(),
	       stepper_motor_get_current_step_objective());

	k_msleep(4000);
	printf("End s:%d o:%d\n", stepper_motor_get_current_step(),
	       stepper_motor_get_current_step_objective());

	return 0;
}
