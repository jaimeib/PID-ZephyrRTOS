// MACROS TO OPERATE WITH TIMESPEC STRUCTS

#define incr_timespec(t1, t2)                                                                      \
	do {                                                                                       \
		(t1)->tv_sec += (t2)->tv_sec;                                                      \
		(t1)->tv_nsec += (t2)->tv_nsec;                                                    \
		if ((t1)->tv_nsec >= 1000000000) {                                                 \
			(t1)->tv_sec++;                                                            \
			(t1)->tv_nsec -= 1000000000;                                               \
		}                                                                                  \
	} while (0)
