
#ifndef __GPIO_H__
#define __GPIO_H__

#include <stdint.h>
#include "task.h"

/* should define in board.h if needed */
//#define CONFIG_GPIO
//#define CONFIG_GPIO_LIST
//#define CONFIG_GPIO_DEBUG
//#define CONFIG_GPIO_COMMAND


/* gpio list */
#ifndef CONFIG_GPIO_LIST
enum gpio_id {
        GPIO_PC13 = 0,
        GPIO_COUNT
};
#endif /* CONFIG_GPIO_LIST */

/* gpio mode list */
enum gpio_mode {
        IO_MODE_INPUT = 0,
        IO_MODE_OUTPUT
};

enum gpio_state {
        GPIO_OFF = 0,
        GPIO_ON,
};

/* gpio struct */
struct gpio {
        /* gpio name */
        const char *name;
        /* gpio mode (auto or manual) */
        enum gpio_mode mode;
        /* gpio port */
        void *port;      // (GPIO_TypeDef *)
        /* gpio pin */
        uint16_t pin;
};

extern struct gpio gpio_list[];

int gpio_get(enum gpio_id id);
int gpio_toggle(enum gpio_id id);
int gpio_set(enum gpio_id id, int state);
int gpio_command_set(enum gpio_id id, int state);
int gpio_command_reset(enum gpio_id id);

void gpio_print(void);
int gpio_init(void);

#endif /* __GPIO_H__ */
