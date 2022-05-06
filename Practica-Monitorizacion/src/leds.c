// Zephyr API
#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>

// Standard C API + POSIX API
#include <stdio.h>
#include <posix/time.h>
#include <posix/unistd.h>
#include <posix/pthread.h>

// Modules
#include "leds.h"

static struct led led0 = {
	.spec = GPIO_DT_SPEC_GET_OR(LED0, gpios, { 0 }),
	.gpio_pin_name = DT_PROP_OR(LED0, label, "RED1"),
	.state = 0,
};

static struct led led1 = {
	.spec = GPIO_DT_SPEC_GET_OR(LED1, gpios, { 0 }),
	.gpio_pin_name = DT_PROP_OR(LED1, label, "GREEN1"),
	.state = 0,
};

static struct led led2 = {
	.spec = GPIO_DT_SPEC_GET_OR(LED2, gpios, { 0 }),
	.gpio_pin_name = DT_PROP_OR(LED2, label, "GREEN2"),
	.state = 0,
};

struct led *select_led(led_number_t led_name)
{
	//Pointer to the Led structure:
	struct led *led = NULL;

	//Select the led to blink
	switch (led_name) {
	case RED1:
		//Select the red (LD1) led:
		led = &led0;
		break;
	case GREEN1:
		//Select the green (LD2) led:
		led = &led1;
		break;
	case GREEN2:
		//Select the green (LD3) led:
		led = &led2;
		break;
	default:
		printf("Error: led_to_blink is not valid\n");
		exit(EXIT_FAILURE);
	}

	return led;
}

void blink_function(struct led *led, uint32_t sleep_ms)
{
	const struct gpio_dt_spec *spec = &led->spec;
	int status = 0;
	int rc;

	if (!device_is_ready(spec->port)) {
		//Print error message and return
		printf("Error: %s device is not ready\n", spec->port->name);
		return;
	}

	rc = gpio_pin_configure_dt(spec, GPIO_OUTPUT);
	if (rc != 0) {
		//Print error message and return
		printf("Error %d: failed to configure pin %d (LED '%s')\n", rc, spec->pin,
		       led->gpio_pin_name);
		return;
	}

	// Infinite loop
	while (true) {
		//Switch led:
		gpio_pin_set(spec->port, spec->pin, status % 2);
		led->state = status % 2;

		//Print on console:
		printf("Toggled led %s; Status=%s\n", led->gpio_pin_name,
		       led->state ? "ON" : "OFF");

		//Update status of the led:
		status++;

		//Wait for period (next activation):
		k_msleep(sleep_ms);
	}
}

void turn_function(struct led *led, int status)
{
	if (led->state == status) {
		//Led state is already in the desired state
		printf("Led %s is already in state %s\n", led->gpio_pin_name,
		       status ? "ON" : "OFF");
		return;
	}

	const struct gpio_dt_spec *spec = &led->spec;
	int rc;

	if (!device_is_ready(spec->port)) {
		//Print error message and return
		printf("Error: %s device is not ready\n", spec->port->name);
		return;
	}

	rc = gpio_pin_configure_dt(spec, GPIO_OUTPUT);
	if (rc != 0) {
		//Print error message and return
		printf("Error %d: failed to configure pin %d (LED '%s')\n", rc, spec->pin,
		       led->gpio_pin_name);
		return;
	}

	//Switch led:
	gpio_pin_set(spec->port, spec->pin, status);
	led->state = status;

	//Print on console:
	printf("Turned led %s; Status=%s\n", led->gpio_pin_name, led->state % 2 ? "ON" : "OFF");
}

void blink_led(led_number_t led_to_blink)
{
	printf("Blinking led thread started\n");

	//Select the led to blink
	struct led *led_selected = select_led(led_to_blink);

	//Blink the led
	blink_function(led_selected, BLINK_LED_PERIOD_MS);
}

void turn_led(led_number_t led_to_turn, int status)
{
	if (status != 0 && status != 1) {
		//Print error message and return
		printf("Error: status is not valid\n");
		return;
	}

	//Select the led to turn
	struct led *led_selected = select_led(led_to_turn);

	//Turn the led
	turn_function(led_selected, status);
}
