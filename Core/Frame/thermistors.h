
#ifndef __THERMISTORS_H__
#define __THERMISTORS_H__

#include <stdint.h>


/* should define in board.h if needed */
//#define CONFIG_THERMISTORS



#ifdef CONFIG_THERMISTORS

struct thermistor_data_pair {
	int mv; /* Scaled voltage level at ADC (in mV) */
	int temp; /* Temperature in Celsius */
};

int get_temperature(int mv);
#endif /* CONFIG_THERMISTORS */

#endif // __THERMISTORS_H__
