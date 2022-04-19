#define LIGHT_REFRESH_PERIOD 2000 /* Light Sensor refresh period */
#define MAX_CONVERTED_VALUE 4095 /* Max converted value */

/* ADC handler declaration */
static ADC_HandleTypeDef AdcHandle;

//ADC FUNCTIONS
static void ADC_Config(void);
static void Error_Handler(void);
static void SystemClock_Config(void);
static void ADC_Select_CH0(void);

//Light Sensor FUNCTIONS:
void light_sensor(void);
