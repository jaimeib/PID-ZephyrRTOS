
#include <posix/pthread.h>

// Initialice the internal thread to manage a stepper motor.
// Return the internal thread id.
pthread_t stepper_motor_initialize(int pin1, int pin2, int pin3, int pin4, int prio);

// Set the desired speed.
void stepper_motor_setspeed(double grads_per_second);

// Move to the desired step at the preset speed.
void stepper_motor_move_to_step(int step_objective);

// Rotate one step clockwise.
// Continuos movement is stopped.
void stepper_motor_step_clockwise();

// Rotate one step anticlockwise.
// Continuos movement is stopped.
void stepper_motor_step_anticlockwise();

int stepper_motor_get_current_step();

int stepper_motor_get_current_step_objective();

void stepper_motor_status(int *step_current, int *step_objective);
