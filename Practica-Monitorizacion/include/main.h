/* MAIN CONFIGURATION*/

typedef enum { LIGHT, INTERNAL_TEMPERATURE } sensor_type_t;

typedef struct {
	sensor_type_t type;
	double value;
} thread_result_t;

//THREADS CONFIGURATION:
#define STACKSIZE 2048
#define NUM_THREADS 5
