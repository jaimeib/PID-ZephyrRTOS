#include <posix/pthread.h>

#define CONTROLLER_DIST_FAR 220
#define CONTROLLER_DIST_NEAR 740

uint16_t controller_get_ball_pos();

void controller_status(uint16_t *dist_objective, uint16_t *dist_current, int *current_angle,
		       int *integral_factor);

pthread_t controller_initialize(int prio, struct timespec period);
