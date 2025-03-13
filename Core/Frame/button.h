
#ifndef __BUTTON_H__
#define __BUTTON_H__

#include <stdint.h>

/* should define in board.h if needed */
// #define CONFIG_BUTTON
// #define CONFIG_BUTTON_DEBUG
// #define CONFIG_BUTTON_COMMAND
// /* Button pin define */
// #define BUTTON_1_Pin GPIO_PIN_6
// #define BUTTON_1_GPIO_Port GPIOC
// #define BUTTON_1_EXTI_IRQn EXTI4_15_IRQn
// #define BUTTON_2_Pin GPIO_PIN_7
// #define BUTTON_2_GPIO_Port GPIOC
// #define BUTTON_2_EXTI_IRQn EXTI4_15_IRQn

/* button list */
enum button_id {
        BUTTON_1 = 0,
        BUTTON_2,
        BUTTON_3,
        BUTTON_4,
        BUTTON_COUNT
};

/* led struct */
struct button {
        /* button name */
        const char *name;
        /* button led port */
        void *port;           // (GPIO_TypeDef *)
        /* button pin */
        const uint16_t pin;
        /* button debounce time (ms) */
        int debounce_ms;
        /* callback function */
        int (*callback)(void);
};

extern struct button button_list[BUTTON_COUNT];

void button_print(void);
int button_get(enum button_id id);
void button_interrupt_callback(uint16_t GPIO_Pin);

#endif /* __BUTTON_H__ */