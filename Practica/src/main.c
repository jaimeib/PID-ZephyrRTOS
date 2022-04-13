#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <sys/printk.h>
#include <sys/__assert.h>
#include <string.h>

//THREADS GENERAL CONFIGURATION:

/* size of stack area used by each thread */
#define STACKSIZE 1024

/* Green Led Thread Stack*/
K_THREAD_STACK_DEFINE(green_led_stack_data, STACKSIZE);
struct k_thread green_led_thread_data;

/* Light Sensor Thread Stack*/
K_THREAD_STACK_DEFINE(light_sensor_stack_data, STACKSIZE);
struct k_thread light_sensor_thread_data;

struct k_mutex console_mutex;

/* scheduling priority used by each thread */
#define HIGH_PRIORITY 7
#define LOW_PRIORITY 3

//LEDS GENERAL CONFIGURATION:

#define GREEN_LED_PERIOD_MS 1000
#define RED_LED_PERIOD_MS 100

#define LED0 DT_ALIAS(led0)
#define LED1 DT_ALIAS(led1)

#if !DT_NODE_HAS_STATUS(LED0, okay)
#error "Unsupported board: led0 devicetree alias is not defined"
#endif

#if !DT_NODE_HAS_STATUS(LED1, okay)
#error "Unsupported board: led1 devicetree alias is not defined"
#endif

struct led {
	struct gpio_dt_spec spec;
	const char *gpio_pin_name;
};

static const struct led led0 = {
	.spec = GPIO_DT_SPEC_GET_OR(LED0, gpios, { 0 }),
	.gpio_pin_name = DT_PROP_OR(LED0, label, "RED"),
};

static const struct led led1 = {
	.spec = GPIO_DT_SPEC_GET_OR(LED1, gpios, { 0 }),
	.gpio_pin_name = DT_PROP_OR(LED1, label, "GREEN"),
};

//LED FUNCTIONS:

void blink(const struct led *led, uint32_t sleep_ms, uint32_t id)
{
	const struct gpio_dt_spec *spec = &led->spec;
	int status = 0;
	int ret;

	if (!device_is_ready(spec->port)) {
		printk("Error: %s device is not ready\n", spec->port->name);
		return;
	}

	ret = gpio_pin_configure_dt(spec, GPIO_OUTPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure pin %d (LED '%s')\n", ret, spec->pin,
		       led->gpio_pin_name);
		return;
	}

	while (1) {
		//Switch led:
		gpio_pin_set(spec->port, spec->pin, status % 2);

		//Print on console:
		k_mutex_lock(&console_mutex, K_FOREVER);
		printk("Toggled led%d; Status=%s\n", id, status % 2 ? "ON" : "OFF");
		k_mutex_unlock(&console_mutex);

		//Sleep:
		k_msleep(sleep_ms);

		//Update status of the led:
		status++;
	}
}

void blink_red(void)
{
	//Blinks led0 with a period of 100 miliseconds:
	blink(&led0, RED_LED_PERIOD_MS, 0);
}

void blink_green(void)
{
	//Blinks led1 with a period of 1 second:
	blink(&led1, GREEN_LED_PERIOD_MS, 1);
}

//LIGHT SENSOR CONFIGURATION:

#define LIGHT_SENSOR_PERIOD_MS 100

//LIGHT SENSOR FUNCTIONS:
void light_sensor(void)
{
	while (1) {
		k_mutex_lock(&console_mutex, K_FOREVER);
		printk("Light sensor value: %d\n", 0);
		k_mutex_unlock(&console_mutex);
		k_msleep(LIGHT_SENSOR_PERIOD_MS);
	}
}

/**
 * 
 */
void main(void)
{
	printk("Starting ... \n");

	/*Creating thread that blinks Green LED*/
	k_tid_t thread_LED_green = k_thread_create(&green_led_thread_data, green_led_stack_data,
						   STACKSIZE, (k_thread_entry_t)blink_green, NULL,
						   NULL, NULL, LOW_PRIORITY, 0, K_NO_WAIT);

	/*If the GREEN LED thread is created failed: Blink Red LED*/
	if (thread_LED_green == 0) {
		k_mutex_lock(&console_mutex, K_FOREVER);
		printk("Error: failed to create Green LED thread\n");
		k_mutex_unlock(&console_mutex);
		blink_red();
	}

	/*Creating thread that blinks Grenn LED */
	k_tid_t thread_light_sensor =
		k_thread_create(&light_sensor_thread_data, light_sensor_stack_data, STACKSIZE,
				(k_thread_entry_t)light_sensor, NULL, NULL, NULL, HIGH_PRIORITY, 0,
				K_NO_WAIT);

	/*If the GREEN LED thread is created failed: Blink Red LED*/
	if (thread_light_sensor == 0) {
		k_mutex_lock(&console_mutex, K_FOREVER);
		printk("Error: failed to create Light Sensor thread\n");
		k_mutex_unlock(&console_mutex);
		blink_red();
	}
}
