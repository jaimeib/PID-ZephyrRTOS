/* INTERNAL TEMPERATURE SENSOR CONFIGURATION*/

// Internal temperature refresh period
#define INTERNAL_TEMP_SENSOR_PERIOD_MS 2000

// Constants for conversion of the internal temperature sensor value
#define MAX_CONVERTED_VALUE 4095 /* Max converted value */
#define AMBIENT_TEMP 25 /* Ambient Temperature */
#define VSENS_AT_AMBIENT_TEMP 760 /* VSENSE value (mv) at ambient temperature */
#define AVG_SLOPE 25 /* Avg_Solpe multiply by 10 */
#define VREF 3300

//Internal Temperature FUNCTIONS:
void internal_temp(void *ptr_result);
