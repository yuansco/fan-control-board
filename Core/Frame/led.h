
#ifndef __LED_H__
#define __LED_H__

#include <stdint.h>
#include "task.h"

/* should define in board.h if needed */
//#define CONFIG_LED
//#define CONFIG_LED_DEBUG
//#define CONFIG_LED_HOOK
//#define CONFIG_LED_COMMAND
//#define CONFIG_LED_BLUE
//#define CONFIG_LED_RED
//#define CONFIG_LED_YELLOW
//#define CONFIG_LED_GREEN


/* led list */
enum led_id {
#ifdef CONFIG_LED_BLUE
        LED_BLUE,
#endif
#ifdef CONFIG_LED_RED
        LED_RED,
#endif
#ifdef CONFIG_LED_YELLOW
        LED_YELLOW,
#endif
#ifdef CONFIG_LED_GREEN
        LED_GREEN,
#endif
        LED_COUNT
};

/* led mode list */
enum led_mode {
        LED_MODE_AUTO = 0,
        LED_MODE_MANUAL,
        LED_MODE_COUNT
};

enum led_state {
        LED_ON = 0,
        LED_OFF,
};

struct led_behavior {
        int time_ms;
        int (*behavior) (enum led_id id);
} __packed;

/* led struct */
struct led {
        /* led name */
        const char *name;
        /* led mode (auto or manual) */
        enum led_mode mode;
        /* led port */
        void *port;      // (GPIO_TypeDef *)
        /* led pin */
        uint16_t pin;
        /* led invert */
        /* invert 1: gpio set low to turn on LED */
        /* invert 0: gpio set high to turn on LED */
        uint8_t invert;
        /* led behavior step */
        int step;
        /* led behavior */
        struct led_behavior *list;
};

#define LED_ON  1
#define LED_OFF 0

extern struct led led_list[LED_COUNT];


int led_on(enum led_id id);
int led_off(enum led_id id);
int led_toggle(enum led_id id);
int led_get(enum led_id id);

int led_set(enum led_id id, int state);
int led_command_set(enum led_id id, int state);
int led_command_reset(enum led_id id);
void led_update_behavior(enum led_id id, int step, struct led_behavior *list);

void led_print(void);
int led_init(void);

extern struct led_behavior led_off_behavior[2];
extern struct led_behavior led_on_behavior[2];
extern struct led_behavior led_blinking_behavior[2];

#endif /* __LED_H__ */
