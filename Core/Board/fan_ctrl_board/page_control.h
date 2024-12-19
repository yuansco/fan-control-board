
#ifndef __PAGE_CONTROL_H
#define __PAGE_CONTROL_H

#define PAGE_FLASH_DELAY_MS 400

/* page list */
enum page_list {
        PAGE_DUTY_CTRL = 0,
        PAGE_RPM_CTRL,
        PAGE_THERMAL_CTRL,
        PAGE_SETUP,
        PAGE_SAVE_DATA,
        PAGE_COUNT
};

enum event_list {
        EVENT_SW_LEFT_SHORT = 0,
        EVENT_SW_LEFT_LONG,
        EVENT_SW_RIGHT_SHORT,
        EVENT_SW_RIGHT_LONG,
        EVENT_COUNT
};

enum setup_point_list {
        SETUP_POINT_0 = 0,
        SETUP_POINT_1,
        SETUP_POINT_2,
        SETUP_POINT_3,
        SETUP_POINT_COUNT
};

/* data type */
enum data_type {
        DATA_TYPE_VOLT = 0,
        DATA_TYPE_RPM,
        DATA_TYPE_PWM,
        DATA_TYPE_TEMP,
        DATA_TYPE_COUNT
};

__attribute__((__packed__)) struct eeprom_data {
        /* data id */
        uint8_t data_id;
        /* eeprom data format version */
        uint8_t version;
        /* setup_item */
        uint8_t setup_item[SETUP_POINT_COUNT];
        /* default fan target rpm */
        int fan_rpm_target;
        /* default fan target temperature */
        int fan_temp_target;
};

extern uint8_t setup_item[SETUP_POINT_COUNT];

struct setup_subitem {
        int value;
        const char *name;
};

extern struct setup_subitem const setup_subitem_pd_volt[5];
extern struct setup_subitem const setup_subitem_pwm_step[4];
extern struct setup_subitem const setup_subitem_rpm_step[4];
extern struct setup_subitem const setup_subitem_temp_step[4];

extern int save_data_ticking;

#define CONFIG_SAVE_DATA_SHOW_TICKING 6

/* user_setting.c */
#define CONFIG_BOARD_INFO_DEBUG
void save_setting(void);
void load_setting(void);

void enter_page(enum page_list page);
void event_trigger(enum event_list event);
void page_init(void);

/* board level console command function */
#ifdef CONFIG_CONSOLE_BOARD_COMMAND
extern struct console_command command_board_define;
#endif /* CONFIG_CONSOLE_BOARD_COMMAND */


#endif /* __PAGE_CONTROL_H */
