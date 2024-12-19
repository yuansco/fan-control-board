
#include "adc.h"
#include "main.h"
#include "console.h"
#include "task.h"
#include "util.h"

#include <stdint.h>

/******************************************************************************/
/* ADC  */


#ifdef CONFIG_ADC_DEBUG
#define CPRINTF(format, args...) PRINTF("ADC: " format, ##args)
#else
#define CPRINTF(format, args...)
#endif

uint32_t adc_value[ADC_COUNT];
int adc_value_scaling[ADC_COUNT];


struct adc adc_list[] = {
        [ADC_ID1] = {
                .name = "gpio ch1",
                .channel = ADC_CHANNEL_1,
                .rank = ADC_REGULAR_RANK_1,
                .mul = 3300,
                .div = 4096,
                .unit = "mV",
        },
        [ADC_ID2] = {
                .name = "gpio ch2",
                .channel = ADC_CHANNEL_2,
                .rank = ADC_REGULAR_RANK_2,
                .mul = 3300,
                .div = 4096,
                .unit = "mV",
        },
        [ADC_ID3] = {
                .name = "gpio ch3",
                .channel = ADC_CHANNEL_3,
                .rank = ADC_REGULAR_RANK_3,
                .mul = 3300,
                .div = 4096,
                .unit = "mV",
        },
        [ADC_ID4] = {
                .name = "gpio ch4",
                .channel = ADC_CHANNEL_4,
                .rank = ADC_REGULAR_RANK_4,
                .mul = 3300,
                .div = 4096,
                .unit = "mV",
        },
};


/**
 * e.g. scaling 0 ~ 4096 to 0 ~ 3300 mV 
 *        mul = 3300 , div = 4096
 *        scaling = adc_value * 3300 / 4096
 * 
 *      scaling 0 ~ 4096 to 0 ~ 22000 mV 
 *        mul = 22000 , div = 4096
 *        scaling = adc_value * 22000 / 4096
 * 
 */ 
void update_scaling_data(void) {

        int i, value_scaling;

        for (i = 0; i < ADC_COUNT; i++) {
                value_scaling = (adc_value[i] * adc_list[i].mul) / adc_list[i].div;

#ifdef CONFIG_ADC_SMOOTH_VALUE
                if (adc_value_scaling[i] == -1) {
                        adc_value_scaling[i] = value_scaling;
                } else {
                        adc_value_scaling[i] = (value_scaling + adc_value_scaling[i]) /2;
                }
#else /* CONFIG_ADC_SMOOTH_VALUE */
                adc_value_scaling[i] = value_scaling;
#endif
        }
}

void read_adc_all(void) {

        int i;

        HAL_ADCEx_Calibration_Start(&hadc1);
        for (i = 0; i < ADC_COUNT; i++) {
                HAL_ADC_Start(&hadc1);
                HAL_ADC_PollForConversion(&hadc1, 50);

                /* Check if the continous conversion is finished */
                if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC)) {
                        /* Get the converted value */
                        adc_value[i] = HAL_ADC_GetValue(&hadc1);
                }
        }

        HAL_ADC_Stop(&hadc1);
}

int adc_polling_loop(void) {

        read_adc_all();
        update_scaling_data();

        return EC_SUCCESS;
}

int adc_init(void) {

        CPRINTF("init\r\n");

        // adc_print();

        for (int i = 0; i < ADC_COUNT; i++) {
                adc_value_scaling[i] = -1;
        }

        CPRINTF("Start conversion\r\n");
        hook_call_loop(&adc_polling_loop, ADC_POLLING_TIME_MS);

        return EC_SUCCESS;
}

/******************************************************************************/
/* ADC console command */

#ifdef CONFIG_ADC_COMMAND

void adc_print(void) {

        int id;

        //read_adc_all();

        PRINTF("\r\nADC info:\r\n");

        for (id = 0; id < ARRAY_SIZE(adc_list); id++) {

                PRINTF(" ADC %d\tname: %-10s\tread: %4d %s (%4d)\r\n", id,
                        adc_list[id].name,
                        adc_value_scaling[id],
                        adc_list[id].unit,
                        (int) adc_value[id]);

        }
}

#endif



