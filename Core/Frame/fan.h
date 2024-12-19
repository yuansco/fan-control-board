
#ifndef __FAN_H
#define __FAN_H

/* Fan PWM */

/* should define in board.h if needed */
//#define CONFIG_FAN
//#define CONFIG_FAN_DEBUG
//#define CONFIG_FAN_PWM_TEST
//#define CONFIG_FAN_COMMAND


#define TIM_PRESCALER 16  // Prescaler = (16 - 1); 
#define TIM_PERIOD 160    // Period = (160 - 1);

/* Fan tachometer */

#define FAN_TACH_SAMPLE_TIME_MS 1000
#define FAN_TACH_PULSE 2

#define FAN_MAX_RPM 7000

int fan_get_duty_step(void);
int fan_set_duty_step(int step);
void fan_set_duty(int duty);
int fan_get_duty(void);
void pwm_increase_duty(void);
void pwm_decrease_duty(void);
int board_tach_callback(void);
int fan_get_rpm(void);
void fan_init(void);
#ifdef CONFIG_FAN_PWM_TEST
int pwm_test_loop(void);
#endif
#endif /* __FAN_H */

