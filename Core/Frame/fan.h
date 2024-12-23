
#ifndef __FAN_H
#define __FAN_H

/* Fan PWM */

/* should define in board.h if needed */
//#define CONFIG_FAN
//#define CONFIG_FAN_DEBUG
//#define CONFIG_FAN_PWM_TEST
//#define CONFIG_FAN_COMMAND

/* fan list */
enum fan_id {
        FAN_1 = 0,
        FAN_COUNT
};

#define TIM_PRESCALER 16  // Prescaler = (16 - 1); 
#define TIM_PERIOD 160    // Period = (160 - 1);

/* Fan tachometer */

#define FAN_TACH_SAMPLE_TIME_MS 1000
#define FAN_TACH_PULSE 2

#define FAN_MAX_RPM 7000

struct fan {
        /* fan name */
        const char *name;
        /* timer */
        void *timer;            // (TIM_HandleTypeDef *) htim3;
        /* pwm output channel */
        int channel;
        /* fan port */
        void *tach_port;        // (GPIO_TypeDef *)
        /* button pin */
        const uint16_t tach_pin;
        /* fan pwm duty */
        int fan_pwm_duty;
        /* fan pwm duty step */
        int duty_step;
        /* fan tach cnt */
        int tach_cnt;
        /* fan speed rpm */
        int fan_speed_rpm;
        /* callback function */
        int (*tach_callback)(void);
};

void fan_interrupt_callback(uint16_t GPIO_Pin);
int fan_get_duty_step(enum fan_id id);
int fan_set_duty_step(enum fan_id id, int step);
int fan_get_duty(enum fan_id id);
void fan_set_duty(enum fan_id id, int duty);
void pwm_decrease_duty(enum fan_id id);
void pwm_increase_duty(enum fan_id id);
//int board_tach_callback(void); todo
int fan_get_rpm(enum fan_id id);
void fan_init(void);
#ifdef CONFIG_FAN_PWM_TEST
int pwm_test_loop(void);
#endif

extern struct fan fan_list[FAN_COUNT];

#endif /* __FAN_H */

