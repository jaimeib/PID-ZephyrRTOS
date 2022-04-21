//Zephyr API
#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>

//Standard C API
#include <stdio.h>
#include <stdlib.h>

//Modules
#include "leds.h"

struct led select_led(led_number_t led_name)
{
	//Led structure:
	struct led led;

	//Select the led to blink
	switch (led_name) {
	case RED1:
		led = led0;
		break;
	case GREEN1:
		led = led1;
		break;
	case GREEN2:
		led = led2;
		break;
	default:
		printf("Error: led_to_blink is not valid\n");
		exit(EXIT_FAILURE);
	}

	return led;
}

void blink_function(const struct led *led, uint32_t sleep_ms)
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

	// Infinite loop
	while (true) {
		//Switch led:
		gpio_pin_set(spec->port, spec->pin, status % 2);

		//Print on console:
		printf("Toggled led %s; Status=%s\n", led->gpio_pin_name,
		       status % 2 ? "ON" : "OFF");

		//Sleep:
		k_msleep(sleep_ms);

		//Update status of the led:
		status++;
	}
}

void turn_function(const struct led *led, int status)
{
	const struct gpio_dt_spec *spec = &led->spec;
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

	//Switch led:
	gpio_pin_set(spec->port, spec->pin, status);

	//Print on console:
	printf("Turned led %s; Status=%s\n", led->gpio_pin_name, status % 2 ? "ON" : "OFF");
}

void blink_led(led_number_t led_to_blink)
{
	struct led led_selected = select_led(led_to_blink);
	blink_function(&led_selected, BLINK_LED_PERIOD_MS);
}

void turn_led(led_number_t led_to_turn, int status)
{
	struct led led_selected = select_led(led_to_turn);
	turn_function(&led_selected, status);
}
