/* LEDS CONFIGURATION*/

typedef enum { RED1, GREEN1, GREEN2 } led_number_t;

//Blinking led period
#define BLINK_LED_PERIOD_MS 1000

#define LED0 DT_ALIAS(led0)
#define LED1 DT_ALIAS(led1)
#define LED2 DT_ALIAS(led2)

#if !DT_NODE_HAS_STATUS(LED0, okay)
#error "Unsupported board: led0 devicetree alias is not defined"
#endif

#if !DT_NODE_HAS_STATUS(LED1, okay)
#error "Unsupported board: led1 devicetree alias is not defined"
#endif

#if !DT_NODE_HAS_STATUS(LED2, okay)
#error "Unsupported board: led2 devicetree alias is not defined"
#endif

struct led {
	struct gpio_dt_spec spec;
	const char *gpio_pin_name;
	int state;
};

//LED FUNCTIONS:
void blink_function(struct led *led, uint32_t sleep_ms);
void turn_function(struct led *led, int status);

void blink_led(led_number_t led_to_blink);
void turn_led(led_number_t led_to_turn, int status);

struct led *select_led(led_number_t led_name);
