#include "adc.h"
#include "at24c02.h"
#include "board.h"
#include "ch224k.h"
#include "console.h"
#include "fan.h"
#include "fan_control.h"
#include "gpio.h"
#include "led.h"
#include "page_control.h"
#include "ssd1306_show.h"
#include "thermistors.h"
#include "util.h"

#include <string.h>


#define CONFIG_PAGE_CONTROL_DEBUG

#ifdef CONFIG_PAGE_CONTROL_DEBUG
#define CPRINTF(format, args...) PRINTF("PAGE: " format, ##args)
#define CPRINTS(format, args...) PRINTS("PAGE: " format, ##args)
#else
#define CPRINTF(format, args...)
#define CPRINTS(format, args...)
#endif


char const page_data_control_duty[BUFF_SIZE_COLS][BUFF_SIZE_ROWS] = {
        "  FAN Duty Ctrl  ",
        "                 ",
        " VBUS:    12.00 V",
        " FAN:     12.00 V",
        " RPM:        7000",
        ">PWM:       100 %"
};

char const page_data_control_rpm[BUFF_SIZE_COLS][BUFF_SIZE_ROWS] = {
        "  Auto RPM Ctrl  ",
        "                 ",
        " FAN:     12.00 V",
        " PWM:         0 %",
        " Actual RPM: 7000",
        ">Target RPM: 6500"
};

char const page_data_control_thermal[BUFF_SIZE_COLS][BUFF_SIZE_ROWS] = {
        "  Auto Temp Ctrl ",
        "                 ",
        " FAN:     12.00 V",
        " PWM:         0 %",
        " Actual T:   55 C",
        ">Target T:   65 C"
};

char const page_data_setup[BUFF_SIZE_COLS][BUFF_SIZE_ROWS] = {
        "   Setup  Mode   ",
        "                 ",
        " PD Volt:      5 ",
        " PWM Step:    10 ",
        " RPM Step:  1000 ",
        " Temp Step:   10 "
};

char const page_data_setup_save[BUFF_SIZE_COLS][BUFF_SIZE_ROWS] = {
        "                 ",
        "                 ",
        "  Saving data... ",
        "                 ",
        "                 ",
        "                 ",
};

struct setup_subitem const setup_subitem_pd_volt[] = {
        [0] = {5,  " 5"},
        [1] = {9,  " 9"},
        [2] = {12, "12"},
        [3] = {15, "15"},
        [4] = {20, "20"},
};

struct setup_subitem const setup_subitem_pwm_step[] = {
        [0] = {1,  " 1"},
        [1] = {5,  " 5"},
        [2] = {10, "10"},
        [3] = {20, "20"},
};

struct setup_subitem const setup_subitem_rpm_step[] = {
        [0] = {100,  " 100"},
        [1] = {200,  " 200"},
        [2] = {500,  " 500"},
        [3] = {1000, "1000"},
};

struct setup_subitem const setup_subitem_temp_step[] = {
        [0] = {1,  " 1"},
        [1] = {2,  " 2"},
        [2] = {5,  " 5"},
        [3] = {10, "10"},
};

/* default setup_item */
uint8_t setup_item[SETUP_POINT_COUNT] = {0, 1, 1 ,2};

static enum page_list current_page;
static enum setup_point_list setup_point;
int save_data_ticking;

void format_data_line(int line, enum data_type t, int data) {
        switch (t)
        {
        case DATA_TYPE_VOLT:
                // page_data[line] = " VBUS:    xx.xx V"
                page_data[line][10] = (char) (data / 10000 + 48);
                page_data[line][11] = (char) (data % 10000 / 1000 + 48);
                page_data[line][13] = (char) (data % 1000 / 100 + 48);
                page_data[line][14] = (char) (data % 100 / 10 + 48);

                if (page_data[line][10] == '0')
                        page_data[line][10] = ' ';
                break;
        case DATA_TYPE_RPM:
                // page_data[line] = " RPM:        7000"
                page_data[line][13] = (char) (data / 1000 + 48);
                page_data[line][14] = (char) (data % 1000 / 100 + 48);
                page_data[line][15] = (char) (data % 100 / 10 + 48);
                page_data[line][16] = (char) (data % 10 + 48);

                /* cut 0 in herder on each number*/
                for (int i = 13; i < 16; i++) {
                        if (page_data[line][i] == '0')
                                page_data[line][i] = ' ';
                        else
                                break;
                }
                break;
        case DATA_TYPE_PWM:
        case DATA_TYPE_TEMP:
                // page_data[line] = " PWM:       100 %"
                page_data[line][12] = (char) (data / 100 + 48);
                page_data[line][13] = (char) (data % 100 / 10 + 48);
                page_data[line][14] = (char) (data % 10 + 48);

                /* cut 0 in herder on each number*/
                for (int i = 12; i < 14; i++) {
                        if (page_data[line][i] == '0')
                                page_data[line][i] = ' ';
                        else
                                break;
                }
                break;

        default:
                break;
        }
}

