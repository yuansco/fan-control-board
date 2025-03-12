#include "adc.h"
#include "board.h"
#include "console.h"
#include "fan.h"
#include "fan_control.h"
#include "gpio.h"
#include "led.h"
#include "page_control.h"
#include "thermistors.h"
#include "util.h"

static int fan_rpm_target = 2000;
static int fan_rpm_step = 200;

static int fan_temp_target = 45;
static int fan_temp_step = 5;

static int temp_actual;

static enum fan_control_mode_list fan_mode = FAN_DUTY_CTRL;


void fan_set_ctrl_mode(enum fan_control_mode_list mode) {
        fan_mode = mode;
}

void fan_set_rpm_step(int rpm) {
        fan_rpm_step = rpm;
}

void fan_set_temp_step(int temp) {
        fan_temp_step = temp;
}

int fan_control_get_target_rpm(void) {
        return fan_rpm_target;
}

int fan_control_get_target_temp(void) {
        return fan_temp_target;
}

int fan_control_get_actual_temp(void) {
        return temp_actual;
}

void fan_control_set_target_rpm(int rpm) {
        fan_rpm_target = rpm;
        if (fan_rpm_target > FAN_RPM_MAX)
                fan_rpm_target = FAN_RPM_MAX;
        if (fan_rpm_target < 0)
                fan_rpm_target = 0;  
}

void fan_control_set_target_temp(int temp) {
        fan_temp_target = temp;
        if (fan_temp_target > FAN_TEMP_MAX)
                fan_temp_target = FAN_TEMP_MAX;
        if (fan_temp_target < 0)
                fan_temp_target = 0;  
}

void fan_increase_rpm(void) {
        fan_rpm_target += fan_rpm_step;
        if (fan_rpm_target > FAN_RPM_MAX)
                fan_rpm_target = FAN_RPM_MAX;
}

void fan_decrease_rpm(void) {
        fan_rpm_target -= fan_rpm_step;
        if (fan_rpm_target < 0)
                fan_rpm_target = 0;
}

void fan_increase_temp(void) {
        fan_temp_target += fan_temp_step;
        if (fan_temp_target > FAN_TEMP_MAX)
                fan_temp_target = FAN_TEMP_MAX;
}

void fan_decrease_temp(void) {
        fan_temp_target -= fan_temp_step;
        if (fan_temp_target < 0)
                fan_temp_target = 0;
}

void fan_speed_up(void) {

        switch (fan_mode)
        {
        case FAN_DUTY_CTRL:
                pwm_increase_duty(FAN_1);
                break;
        case FAN_RPM_CTRL:
                fan_increase_rpm();
                break;
        case FAN_THERMAL_CTRL:
                fan_increase_temp();
                break;
        default:
                break;
        }
}

void fan_speed_down(void) {

        switch (fan_mode)
        {
        case FAN_DUTY_CTRL:
                pwm_decrease_duty(FAN_1);
                break;
        case FAN_RPM_CTRL:
                fan_decrease_rpm();
                break;
        case FAN_THERMAL_CTRL:
                fan_decrease_temp();
                break;
        default:
                break;
        }
}

/**
 * thermal control policy
 * 
 *      e.g. target temperature = 45
 * 
 *      temperature(C)     duty(%)
 *                0    ->   10
 *               25    ->   10
 *               42    ->   15
 *               43    ->   20
 *               44    ->   25
 *               45    ->   30
 *               46    ->   45
 *               47    ->   65
 *               48    ->   90
 *               49    ->  100
 *               80    ->  100
 */

const struct fan_step fan_table[] = {
        { .diff = -4, .duty = 10 },
        { .diff = -3, .duty = 15 },
        { .diff = -2, .duty = 20 },
        { .diff = -1, .duty = 25 },
        { .diff =  0, .duty = 30 },
        { .diff =  1, .duty = 45 },
        { .diff =  2, .duty = 65 },
        { .diff =  3, .duty = 90 },
        { .diff =  4, .duty = 100},
};
const int fan_table_count = ARRAY_SIZE(fan_table);


