
#include "adc.h"
#include "board.h"
#include "ch224k.h"
#include "gpio.h"
#include "console.h"

#ifdef CONFIG_PD_CH224K

#ifdef CONFIG_PD_CH224K_DEBUG
#define CPRINTF(format, args...) PRINTF("CH224K: " format, ##args)
#define CPRINTS(format, args...) PRINTS("CH224K: " format, ##args)
#else
#define CPRINTF(format, args...)
#define CPRINTS(format, args...)
#endif

static int pd_state = PD_VOLT_5V;


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

const int pd_volt_value[PD_VOLT_COUNT] = {
        [PD_VOLT_5V] =   5,
        [PD_VOLT_9V] =   9,
        [PD_VOLT_12V] = 12,
        [PD_VOLT_15V] = 15,
        [PD_VOLT_20V] = 20,
};

enum pd_volt ch224k_get_pd_power(void) {
        return pd_state;
}

__overridable void board_error(int e) {

}

#ifdef CONFIG_PD_CH224K_ERR_DETECT
int ch224k_check_volt(void) {

        int vbus_curr, vbus_target;

        vbus_curr = adc_value_scaling[VBUS_MON];

        vbus_target = pd_volt_value[pd_state] * 1000;

        if (ABS(vbus_target - vbus_curr) > PD_CHECK_VOLT_DIFF_MV) {
                CPRINTS("pd request fail! config is %dV, but VBUS is %d.%02dV",
                                                vbus_target, (vbus_curr/1000), (vbus_curr%1000/10));
                board_error(0);
                return EC_ERROR_UNKNOWN;
        } else {
                CPRINTS("pd request success! current VBUS is %d.%02dV",
                                                (vbus_curr/1000), (vbus_curr%1000/10));
        }

        return EC_SUCCESS;
}
#endif

int ch224k_select_pd_power(enum pd_volt volt)
{
        pd_state = volt;
        pd_print();

        switch (volt)
        {
        case PD_VOLT_20V:
                gpio_set(CH224K_CFG_IO1, GPIO_OFF);
                gpio_set(CH224K_CFG_IO2, GPIO_ON);
                gpio_set(CH224K_CFG_IO3, GPIO_OFF);
                break;
        case PD_VOLT_15V:
                gpio_set(CH224K_CFG_IO1, GPIO_OFF);
                gpio_set(CH224K_CFG_IO2, GPIO_ON);
                gpio_set(CH224K_CFG_IO3, GPIO_ON);
                break;
        case PD_VOLT_12V:
                gpio_set(CH224K_CFG_IO1, GPIO_OFF);
                gpio_set(CH224K_CFG_IO2, GPIO_OFF);
                gpio_set(CH224K_CFG_IO3, GPIO_ON);
                break;
        case PD_VOLT_9V:
                gpio_set(CH224K_CFG_IO1, GPIO_OFF);
                gpio_set(CH224K_CFG_IO2, GPIO_OFF);
                gpio_set(CH224K_CFG_IO3, GPIO_OFF);
                break;
        // default is 5V
        case PD_VOLT_5V:
        default:
                gpio_set(CH224K_CFG_IO1, GPIO_ON);
                gpio_set(CH224K_CFG_IO2, GPIO_OFF);
                gpio_set(CH224K_CFG_IO3, GPIO_OFF);
                break;
        }

#ifdef CONFIG_PD_CH224K_ERR_DETECT
        hook_call_deferred(&ch224k_check_volt, PD_CHECK_DELAY_TIME_MS);

#endif
        return EC_SUCCESS;
}

int ch224k_init(void)
{
        CPRINTS("init");

        // default is 5V
        ch224k_select_pd_power(PD_VOLT_5V);

        return EC_SUCCESS;
}

/******************************************************************************/
/* ch224k console command */

#ifdef CONFIG_PD_CH224K_COMMAND

static const char *const pd_mode_name[] = {
        [PD_VOLT_5V] = "5V",
        [PD_VOLT_9V] = "9V",
        [PD_VOLT_12V] = "12V",
        [PD_VOLT_15V] = "15V",
        [PD_VOLT_20V] = "20V",
};

void pd_print(void) {

        CPRINTS("PD config is %s", pd_mode_name[pd_state]);
}
#else
void pd_print(void) {}
#endif

#endif /* CONFIG_PD_CH224K_COMMAND */
