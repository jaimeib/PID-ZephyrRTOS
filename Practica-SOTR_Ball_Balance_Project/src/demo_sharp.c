
#include <stdio.h>
#include <posix/unistd.h>
#include <Arduino.h> //TODO:
//#include <wiring_analog.h>
#define ANALOG_PIN 1

//  x  -> y
// 45  -> 97 mm
// 390 -> 65 mm

// 290  -> 95 mm
// 650  -> 35 mm
// const double a = (45.0 - 390.0) / (97.0 - 65.0);
//const double a = (290.0 - 650.0) / (95.0 - 35.0);
//const double b = 3936;

uint32_t sharp_sensor_dist_mm()
{
	uint32_t measure = 0;
	measure = analogRead(ANALOG_PIN);
	//uint32_t dist_mm = (uint32_t) (a * measure + b);
	uint32_t dist_mm = ((2914.0 / (measure + 5.0)) - 1.0) * 10;
	printf("adc:%d mm:%d\n", measure, dist_mm);
	return dist_mm;
}

int main(int argc, char **argv)
{
	printf("Demo Sharp\n");
	//uint32_t dist = 0;
	while (1) {
		sharp_sensor_dist_mm();
		sleep(2);
	}

	return 0;
}
