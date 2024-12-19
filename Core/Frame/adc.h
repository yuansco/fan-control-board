
#include <stdint.h>
#include "main.h"

/* should define in board.h if needed */
//#define CONFIG_ADC
//#define CONFIG_ADC_DEBUG
//#define CONFIG_ADC_COMMAND
//#define CONFIG_ADC_SMOOTH_VALUE

#define ADC_POLLING_TIME_MS 500


/**
 *  ADC
 */

enum adc_id {
        ADC_ID1 = 0,
        ADC_ID2,
        ADC_ID3,
        ADC_ID4,
        ADC_COUNT
};

/* led struct */
struct adc {
        /* adc name */
        const char *name;
        /* adc channel */
        uint32_t channel;
        /* adc channel */
        uint32_t rank;
        /* adc mul */
        int mul;
        /* adc div */
        int div; 
        /* callback function */
        const char *unit;
};

extern ADC_HandleTypeDef hadc1;

extern struct adc adc_list[ADC_COUNT];

extern uint32_t adc_value[ADC_COUNT];
extern int adc_value_scaling[ADC_COUNT];


int adc_init(void);
void adc_print(void);