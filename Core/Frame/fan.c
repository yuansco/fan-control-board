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


void fan_interrupt_callback(uint16_t GPIO_Pin) {

        if (GPIO_Pin == fan_list[FAN_1].tach_pin)
                fan_list[FAN_1].tach_callback();
        else
                return;
}

int fan_get_duty_step(enum fan_id id) {
        return fan_list[id].duty_step;
}

int fan_set_duty_step(enum fan_id id, int step) {

        if(step > 100 || step < 0)
                return EC_ERROR_INVAL;

        fan_list[id].duty_step = step;

        return EC_SUCCESS;
}

int fan_get_duty(enum fan_id id) {
        return fan_list[id].fan_pwm_duty;
}

void fan_set_duty(enum fan_id id, int duty) {

        int pulse;

        if (duty<0) {
                duty=0;
        } else if (duty>100) {
                duty=100;
        }

        fan_list[id].fan_pwm_duty = duty;

        pulse = fan_list[id].fan_pwm_duty * TIM_PERIOD / 100;

        TIM_OC_InitTypeDef sConfigOC;

        sConfigOC.OCMode = TIM_OCMODE_PWM1;
        sConfigOC.Pulse = pulse;
        sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
        sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
        HAL_TIM_PWM_ConfigChannel(fan_list[id].timer, &sConfigOC, fan_list[id].channel);

        HAL_TIM_PWM_Start(fan_list[id].timer, TIM_CHANNEL_1);
}

void pwm_decrease_duty(enum fan_id id) {

        int duty = fan_list[id].fan_pwm_duty - fan_list[id].duty_step;

        if (duty < 0)
                duty = 0;

        fan_set_duty(id, duty);
}

void pwm_increase_duty(enum fan_id id) {

        int duty = fan_list[id].fan_pwm_duty + fan_list[id].duty_step;

        if (duty > 100)
                duty = 100;

        fan_set_duty(id, duty);
}

__maybe_unused static int pwm_test_loop(void) {

        static int step, duty;

        if(duty <= 0)
                step = 5;
        if(duty >= 100)
                step = -5;

        duty += step;
        fan_set_duty(0, duty); 

        CPRINTS("fan 0: %d", duty);
        return EC_SUCCESS;
}

/* Fan tachometer */

static void start_fan_tach(void);

static int fan_1_tach_callback(void) {

        fan_list[0].tach_cnt++;
        return EC_SUCCESS;
}

int fan_get_rpm(enum fan_id id) {

        if (fan_list[id].fan_speed_rpm > FAN_MAX_RPM)
                return FAN_MAX_RPM;
        if (fan_list[id].fan_speed_rpm < 0)
                return 0;

        return fan_list[id].fan_speed_rpm;
}

static int calculate_fan_speed_deferred(void) {

        int i;

        for(i=0; i<FAN_COUNT; i++) {

                if (fan_list[i].tach_cnt == 0) {
                        /* Sometimes lower pwm fails to drive the fan */
                        if (fan_list[i].fan_pwm_duty >= 5) {
                                CPRINTS("Fan %d stalled!", i);
                        }
                        fan_list[i].fan_speed_rpm = 0;
                } else {
                        fan_list[i].fan_speed_rpm = (fan_list[i].tach_cnt * (60000/FAN_TACH_SAMPLE_TIME_MS)) / FAN_TACH_PULSE;
                }

                start_fan_tach();
        }

        return EC_SUCCESS;
}

static void start_fan_tach(void) {

        int i;

        for(i=0; i<FAN_COUNT; i++) {
                fan_list[i].tach_cnt = 0;
        }
        hook_call_deferred(&calculate_fan_speed_deferred, FAN_TACH_SAMPLE_TIME_MS);
}

void fan_init(void) {

        int i;

        /* pwm init */
        for(i=0; i<FAN_COUNT; i++) {
                fan_set_duty(i, 0);
        }

#ifdef CONFIG_FAN_PWM_TEST
        hook_call_loop(&pwm_test_loop, 200);
#endif

        start_fan_tach();
}

/******************************************************************************/

struct fan fan_list[] = {
        [FAN_1] = {
                .name = "fan 1",
                .timer = &htim3,
                .channel = TIM_CHANNEL_1,
                .tach_port = FAN_1_GPIO_Port,
                .tach_pin = FAN_1_Pin,
                .fan_pwm_duty = 0,
                .duty_step = 5,
                .tach_cnt = 0,
                .fan_speed_rpm = 0,
                .tach_callback = &fan_1_tach_callback,
        },
};

/******************************************************************************/
/* FAN console command */

#ifdef CONFIG_FAN_COMMAND

void fan_print(void) {

        int i;

        for(i=0; i<FAN_COUNT; i++) {
                PRINTS(" Fan ID: %d\t duty: %d\t rpm: %d", i,
                        fan_list[i].fan_pwm_duty,
                        fan_list[i].fan_speed_rpm);
        }
}

#endif  /* CONFIG_FAN_COMMAND */


#endif /* CONFIG_FAN */

