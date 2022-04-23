/* LIGHT SENSOR CONFIGURATION*/

#define LIGHT_REFRESH_PERIOD 100 /* Light Sensor refresh period */
#define MAX_CONVERTED_VALUE 4095 /* Max converted value */
#define MAX_LIGHT_VALUE 100 /* Max light value */

/* ADC handler declaration */
static ADC_HandleTypeDef AdcHandle;

//ADC FUNCTIONS:
static void ADC_Config(void);
static void Error_Handler(void);
static void SystemClock_Config(void);
static void ADC_Select_CH0(void);

//Light Sensor FUNCTIONS:
void light_sensor(void *ptr_result);
