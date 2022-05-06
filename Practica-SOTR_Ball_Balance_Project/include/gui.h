#ifndef _BALANCER_GUI_H_
#define _BALANCER_GUI_H_
#include <posix/pthread.h>
#define TEXT_Y 40
#define TEXT_X 300

/*
 * Create the gui thread.
 * @param prio priority of the thread.
 * @param period period of the thread.
 * @param real_x_left real x coordenate of the left border of the bar. It is
 * the value used by the controller as input poisition value.
 * @param real_y_left real y coordenate of the left border of the bar. It is
 * the value used by the controller as input poisition value.
 */
int gui_initialize(int prio, struct timespec period, int real_x_left, int real_x_right);

// User's ball X position in range real_x_left..real_x_right
uint16_t gui_get_real_ball_x();

#endif // _BALANCER_GUI_H_
