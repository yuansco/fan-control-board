
#ifndef __CH224K_H__
#define __CH224K_H__


#include "main.h"

/* should define in board.h if needed */
//#define CONFIG_PD_CH224K
//#define CONFIG_PD_CH224K_DEBUG
//#define CONFIG_PD_CH224K_COMMAND
//#define CONFIG_PD_CH224K_ERR_DETECT
/* PD config pin define */
// #define CH224K_CFG_IO1 GPIO_CH224K_CFG1
// #define CH224K_CFG_IO2 GPIO_CH224K_CFG2
// #define CH224K_CFG_IO3 GPIO_CH224K_CFG3

#define PD_CHECK_DELAY_TIME_MS 2000
#define PD_CHECK_VOLT_DIFF_MV 800

/*
# IO config of Request voltage 
# +------+------+------+------+
# | CFG1 | CFG2 | CFG3 | Volt | 
# +------+------+------+------+
# |    1 |    - |    - |   5V | 
# |    0 |    0 |    0 |   9V | 
# |    0 |    0 |    1 |  12V | 
# |    0 |    1 |    1 |  15V | 
# |    0 |    1 |    0 |  20V | 
# +------+------+------+------+
*/


enum pd_volt {
        PD_VOLT_5V = 0,
        PD_VOLT_9V,
        PD_VOLT_12V,
        PD_VOLT_15V,
        PD_VOLT_20V,
        PD_VOLT_COUNT
};

extern const int pd_volt_value[PD_VOLT_COUNT];

enum pd_volt ch224k_get_pd_power(void);
int ch224k_select_pd_power(enum pd_volt volt);
int ch224k_init(void);
void pd_print(void);

#endif // __CH224K_H__