static int save_setting_flag;

int page_data_update_loop(void) {

        int vbus, vfan, duty;
        int fan_rpm_actual, fan_rpm_target;
        int temp_actual, temp_target;

        vfan = adc_value_scaling[FAN_PWR_MON];
        fan_rpm_actual = fan_get_rpm();
        duty = fan_get_duty();


        // CPRINTS("vbus=%5d ,vfan=%5d ,fan_rpm_actual=%4d ,fan_rpm_target=%4d ,duty=%3d",
        //         vbus, vfan, fan_rpm_actual, fan_rpm_target, duty);

        switch (current_page)
        {
        case PAGE_DUTY_CTRL:

                vbus = adc_value_scaling[VBUS_MON];

                format_data_line(2, DATA_TYPE_VOLT, vbus);
                format_data_line(3, DATA_TYPE_VOLT, vfan);
                format_data_line(4, DATA_TYPE_RPM, fan_rpm_actual);
                format_data_line(5, DATA_TYPE_PWM, duty);
                break;
        case PAGE_RPM_CTRL:

                fan_rpm_target = fan_control_get_target_rpm();

                format_data_line(2, DATA_TYPE_VOLT, vfan);
                format_data_line(3, DATA_TYPE_PWM, duty);
                format_data_line(4, DATA_TYPE_RPM, fan_rpm_actual);
                format_data_line(5, DATA_TYPE_RPM, fan_rpm_target);
                break;
        case PAGE_THERMAL_CTRL:

                temp_target = fan_control_get_target_temp();
                temp_actual = fan_control_get_actual_temp();

                format_data_line(2, DATA_TYPE_VOLT, vfan);
                format_data_line(3, DATA_TYPE_PWM, duty);
                format_data_line(4, DATA_TYPE_TEMP, temp_actual);
                format_data_line(5, DATA_TYPE_TEMP, temp_target);
                break;
        case PAGE_SETUP:

                if (save_setting_flag) {
                        if (save_data_ticking >= 1) {
                                save_data_ticking--;
                                break;
                        } else {
                                save_setting_flag = 0;
                                /* back to setup page */
                                enter_page(PAGE_SETUP);
                        }
                }
 
                // page_data[2] = "  PD Volt:     5 "
                page_data[2][14] = setup_subitem_pd_volt[setup_item[0]].name[0];
                page_data[2][15] = setup_subitem_pd_volt[setup_item[0]].name[1];

                // page_data[3] = "  PWM Step:   10 "
                page_data[3][14] = setup_subitem_pwm_step[setup_item[1]].name[0];
                page_data[3][15] = setup_subitem_pwm_step[setup_item[1]].name[1];

                // page_data[4] = "  RPM Step: 1000 "
                page_data[4][12] = setup_subitem_rpm_step[setup_item[2]].name[0];
                page_data[4][13] = setup_subitem_rpm_step[setup_item[2]].name[1];
                page_data[4][14] = setup_subitem_rpm_step[setup_item[2]].name[2];
                page_data[4][15] = setup_subitem_rpm_step[setup_item[2]].name[3];

                // page_data[3] = "  Temp Step:  10 "
                page_data[5][14] = setup_subitem_temp_step[setup_item[3]].name[0];
                page_data[5][15] = setup_subitem_temp_step[setup_item[3]].name[1];

                // setup_point is '>'
                page_data[2][0] = ' ';
                page_data[3][0] = ' ';
                page_data[4][0] = ' ';
                page_data[5][0] = ' ';
                page_data[setup_point+2][0] = '>';
                break;
        case PAGE_SAVE_DATA:
                /* stay in save data page a while to show "Saving data..." */
                if (save_data_ticking >= 1) {
                        save_data_ticking--;
                } else {
                        /* back to setup page */
                        enter_page(PAGE_SETUP);
                }
                break;
        default:
                break;
        }

        /* refresh page */
        ssd1306_show();

        return EC_SUCCESS;
}

/* setup mode */
void setup_next_item(void) {

        setup_point++;
        setup_point = setup_point % SETUP_POINT_COUNT;

        CPRINTS("setup_point: %d", setup_point);
}

