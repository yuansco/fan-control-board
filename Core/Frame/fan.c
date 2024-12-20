#include <stdint.h>
#include "main.h"
#include "adc.h"
#include "board.h"
#include "button.h"
#include "console.h"
#include "fan.h"
#include "gpio.h"
#include "led.h"
#include "task.h"
#include "util.h"
#include "page_control.h"

/* Fan PWM */

#ifdef CONFIG_FAN

#ifdef CONFIG_FAN_DEBUG
#define CPRINTF(format, args...) PRINTF("FAN: " format, ##args)
#define CPRINTS(format, args...) PRINTS("FAN: " format, ##args)
#else
#define CPRINTF(format, args...)
#define CPRINTS(format, args...)
#endif

static int fan_pwm_duty = 0;
static int duty_step = 5;

int fan_get_duty_step(void) {
        return duty_step;
}

int fan_set_duty_step(int step) {

        if(step > 100 || step < 0)
                return EC_ERROR_INVAL;

        duty_step = step;

        return EC_SUCCESS;
}

int fan_get_duty(void) {
        return fan_pwm_duty;
}

void fan_set_duty(int duty) {

        int pulse;

        if (duty<0) {
                duty=0;
        } else if (duty>100) {
                duty=100;
        }

        fan_pwm_duty = duty;

        pulse = fan_pwm_duty * TIM_PERIOD / 100;

        TIM_OC_InitTypeDef sConfigOC;

        sConfigOC.OCMode = TIM_OCMODE_PWM1;
        sConfigOC.Pulse = pulse;
        sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
        sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
        HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1);

        HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
}

void pwm_decrease_duty(void) {

        int duty = fan_pwm_duty - duty_step;

        if (duty < 0)
                duty = 0;

        fan_set_duty(duty);
}

void pwm_increase_duty(void) {

        int duty = fan_pwm_duty + duty_step;

        if (duty > 100)
                duty = 100;

        fan_set_duty(duty);
}

int pwm_test_loop(void) {

        static int step, duty;

        if(duty <= 0)
                step = 5;
        if(duty >= 100)
                step = -5;

        duty += step;
        fan_set_duty(duty); 

        CPRINTS("PWM: %d", duty);
        return EC_SUCCESS;
}

/* Fan tachometer */

static int cnt = 0;
static int fan_speed_rpm;
void start_fan_tach(void);

int board_tach_callback(void) {

        cnt++;
        return EC_SUCCESS;
}

int fan_get_rpm(void) {

        if (fan_speed_rpm > FAN_MAX_RPM)
                return FAN_MAX_RPM;
        if (fan_speed_rpm < 0)
                return 0;

        return fan_speed_rpm;
}

int calculate_fan_speed_deferred(void) {

        if (cnt == 0) {
                /* Sometimes lower pwm fails to drive the fan */
                if (fan_pwm_duty >= 5) {
                        CPRINTS("Fan stalled!");
                }
                fan_speed_rpm = 0;
        } else {
                fan_speed_rpm = (cnt * (60000/FAN_TACH_SAMPLE_TIME_MS)) / FAN_TACH_PULSE;
        }

        start_fan_tach();

        return EC_SUCCESS;
}

void start_fan_tach(void) {

        cnt = 0;
        hook_call_deferred(&calculate_fan_speed_deferred, FAN_TACH_SAMPLE_TIME_MS);
}

void fan_init(void) {

        /* pwm init */
        fan_set_duty(0);

#ifdef CONFIG_FAN_PWM_TEST
        hook_call_loop(&pwm_test_loop, 100);
#endif

        start_fan_tach();
}

/******************************************************************************/
/* FAN console command */

#ifdef CONFIG_FAN_COMMAND

void fan_print(void) {

        CPRINTS(" Fan ID: %d\t duty: %d\t rpm: %d", 0,
                        fan_pwm_duty,
                        fan_speed_rpm);

}

#endif  /* CONFIG_FAN_COMMAND */


#endif /* CONFIG_FAN */

