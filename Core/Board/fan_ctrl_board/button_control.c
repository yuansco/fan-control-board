#include <stdint.h>
#include "main.h"
#include "board.h"
#include "button.h"
#include "console.h"
#include "fan.h"
#include "gpio.h"
#include "task.h"
#include "util.h"
#include "page_control.h"
#include "led.h"

/* Button Control */

#define LONG_PRESS_HOLD_TIME_MS 1000
#define BUTTON_HOLD_SAMPLE_TIME_MS 5

static int button_1_hold_time;

/* SW_LEFT short & long press detect */
int button_1_hold_deferred(void) {

        int state = button_get(BUTTON_1);

        if (state == 0) {
                button_1_hold_time++;
                if (button_1_hold_time >= \
                (LONG_PRESS_HOLD_TIME_MS - button_list[BUTTON_1].debounce_ms)/BUTTON_HOLD_SAMPLE_TIME_MS) {
                        PRINTF("%s long press\r\n", button_list[BUTTON_1].name);
                        // long press
                        button_1_hold_time = 0;
                        event_trigger(EVENT_SW_LEFT_LONG);
                        return EC_SUCCESS;
                }
                hook_call_deferred(&button_1_hold_deferred, BUTTON_HOLD_SAMPLE_TIME_MS);
                return EC_SUCCESS;
        } else {
                // short press
                PRINTF("%s short press\r\n", button_list[BUTTON_1].name);
                event_trigger(EVENT_SW_LEFT_SHORT);
        }

        button_1_hold_time = 0;

        return EC_SUCCESS;
}

/* override button 1 behavior - SW_LEFT */
__override int board_button_1_deferred(int state) {

        PRINTF("%s %s\r\n", button_list[BUTTON_1].name, (state? "released" : "pressed"));

        if (state == 0) {
                hook_call_deferred(&button_1_hold_deferred, BUTTON_HOLD_SAMPLE_TIME_MS);
        }

        return EC_SUCCESS;
}

static int button_2_hold_time;

/* SW_RIGHT short & long press detect */
int button_2_hold_deferred(void) {

        int state = button_get(BUTTON_2);

        if (state == 0) {
                button_2_hold_time++;
                if (button_2_hold_time >= \
                (LONG_PRESS_HOLD_TIME_MS - button_list[BUTTON_2].debounce_ms)/BUTTON_HOLD_SAMPLE_TIME_MS) {
                        PRINTF("%s long press\r\n", button_list[BUTTON_2].name);
                        // long press
                        button_2_hold_time = 0;
                        event_trigger(EVENT_SW_RIGHT_LONG);
                        return EC_SUCCESS;
                }
                hook_call_deferred(&button_2_hold_deferred, BUTTON_HOLD_SAMPLE_TIME_MS);
                return EC_SUCCESS;
        } else {
                // short press
                PRINTF("%s short press\r\n", button_list[BUTTON_2].name);
                event_trigger(EVENT_SW_RIGHT_SHORT);
        }

        button_2_hold_time = 0;

        return EC_SUCCESS;
}

/* override button 2 behavior - SW_RIGHT */
__override int board_button_2_deferred(int state) {

        PRINTF("%s %s\r\n", button_list[BUTTON_2].name, (state? "released" : "pressed"));

        if (state == 0) {
                hook_call_deferred(&button_2_hold_deferred, BUTTON_HOLD_SAMPLE_TIME_MS);
        }

        return EC_SUCCESS;
}

/* override button 3 behavior - SW_FAN_PWR */
__override int __board_button_3_deferred(int state) {

        PRINTF("SW_FAN_PWR %s\r\n", (state? "on" : "off"));

        gpio_set(GPIO_FAN_DC_CTRL, state);

        PRINTF("FAN_DC_CTRL %sable\r\n", (state? "en" : "dis"));

        if (state == 1) {
                /* led green on */
                led_update_behavior(LED_GREEN, ARRAY_SIZE(led_on_behavior),
                        (struct led_behavior *) &led_on_behavior);
        } else {
                /* led green off */
                led_update_behavior(LED_GREEN, ARRAY_SIZE(led_off_behavior),
                        (struct led_behavior *) &led_off_behavior);
        }

        return EC_SUCCESS;
}

/* override button 3 behavior - SW_FAN_PWR */
__override int board_button_3_deferred(int state) {

        if (state == 0) {
                event_trigger(EVENT_FAN_POWER);
        }

        return EC_SUCCESS;
}

/******************************************************************************/
/* button console command */

static const char *const button_event_name[] = {
        [EVENT_SW_LEFT_SHORT] = "left button short press",
        [EVENT_SW_LEFT_LONG] = "left button long press",
        [EVENT_SW_RIGHT_SHORT] = "right button short press",
        [EVENT_SW_RIGHT_LONG] = "right button long press",
        [EVENT_FAN_POWER] = "fan power press",
};


#ifdef CONFIG_CONSOLE_BOARD_COMMAND
/* override board console command */
int command_button(int argc, const char **argv) {

        char *e;
        int event_id;

        if (argc == 1) {

                PRINTF(" ID  Event\r\n");
                for(int i=0; i<EVENT_COUNT; i++)
                        PRINTF("  %d  %s\r\n", i, button_event_name[i]);

                return EC_SUCCESS;
        }

        if (argc != 2)
                return EC_ERROR_INVAL;

        event_id = strtoi(argv[1], &e, 10);

        if (*e || event_id < 0 || event_id >= EVENT_COUNT)
                return EC_ERROR_INVAL;

        event_trigger(event_id);
                
        return EC_SUCCESS;
}

struct console_command command_board_define = {
        .name = "button",
        .arg_desc = "button [event id]",
        .depiction = "Trigger a simulate button event",
        .handler = &command_button,
};

#endif /* CONFIG_CONSOLE_BOARD_COMMAND */


