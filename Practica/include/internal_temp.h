/* INTERNAL TEMPERATURE SENSOR CONFIGURATION*/

#define TEMP_REFRESH_PERIOD 2000 /* Internal temperature refresh period */
#define MAX_CONVERTED_VALUE 4095 /* Max converted value */
#define AMBIENT_TEMP 25 /* Ambient Temperature */
#define VSENS_AT_AMBIENT_TEMP 760 /* VSENSE value (mv) at ambient temperature */
#define AVG_SLOPE 25 /* Avg_Solpe multiply by 10 */
#define VREF 3300

/* ADC handler declaration */
static ADC_HandleTypeDef AdcHandle;

//ADC FUNCTIONS:
static void ADC_Config(void);
static void Error_Handler(void);
static void SystemClock_Config(void);
static void ADC_Select_CHTemp(void);

//Internal Temperature FUNCTIONS:
void internal_temp(void *ptr_result);
