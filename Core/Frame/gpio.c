
#include "board.h"
#include "button.h"
#include "gpio.h"
#include "main.h"
#include "console.h"
#include "util.h"

/******************************************************************************/
/* GPIO */

#ifdef CONFIG_GPIO


#ifdef CONFIG_GPIO_DEBUG
#define CPRINTF(format, args...) PRINTF("GPIO: " format, ##args)
#define CPRINTS(format, args...) PRINTS("GPIO: " format, ##args)
#else
#define CPRINTF(format, args...)
#define CPRINTS(format, args...)
#endif


int gpio_set(enum gpio_id id, int state) {

        HAL_GPIO_WritePin(gpio_list[id].port, gpio_list[id].pin, state);
        return EC_SUCCESS;
}

int gpio_get(enum gpio_id id) {
        return HAL_GPIO_ReadPin(gpio_list[id].port, gpio_list[id].pin);
}

int gpio_toggle(enum gpio_id id) {

        HAL_GPIO_TogglePin(gpio_list[id].port, gpio_list[id].pin);
        return EC_SUCCESS;
}

#ifndef CONFIG_GPIO_LIST
struct gpio gpio_list[] = {
        {
                .name = "PC13",
                .mode = IO_MODE_OUTPUT,
                .port = PC13_GPIO_Port,
                .pin = PC13_Pin,
        }
};
#endif /* CONFIG_GPIO_LIST */

int gpio_init(void) {

        CPRINTS("init");

        gpio_print();

        return EC_SUCCESS;
}

/******************************************************************************/
/* GPIO interrupt */

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin) {

#ifdef CONFIG_BUTTON
        button_interrupt_callback(GPIO_Pin);
#endif

}

void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin) {

#ifdef CONFIG_BUTTON
        button_interrupt_callback(GPIO_Pin);
#endif

}


/******************************************************************************/
/* GPIO console command */

#ifdef CONFIG_GPIO_GET_COMMAND

static const char *const gpio_mode_name[] = {
        [IO_MODE_INPUT] = "IN",
        [IO_MODE_OUTPUT] = "OUT",
};

void gpio_print(void) {

        CPRINTS("GPIO info:");

        CPRINTS("   id           GPIO   mode   state");

        for (int id = 0; id < GPIO_COUNT; id++) {
                CPRINTS("   %2d  %13s    %3s       %d",
                        id,
                        gpio_list[id].name,
                        gpio_mode_name[gpio_list[id].mode],
                        gpio_get(id));
        }
}

#endif

#endif /* CONFIG_LED */
