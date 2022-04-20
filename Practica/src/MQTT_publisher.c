// Zephyr API
#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>

//Standard C + POSIX API
#include <stdio.h>
#include <posix/pthread.h>

//Modules
#include "main.h"

void publish(void *ptr_result)
{
	printf("Publisher thread started\n");
}
