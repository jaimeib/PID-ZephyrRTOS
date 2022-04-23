/* MAIN CONFIGURATION*/

typedef enum { LIGHT, INTERNAL_TEMPERATURE } sensor_type_t;

typedef struct {
	sensor_type_t type;
	int value;
} thread_result_t;