void thermal_update_fan_duty(int temp_target, int temp_curr) {

        int i, temp_diff;

        temp_diff = temp_curr - temp_target;

        for(i=0; i<(fan_table_count-1); i++) {
                if(temp_diff <= fan_table[i].diff)
                        break;
        }

        //PRINTS("i=%d, temp=%3d, duty=%3d\n", i, temp_curr, fan_table[i].duty);
        fan_set_duty(0, fan_table[i].duty);
}

int fan_speed_control_loop(void) {

        int state, duty;
        int fan_adjust_step;
        int temp_curr;
        int rpm_curr, rpm_diff;

        duty = fan_get_duty(FAN_1);

        if (duty != 0)
                /* yellow led blinking */
                led_update_behavior(LED_YELLOW, ARRAY_SIZE(led_blinking_behavior),
                                (struct led_behavior *) &led_blinking_behavior);
        else 
                /* yellow led off */
                led_update_behavior(LED_YELLOW, ARRAY_SIZE(led_off_behavior),
                                (struct led_behavior *) &led_off_behavior);

        /* Do not automatically control the fan if the fan power is not turned on yet */
        state = gpio_get(GPIO_FAN_DC_CTRL);

        if (state == GPIO_OFF)
                return EC_SUCCESS;

        switch (fan_mode)
        {
        case FAN_RPM_CTRL:

                /* get current rpm */
                rpm_curr = fan_get_rpm(FAN_1);

                if(rpm_curr == 0 && fan_get_duty(FAN_1) == 100) {
                        PRINTS("Fan stalled!");
                        return EC_SUCCESS;
                }

                /*   -rpm_diff -> decrease rpm
                 *   +rpm_diff -> increase rpm
                 */
                rpm_diff = (fan_rpm_target - rpm_curr);

                /* do not adjust the fan if rpm_diff lower than RPM_MARGIN */
                if (ABS(rpm_diff) <= FAN_CTRL_RPM_MARGIN(fan_rpm_target))
                        break;

                /* In rpm control mode, use dynamic duty step to adjust the fan. */
                if (ABS(rpm_diff) > 1500) {
                        fan_adjust_step = 5;
                } else if(ABS(rpm_diff) > 1000) {
                        fan_adjust_step = 4;
                } else if(ABS(rpm_diff) > 600) {
                        fan_adjust_step = 3;
                } else if(ABS(rpm_diff) > 300) {
                        fan_adjust_step = 2;
                } else {
                        fan_adjust_step = 1;
                }

                /* increase rpm if rpm_diff is positive or
                 * decrease rpm if rpm_diff is negative.
                 */
                if (rpm_diff > 0) {
                        fan_set_duty(FAN_1, fan_get_duty(FAN_1) + fan_adjust_step);
                } else if (rpm_diff < 0) {
                        fan_set_duty(FAN_1, fan_get_duty(FAN_1) - fan_adjust_step);
                }
                break;
        case FAN_THERMAL_CTRL:

                /* get temperature from ADC scaling buffer */
                temp_curr = get_temperature(adc_value_scaling[TEMP_TARGET]);

#ifdef FAN_CTRL_SMOOTH_TEMP
                temp_actual = (temp_actual * 3 + temp_curr * 7) / 10;
#else
                temp_actual = temp_curr;
#endif
                //PRINTS("temp_actual:%d",temp_actual);
                /* update fan duty by thermal control policy */
                thermal_update_fan_duty(fan_temp_target, temp_actual);

                break;
        case FAN_DUTY_CTRL:
                /* manual mode */
                return EC_SUCCESS;
        case FAN_NO_CTRL:
                return EC_SUCCESS;
        default:
                break;
        }

        return EC_SUCCESS;
}

void fan_control_init(void) {
        hook_call_loop(&fan_speed_control_loop, FAN_CTRL_POLLING_TIME_MS);
}

