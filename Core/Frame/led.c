
#include "board.h"
#include "led.h"
#include "main.h"
#include "task.h"
#include "console.h"
#include "util.h"

/******************************************************************************/
/* LED */

#ifdef CONFIG_LED


#ifdef CONFIG_LED_DEBUG
#define CPRINTF(format, args...) PRINTF("LED: " format, ##args)
#define CPRINTS(format, args...) PRINTS("LED: " format, ##args)
#else
#define CPRINTF(format, args...)
#define CPRINTS(format, args...)
#endif


int led_on(enum led_id id) {

        HAL_GPIO_WritePin(led_list[id].port, led_list[id].pin,
                ((led_list[id].invert) ? GPIO_PIN_RESET : GPIO_PIN_SET));

        return EC_SUCCESS;
}

int led_off(enum led_id id) {

        HAL_GPIO_WritePin(led_list[id].port, led_list[id].pin,
                ((led_list[id].invert) ? GPIO_PIN_SET : GPIO_PIN_RESET));

        return EC_SUCCESS;
}

int led_toggle(enum led_id id) {
        HAL_GPIO_TogglePin(led_list[id].port, led_list[id].pin);
        return EC_SUCCESS;
}

int led_set(enum led_id id, int state) {

        if (led_list[id].invert)
                state = !state;

        HAL_GPIO_WritePin(led_list[id].port, led_list[id].pin, state);
        return EC_SUCCESS;
}

int led_get(enum led_id id) {
        return HAL_GPIO_ReadPin(led_list[id].port, led_list[id].pin);
}

int led_command_set(enum led_id id, int state) {
        led_list[id].mode = LED_MODE_MANUAL;
        led_set(id, !!(state));
        return EC_SUCCESS;
}

int led_command_reset(enum led_id id) {
        led_list[id].mode = LED_MODE_AUTO;
        return EC_SUCCESS;
}

/* led behavior: off */
struct led_behavior led_off_behavior[] = {
        {
                .behavior = &led_off,
                .time_ms = 500,
        },
        {
                .behavior = &led_off,
                .time_ms = 500,
        }
};
const int LED_OFF_STEP = ARRAY_SIZE(led_off_behavior);

/* led behavior: on */
struct led_behavior led_on_behavior[] = {
        {
                .behavior = &led_on,
                .time_ms = 500,
        },
        {
                .behavior = &led_on,
                .time_ms = 500,
        }
};
const int LED_ON_STEP = ARRAY_SIZE(led_on_behavior);

/* led behavior: blinking 1 sec on / 1 sec off */
struct led_behavior led_blinking_behavior[] = {
        {
                .behavior = &led_on,
                .time_ms = 1000,
        },
        {
                .behavior = &led_off,
                .time_ms = 1000,
        },
};
const int LED_BLINKING_STEP = ARRAY_SIZE(led_blinking_behavior);


struct led led_list[] = {
#ifdef CONFIG_LED_BLUE
        [LED_BLUE] = {
                .name = "blue",
                .mode = LED_MODE_AUTO,
                .port = LED_BLUE_GPIO_Port,
                .pin = LED_BLUE_Pin,
                .invert = TRUE,
                .step = LED_OFF_STEP,
                .list = (struct led_behavior *) &led_off_behavior,
        },
#endif /* CONFIG_LED_BLUE */
#ifdef CONFIG_LED_RED
        [LED_RED] = {
                .name = "red",
                .mode = LED_MODE_AUTO,
                .port = LED_RED_GPIO_Port,
                .pin = LED_RED_Pin,
                .invert = TRUE,
                .step = LED_OFF_STEP,
                .list = (struct led_behavior *) &led_off_behavior,
        },
#endif /* CONFIG_LED_RED */
#ifdef CONFIG_LED_YELLOW
        [LED_YELLOW] = {
                .name = "yellow",
                .mode = LED_MODE_AUTO,
                .port = LED_YELLOW_GPIO_Port,
                .pin = LED_YELLOW_Pin,
                .invert = TRUE,
                .step = LED_OFF_STEP,
                .list = (struct led_behavior *) &led_off_behavior,
        },
#endif /* CONFIG_LED_YELLOW */
#ifdef CONFIG_LED_GREEN
        [LED_GREEN] = {
                .name = "green",
                .mode = LED_MODE_AUTO,
                .port = LED_GREEN_GPIO_Port,
                .pin = LED_GREEN_Pin,
                .invert = TRUE,
                .step = LED_OFF_STEP,
                .list = (struct led_behavior *) &led_off_behavior,
        },
#endif /* CONFIG_LED_GREEN */

};

#ifdef CONFIG_LED_HOOK

#ifdef CONFIG_LED_BLUE
int update_blue_led = FALSE;

