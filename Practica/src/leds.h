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
void blink(const struct led *led, uint32_t sleep_ms, uint32_t id);
void blink_red(void);
void blink_green(void);
