#include "board.h"
#include "thermistors.h"
#include "console.h"

#ifdef CONFIG_THERMISTORS


// #define PU_VOLTAGE_MV 3300
// #define PU_RES_KOHM 10

/*
 * NTC-3950-100K 
 * R25-100.0K Q±1%   B25/50-3950K ±1% 
 * 
 * PU_VOLTAGE_MV = 3300
 * PU_RES_KOHM = 10
 * 
 * Voltage(mV)[T(°C)] = PU_VOLTAGE_MV * Rnom[T(°C)] / (PU_RES_KOHM + Rnom[T(°C)])
 */

/*
T(°C)    Rnom(Kohm)     Voltage(V)       Voltage(mV)
0        325.0790       3.201515762      3201
10       199.7229       3.142649515      3142
20       125.1824       3.055885382      3055
30       80.3868        2.934902442      2934
40       52.9307        2.775613651      2775
50       35.6960        2.577836134      2577
60       24.6029        2.346322707      2346
70       17.2854        2.090561986      2090
80       12.346         1.823225633      1823
90       8.9273         1.556486662      1556
100      6.5502         1.306066392      1306
110      4.8883         1.083494422      1083
120      3.7015         0.891504579      891
130      2.8116         0.724209310      724
140      2.1460         0.583056150      583
150      1.6649         0.471000180      471
160      1.3136         0.383156554      383
170      1.0512         0.313898943      313
180      0.8423         0.256365346      256
190      0.6768         0.209186273      209
200      0.5519         0.172601142      172
210      0.4566         0.1440984641     144
220      0.382          0.1214216914     121
230      0.3192         0.1020776804     102
240      0.2662         0.08556817518    85
250      0.2236         0.07217418522    72
260      0.1894         0.0613402163     61


*/

static const struct thermistor_data_pair thermistor_data[] = {
	{ 3201,   0 },
	{ 3142,  10 },
	{ 3055,  20 },
	{ 2934,  30 },
	{ 2775,  40 },
	{ 2577,  50 },
	{ 2346,  60 },
	{ 2090,  70 },
	{ 1823,  80 },
	{ 1556,  90 },
	{ 1306, 100 },
	{ 1083, 110 },
	{  891, 120 },
	{  724, 130 },
	{  583, 140 },
	{  471, 150 },
	{  383, 160 },
	{  313, 170 },
	{  256, 180 },
	{  209, 190 },
	{  172, 200 },
	{  144, 210 },
	{  121, 220 },
	{  102, 230 },
	{   85, 240 },
	{   72, 250 },
	{   61, 260 },
};

int thermistor_linear_interpolate(int mv,
				  const struct thermistor_data_pair *data)
{
	//const struct thermistor_data_pair *data = info;
	int v_high = 0, v_low = 0, t_low, t_high, num_steps;
	int head, tail, mid = 0;


	/*
	 * If input value is out of bounds return the lowest or highest
	 * value in the data sets provided.
	 */
	if (mv > data[0].mv)
		return data[0].temp;
	else if (mv < data[ARRAY_SIZE(thermistor_data) - 1].mv)
		return data[ARRAY_SIZE(thermistor_data) - 1].temp;

	head = 0;
	tail = ARRAY_SIZE(thermistor_data) - 1;
	while (head != tail) {
		mid = (head + tail) / 2;
		v_high = data[mid].mv;
		v_low = data[mid + 1].mv;

		if ((mv <= v_high) && (mv >= v_low))
			break;
		else if (mv > v_high)
			tail = mid;
		else if (mv < v_low)
			head = mid + 1;
	}

	t_low = data[mid].temp;
	t_high = data[mid + 1].temp;

	/*
	 * The obvious way of doing this is to figure out how many mV per
	 * degree are in between the two points (mv_per_deg_c), and then how
	 * many of those exist between the input voltage and voltage of
	 * lower temperature :
	 *   1. mv_per_deg_c = (v_high - v_low) / (t_high - t_low)
	 *   2. num_steps = (v_high - mv) / mv_per_deg_c
	 *   3. result = t_low + num_steps
	 *
	 * Combine #1 and #2 to mitigate precision loss due to integer division.
	 */
	num_steps = ((v_high - mv) * (t_high - t_low)) / (v_high - v_low);
	return (t_low + num_steps);
}

int get_temperature(int mv) {
        return thermistor_linear_interpolate(mv, thermistor_data);
}

#endif /* CONFIG_THERMISTORS */
