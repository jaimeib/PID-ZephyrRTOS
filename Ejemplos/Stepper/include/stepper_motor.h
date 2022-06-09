
#include <posix/pthread.h>

#define GPIO_IN1_PORT GPIOJ
#define GPIO_IN1_PIN GPIO_PIN_4
#define GPIO_IN2_PORT GPIOJ
#define GPIO_IN2_PIN GPIO_PIN_3
#define GPIO_IN3_PORT GPIOF
#define GPIO_IN3_PIN GPIO_PIN_7
#define GPIO_IN4_PORT GPIOC
#define GPIO_IN4_PIN GPIO_PIN_8

// Initialice the internal thread to manage a stepper motor.
// Return the internal thread id.
pthread_t stepper_motor_initialize(int prio);

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
