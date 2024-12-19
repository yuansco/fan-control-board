
#ifndef __FAN_CONTROL_H
#define __FAN_CONTROL_H

#define FAN_RPM_MAX 6000
#define FAN_TEMP_MAX 80

/* Rpm deviation (Unit:percent) */
#define FAN_CTRL_RPM_DEVIATION 7

/* Dynamic margin of target rpm */
//#define FAN_CTRL_RPM_MARGIN(rpm_target) (((rpm_target) * FAN_CTRL_RPM_DEVIATION) / 100)

/* Static margin of target rpm */
#define FAN_CTRL_RPM_MARGIN(rpm_target) 100

#define FAN_CTRL_POLLING_TIME_MS 1000

#define FAN_CTRL_SMOOTH_TEMP

/* fan control mode list */
enum fan_control_mode_list {
        FAN_DUTY_CTRL = 0,
        FAN_RPM_CTRL,
        FAN_THERMAL_CTRL,
        FAN_NO_CTRL,
        FAN_COUNT
};

struct fan_step {
	int diff;
	int duty;
};

int fan_control_get_target_rpm(void);
int fan_control_get_target_temp(void);
void fan_control_set_target_rpm(int rpm);
void fan_control_set_target_temp(int temp);
int fan_control_get_actual_temp(void);
void fan_speed_up(void);
void fan_speed_down(void);
void fan_set_rpm_step(int rpm);
void fan_set_temp_step(int temp);
void fan_set_ctrl_mode(enum fan_control_mode_list mode);
void fan_control_init(void);


#endif /* __FAN_CONTROL_H */