int led_blue_deferred(void) {

        static int i;

        if(update_blue_led) {
                update_blue_led = FALSE;
                i = 0;
        }

        if (led_list[LED_BLUE].mode == LED_MODE_AUTO)
                led_list[LED_BLUE].list[i].behavior(LED_BLUE);

        hook_call_deferred(&led_blue_deferred, led_list[LED_BLUE].list[i++].time_ms);

        if (i >= led_list[LED_BLUE].step)
                i = 0;

        return EC_SUCCESS;
}
#endif /* CONFIG_LED_BLUE */

#ifdef CONFIG_LED_RED
int update_red_led = FALSE;

int led_red_deferred(void) {

        static int i;

        if(update_red_led) {
                update_red_led = FALSE;
                i = 0;
        }

        if (led_list[LED_RED].mode == LED_MODE_AUTO)
                led_list[LED_RED].list[i].behavior(LED_RED);

        hook_call_deferred(&led_red_deferred, led_list[LED_RED].list[i++].time_ms);

        if (i >= led_list[LED_RED].step)
                i = 0;

        return EC_SUCCESS;
}
#endif /* CONFIG_LED_RED */

#ifdef CONFIG_LED_YELLOW
int update_yellow_led = FALSE;

int led_yellow_deferred(void) {

        static int i;

        if(update_yellow_led) {
                update_yellow_led = FALSE;
                i = 0;
        }

        if (led_list[LED_YELLOW].mode == LED_MODE_AUTO)
                led_list[LED_YELLOW].list[i].behavior(LED_YELLOW);

        hook_call_deferred(&led_yellow_deferred, led_list[LED_YELLOW].list[i++].time_ms);

        if (i >= led_list[LED_YELLOW].step)
                i = 0;

        return EC_SUCCESS;
}
#endif /* CONFIG_LED_YELLOW */


#ifdef CONFIG_LED_GREEN
int update_green_led = FALSE;

int led_green_deferred(void) {

        static int i;

        if(update_green_led) {
                update_green_led = FALSE;
                i = 0;
        }

        if (led_list[LED_GREEN].mode == LED_MODE_AUTO)
                led_list[LED_GREEN].list[i].behavior(LED_GREEN);

        hook_call_deferred(&led_green_deferred, led_list[LED_GREEN].list[i++].time_ms);

        if (i >= led_list[LED_GREEN].step)
                i = 0;

        return EC_SUCCESS;
}
#endif /* CONFIG_LED_GREEN */

#endif /* CONFIG_LED_HOOK */

void led_update_behavior(enum led_id id, int step, struct led_behavior *list) {

        if (id >= LED_COUNT || id < 0) {
                CPRINTS("%s: error led_id", __func__);
                return;
        }

        led_list[id].step = step;
        led_list[id].list = list;

        switch (id)
        {
#ifdef CONFIG_LED_BLUE
        case LED_BLUE:
                update_blue_led = TRUE;
                break;
#endif /* CONFIG_LED_BLUE */
#ifdef CONFIG_LED_RED
        case LED_RED:
                update_red_led = TRUE;
                break;
#endif /* CONFIG_LED_RED */
#ifdef CONFIG_LED_YELLOW
        case LED_YELLOW:
                update_yellow_led = TRUE;
                break;
#endif /* CONFIG_LED_YELLOW */
        default:
                break;
        }
}

int led_init(void) {

        int id;

        CPRINTS("init");

        for (id = 0; id < ARRAY_SIZE(led_list); id++)
                led_off(id);

        led_print();

#ifdef CONFIG_LED_HOOK
#ifdef CONFIG_LED_BLUE
        hook_call_deferred(&led_blue_deferred, 0);
#endif /* CONFIG_LED_BLUE */
#ifdef CONFIG_LED_RED
        hook_call_deferred(&led_red_deferred, 0);
#endif /* CONFIG_LED_RED */
#ifdef CONFIG_LED_YELLOW
        hook_call_deferred(&led_yellow_deferred, 0);
#endif /* CONFIG_LED_YELLOW */
#ifdef CONFIG_LED_GREEN
        hook_call_deferred(&led_green_deferred, 0);
#endif /* CONFIG_LED_GREEN */
#endif /* CONFIG_LED_HOOK */

        return EC_SUCCESS;
}


/******************************************************************************/
/* LED console command */

#ifdef CONFIG_LED_COMMAND

static const char *const led_mode_name[] = {
        [LED_MODE_AUTO] = "LED_MODE_AUTO",
        [LED_MODE_MANUAL] = "LED_MODE_MANUAL",
};

void led_print(void) {

        int id;

        CPRINTS("LED info:");

        for (id = 0; id < ARRAY_SIZE(led_list); id++) {
                CPRINTS(" [LED ID: %d]\r\n    name: %s\r\n    mode: %s\r\n    step: %d\r\n    state: %d", id,
                        led_list[id].name,
                        led_mode_name[led_list[id].mode],
                        led_list[id].step,
                        led_get(id));
        }
}

#endif

#endif /* CONFIG_LED */
