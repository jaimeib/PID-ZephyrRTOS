//Zephyr API
#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>

//Standard C API
#include <stdio.h>

#include "leds.h"

void blink(const struct led *led, uint32_t sleep_ms, uint32_t id)
{
	const struct gpio_dt_spec *spec = &led->spec;
	int status = 0;
	int ret;

	if (!device_is_ready(spec->port)) {
		printf("Error: %s device is not ready\n", spec->port->name);
		return;
	}

	ret = gpio_pin_configure_dt(spec, GPIO_OUTPUT);
	if (ret != 0) {
		printf("Error %d: failed to configure pin %d (LED '%s')\n", ret, spec->pin,
		       led->gpio_pin_name);
		return;
	}

	while (1) {
		//Switch led:
		gpio_pin_set(spec->port, spec->pin, status % 2);

		//Print on console:
		printf("Toggled led%d; Status=%s\n", id, status % 2 ? "ON" : "OFF");

		//Sleep:
		k_msleep(sleep_ms);

		//Update status of the led:
		status++;
	}
}

void blink_red(void)
{
	printf("Red LED thread started\n");

	//Blinks led0
	blink(&led0, RED_LED_PERIOD_MS, 0);
}

void blink_green(void)
{
	printf("Green LED thread started\n");

	//Blinks led1
	blink(&led1, GREEN_LED_PERIOD_MS, 1);
}
