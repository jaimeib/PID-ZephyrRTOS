//Standart C + POSIX API
#include <posix/pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>

//Modules
#include "misc/timespec_operations.h"
#include "misc/error_checks.h"
#include "controller.h"
#include "screen.h"
#include "stepper_motor.h"
#include "gui.h"

#define BAR_LENGTH 600
#define BAR_HIGH 40
#define BALL_COLOR LCD_COLOR_BLACK
#define BALL_REAL_COLOR LCD_COLOR_RED
#define BAR_COLOR LCD_COLOR_BLUE
#define BALL_Y 250
#define BAR_Y 300
#define BAR_X_MIN 100
#define BAR_X_MAX (BAR_X_MIN + BAR_LENGTH)
#define BALL_RADIO (BAR_Y - BALL_Y)

static uint16_t dist_left_x;
static uint16_t dist_right_x;

static volatile uint16_t ball_touch_x;

/*static int dist_to_pixel(int dist) {
  int dist_rel = ((double) dist - dist_left_x) /
  ((double) dist_right_x - dist_left_x);
  int x_pixel =  dist_rel * BAR_LENGTH + BAR_X;
  printf("dist_rel:%d x_pixel:%d\n", dist_rel, x_pixel);
  return x_pixel;
}*/

static long map(long x, long in_min, long in_max, long out_min, long out_max)
{
	return (x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}

uint16_t gui_get_real_ball_x()
{
	return map(ball_touch_x, BAR_X_MIN, BAR_X_MAX, dist_left_x, dist_right_x);
}

static struct timespec thread_period;

void *gui_thread_body(void *args)
{
	uint16_t real_ball_x;
	ball_touch_x = screen_get_x_size() / 2;

	while (1) {
		// clear balls in the former position
		screen_delete_circle(ball_touch_x, BALL_Y, BALL_RADIO);
		screen_delete_circle(real_ball_x, BALL_Y, BALL_RADIO);

		// read current ball position
		//real_ball_x = dist_to_pixel(controller_get_ball_pos());
		real_ball_x = map(controller_get_ball_pos(), dist_left_x, dist_right_x, BAR_X_MIN,
				  BAR_X_MAX);

		// read touch
		int touch_x = screen_get_x_touch();
		if (touch_x != -1) {
			if (touch_x > BAR_X_MAX) {
				touch_x = BAR_X_MAX;
			}
			if (touch_x < BAR_X_MIN) {
				touch_x = BAR_X_MIN;
			}

			ball_touch_x = touch_x;
		}
		//printf("GUI: R:%d  Touch:%d\n", real_ball_x, ball_touch_x);

		// print bar and balls
		screen_fill_rectangle(BAR_X_MIN, BAR_Y, BAR_LENGTH, BAR_HIGH, BAR_COLOR);
		screen_fill_circle(ball_touch_x, BALL_Y, BALL_RADIO, BALL_COLOR);
		screen_draw_circle(real_ball_x, BALL_Y, BALL_RADIO, BALL_REAL_COLOR);

		// print status
		char str[25];
		snprintf(str, 25, "Pixel ball:%3d touch:%3d", real_ball_x, ball_touch_x);
		screen_print_text(TEXT_X, TEXT_Y, str, LCD_COLOR_BLACK);
		snprintf(str, 25, "Real  ball:%3d touch:%3d", controller_get_ball_pos(),
			 gui_get_real_ball_x());
		screen_print_text(TEXT_X, TEXT_Y + 30, str, LCD_COLOR_BLACK);

		// print stepper status
		int step_current;
		int step_objective;
		stepper_motor_status(&step_current, &step_objective);
		snprintf(str, 25, "Stepper %3d -> %3d", step_current, step_objective);
		screen_print_text(TEXT_X, TEXT_Y + 60, str, LCD_COLOR_BLACK);

		// print controller status
		uint16_t dist_objective;
		uint16_t dist_current;
		int current_angle;
		int integral_factor;
		controller_status(&dist_objective, &dist_current, &current_angle, &integral_factor);
		snprintf(str, 25, "Ctr %3d->%3d->%3d i:%3d", dist_objective, dist_current,
			 current_angle, integral_factor);
		screen_print_text(TEXT_X, TEXT_Y + 90, str, LCD_COLOR_BLACK);

		// Wait for the next activation
		long period_ms = timespec2msec(&thread_period);
		k_msleep(period_ms);
	}
}

int gui_initialize(int prio, struct timespec period, int real_x_left, int real_x_right)
{
	screen_initialize();

	thread_period = period;
	dist_left_x = real_x_left;
	dist_right_x = real_x_right;

	// create thread
	pthread_attr_t attr;
	pthread_t th;
	struct sched_param sch_param;

	// Crea el objeto de atributos
	CHK(pthread_attr_init(&attr));
	//CHK(pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED)); //FIXME:
	CHK(pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE));
	CHK(pthread_attr_setschedpolicy(&attr, SCHED_FIFO));
	sch_param.sched_priority = prio;
	CHK(pthread_attr_setschedparam(&attr, &sch_param));
	CHK(pthread_create(&th, &attr, gui_thread_body, NULL));

	return 0;
}