/* update setup_item */
void setup_item_change(void) {

        switch (setup_point)
        {
        case SETUP_POINT_0:
                setup_item[0] = (setup_item[0] + 1) % ARRAY_SIZE(setup_subitem_pd_volt);
                break;
        case SETUP_POINT_1:
                setup_item[1] = (setup_item[1] + 1) % ARRAY_SIZE(setup_subitem_pwm_step);
                break;
        case SETUP_POINT_2:
                setup_item[2] = (setup_item[2] + 1) % ARRAY_SIZE(setup_subitem_rpm_step);
                break;
        case SETUP_POINT_3:
                setup_item[3] = (setup_item[3] + 1) % ARRAY_SIZE(setup_subitem_temp_step);
                break;
        default:
                break;
        }
}

void event_trigger(enum event_list event) {

        CPRINTS("event_trigger %d", event);

        switch (event)
        {
        case EVENT_SW_LEFT_SHORT:
                if (current_page != PAGE_SETUP)
                        fan_speed_down();
                else
                        setup_item_change();
                break;
        case EVENT_SW_RIGHT_SHORT:
                if (current_page != PAGE_SETUP)
                        fan_speed_up();
                else
                        setup_next_item();
                break;
        case EVENT_SW_RIGHT_LONG:
                /* go to next page */
                enter_page(current_page + 1);
                break;
        case EVENT_SW_LEFT_LONG:
                if (current_page != PAGE_SETUP) {
                        load_setting();
                } else {
                        save_setting_flag = 1;
                        /* start to count down save data screen */
                        save_data_ticking = SAVE_DATA_SHOW_TICKING;
                        /* show save setting */
                        memcpy(page_data, page_data_setup_save, sizeof(page_data_setup_save));
                        /* save setting */
                        save_setting();
                }
                break;
        case EVENT_FAN_POWER:
                if (current_page == PAGE_SETUP) {
                        CPRINTS("Prevent fan Power enable on setup page");
                        break;
                }

                int state = !gpio_get(GPIO_FAN_DC_CTRL);

                CPRINTS("FAN Power %sable", (state? "en" : "dis"));

                /* enable or disable fan power */
                gpio_set(GPIO_FAN_DC_CTRL, state);

                if (state) {
                        /* led green on */
                        led_update_behavior(LED_GREEN, ARRAY_SIZE(led_on_behavior),
                                (struct led_behavior *) &led_on_behavior);
                } else {
                        /* led green off */
                        led_update_behavior(LED_GREEN, ARRAY_SIZE(led_off_behavior),
                                (struct led_behavior *) &led_off_behavior);
                }
                break;
        default:
                break;
        }
}

static const char *const page_name[] = {
        [PAGE_DUTY_CTRL] = "Duty Control",
        [PAGE_RPM_CTRL] = "PWM Control",
        [PAGE_THERMAL_CTRL] = "Thermal Control",
        [PAGE_SETUP] = "Setup",
};

void enter_page(enum page_list page) {

        /* Do not change page while saving data */
        if(save_setting_flag == 0)
                current_page = page % (PAGE_COUNT - 1);

        CPRINTS("enter page: %s", page_name[current_page]);

        /* always disable fan power if user switch page due to different control mode */
        gpio_set(GPIO_FAN_DC_CTRL, 0);

        /* led green off */
        led_update_behavior(LED_GREEN, ARRAY_SIZE(led_off_behavior),
                (struct led_behavior *) &led_off_behavior);

        /* update fan control mode and prepare page_data */
        switch (current_page)
        {
        case PAGE_DUTY_CTRL:
                fan_set_ctrl_mode(FAN_DUTY_CTRL);
                memcpy(page_data, page_data_control_duty, sizeof(page_data_control_duty));
                break;
        case PAGE_RPM_CTRL:
                fan_set_ctrl_mode(FAN_RPM_CTRL);
                memcpy(page_data, page_data_control_rpm, sizeof(page_data_control_rpm));
                break;
        case PAGE_THERMAL_CTRL:
                fan_set_ctrl_mode(FAN_THERMAL_CTRL);
                memcpy(page_data, page_data_control_thermal, sizeof(page_data_control_thermal));
                break;
        case PAGE_SETUP:
                fan_set_ctrl_mode(FAN_NO_CTRL);
                setup_point = 0;
                memcpy(page_data, page_data_setup, sizeof(page_data_setup));
                break;
        case PAGE_SAVE_DATA:
                memcpy(page_data, page_data_setup_save, sizeof(page_data_setup_save));
                break;
        default:
                break;
        }

        /* update data to page_data immediately */
        page_data_update_loop();
        /* refresh page */
        ssd1306_show();
}

void page_init(void) {

        CPRINTS("init");

        load_setting();

        // default page
        enter_page(PAGE_DUTY_CTRL);

        hook_call_loop(&page_data_update_loop, PAGE_FLASH_DELAY_MS);
}

