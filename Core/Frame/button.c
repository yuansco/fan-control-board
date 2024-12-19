
#include "button.h"
#include "console.h"
#include "task.h"
#include "main.h"
#include "util.h"


/******************************************************************************/
/* Button */

#ifdef CONFIG_BUTTON

#ifdef CONFIG_BUTTON_DEBUG
#define CPRINTF(format, args...) PRINTF("BUTTON: " format, ##args)
#else
#define CPRINTF(format, args...) 
#endif

int button_get(enum button_id id) {
        return HAL_GPIO_ReadPin(button_list[id].port, button_list[id].pin);
}

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin) {

        if (GPIO_Pin == button_list[BUTTON_1].pin)
                button_list[BUTTON_1].callback();
        else if (GPIO_Pin == button_list[BUTTON_2].pin)
                button_list[BUTTON_2].callback();
        else if (GPIO_Pin == button_list[BUTTON_3].pin)
                button_list[BUTTON_3].callback();
        else if (GPIO_Pin == button_list[BUTTON_4].pin)
                button_list[BUTTON_4].callback();
        else
                return;
}

void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin) {

        if (GPIO_Pin == button_list[BUTTON_1].pin)
                button_list[BUTTON_1].callback();
        else if (GPIO_Pin == button_list[BUTTON_2].pin)
                button_list[BUTTON_2].callback();
        else if (GPIO_Pin == button_list[BUTTON_3].pin)
                button_list[BUTTON_3].callback();
        else if (GPIO_Pin == button_list[BUTTON_4].pin)
                button_list[BUTTON_4].callback();
        else
                return;
}

static uint8_t button_1_debounce = 0;
static uint8_t button_2_debounce = 0;
static uint8_t button_3_debounce = 0;
static uint8_t button_4_debounce = 0;

__overridable void board_button_1_deferred(int state)
{
}

int button_1_deferred(void) {

        //PRINTF("button 1 state: %d\r\n", button_get(BUTTON_1));
        board_button_1_deferred(button_get(BUTTON_1));
        button_1_debounce = 0;
        return EC_SUCCESS;
}

int button_1_callback(void) {

        if (!button_1_debounce) {
                hook_call_deferred(&button_1_deferred, button_list[BUTTON_1].debounce_ms);
                button_1_debounce = 1;
        }
        return EC_SUCCESS;
}

__overridable void board_button_2_deferred(int state)
{
}

int button_2_deferred(void) {

        //PRINTF("button 2 state: %d\r\n", button_get(BUTTON_2));
        board_button_2_deferred(button_get(BUTTON_2));
        button_2_debounce = 0;
        return EC_SUCCESS;
}

int button_2_callback(void) {

        if (!button_2_debounce) {
                hook_call_deferred(&button_2_deferred, button_list[BUTTON_2].debounce_ms);
                button_2_debounce = 1;
        }
        return EC_SUCCESS;
}

__overridable void board_button_3_deferred(int state)
{
}

int button_3_deferred(void) {

        //PRINTF("button 2 state: %d\r\n", button_get(BUTTON_2));
        board_button_3_deferred(button_get(BUTTON_3));
        button_3_debounce = 0;
        return EC_SUCCESS;
}

int button_3_callback(void) {

        if (!button_3_debounce) {
                hook_call_deferred(&button_3_deferred, button_list[BUTTON_3].debounce_ms);
                button_3_debounce = 1;
        }
        return EC_SUCCESS;
}

__overridable void board_button_4_deferred(int state)
{
}

int button_4_deferred(void) {

        board_button_4_deferred(button_get(BUTTON_4));
        button_4_debounce = 0;
        return EC_SUCCESS;
}

int button_4_callback(void) {

        if (!button_4_debounce) {
                hook_call_deferred(&button_4_deferred, button_list[BUTTON_4].debounce_ms);
                button_4_debounce = 1;
        }
        return EC_SUCCESS;
}

struct button button_list[] = {
        [BUTTON_1] = {
                .name = "button 1",
                .port = BUTTON_1_GPIO_Port,
                .pin = BUTTON_1_Pin,
                .debounce_ms = 200,
                .callback = &button_1_callback,
        },
        [BUTTON_2] = {
                .name = "button 2",
                .port = BUTTON_2_GPIO_Port,
                .pin = BUTTON_2_Pin,
                .debounce_ms = 200,
                .callback = &button_2_callback,
        },
        [BUTTON_3] = {
                .name = "button 3",
                .port = BUTTON_3_GPIO_Port,
                .pin = BUTTON_3_Pin,
                .debounce_ms = 200,
                .callback = &button_3_callback,
        },
        [BUTTON_4] = {
                .name = "button 4",
                .port = BUTTON_4_GPIO_Port,
                .pin = BUTTON_4_Pin,
                .debounce_ms = 200,
                .callback = &button_4_callback,
        },
};


/******************************************************************************/
/* button console command */

#ifdef CONFIG_BUTTON_COMMAND
void button_print(void) {

        int id;

        PRINTF("\r\nButton info:\r\n");

        for (id = 0; id < ARRAY_SIZE(button_list); id++) {
                PRINTF(" Button %d\tname: %s\t\tstate: %d\r\n", id,
                        button_list[id].name,
                        button_get(id));
        }
}

#endif /* CONFIG_BUTTON_COMMAND */

#endif /* CONFIG_BUTTON */

