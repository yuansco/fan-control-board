
#include <stdint.h>
#include "main.h"
#include "adc.h"
#include "board.h"
#include "button.h"
#include "console.h"
#include "fan.h"
#include "fan_control.h"
#include "gpio.h"
#include "led.h"
#include "task.h"
#include "util.h"
#include "page_control.h"


/* override board_init function */
__override void board_init(void) {

        PRINTF("%s: %s\r\n", BOARDNAME, __func__);

        /* button init */
        button_list[BUTTON_1].name = "SW_LEFT";
        button_list[BUTTON_2].name = "SW_RIGHT";
        button_list[BUTTON_3].name = "SW_FAN_PWR";
        button_list[BUTTON_4].name = "FAN_TACH";
        button_list[BUTTON_4].callback = &board_tach_callback;

        /* adc init */
        // TEMP_AMB - ADC_IN3 (PA3)
        adc_list[ADC_ID1].name = "TEMP_AMB";
        adc_list[ADC_ID1].channel = ADC_CHANNEL_3;

        // TEMP_TARGET - ADC_IN4 (PA4)
        adc_list[ADC_ID2].name = "TEMP_TARGET";
        adc_list[ADC_ID2].channel = ADC_CHANNEL_4;

        // VBUS_MON - ADC_IN5 (PA5)
        adc_list[ADC_ID3].name = "VBUS_MON";
        adc_list[ADC_ID3].channel = ADC_CHANNEL_5;
        adc_list[ADC_ID3].mul = 22000;
        adc_list[ADC_ID3].div = 4096;

        // FAN_PWR_MON - ADC_IN11 (PB10)
        adc_list[ADC_ID4].name = "FAN_PWR_MON";
        adc_list[ADC_ID4].channel = ADC_CHANNEL_11;
        adc_list[ADC_ID4].mul = 22000;
        adc_list[ADC_ID4].div = 4096;

        /* board command init */
        command[COMMAND_BOARD_COMMAND] = command_board_define;

        /* page init */
        page_init();

        /* FAN control init */
        fan_control_init();
}

/* override gpio_list */
struct gpio gpio_list[GPIO_COUNT] = {
        /* PB5 */
        [GPIO_SW_LEFT] = {
                .name = "SW_LEFT",
                .mode = IO_MODE_INPUT,
                .port = SW_LEFT_GPIO_Port,
                .pin = SW_LEFT_Pin,
        },
        /* PB4 */
        [GPIO_SW_RIGHT] = {
                .name = "SW_RIGHT",
                .mode = IO_MODE_INPUT,
                .port = SW_RIGHT_GPIO_Port,
                .pin = SW_RIGHT_Pin,
        },
        /* PA12 */
        [GPIO_SW_FAN_PWR] = {
                .name = "SW_FAN_PWR",
                .mode = IO_MODE_INPUT,
                .port = SW_FAN_PWR_GPIO_Port,
                .pin = SW_FAN_PWR_Pin,
        },
        /* PA10 */
        [GPIO_SW_UP] = {
                .name = "SW_UP",
                .mode = IO_MODE_INPUT,
                .port = SW_UP_GPIO_Port,
                .pin = SW_UP_Pin,
        },
        /* PA9 */
        [GPIO_SW_DOWN] = {
                .name = "SW_DOWN",
                .mode = IO_MODE_INPUT,
                .port = SW_DOWN_GPIO_Port,
                .pin = SW_DOWN_Pin,
        },
        /* PA8 */
        [GPIO_SW_START_STOP] = {
                .name = "SW_START_STOP",
                .mode = IO_MODE_INPUT,
                .port = SW_START_STOP_GPIO_Port,
                .pin = SW_START_STOP_Pin,
        },
        /* PA0 */
        [GPIO_CH224K_CFG1] = {
                .name = "CH224K_CFG1",
                .mode = IO_MODE_OUTPUT,
                .port = CH224K_CFG1_GPIO_Port,
                .pin = CH224K_CFG1_Pin,
        },
        /* PA1 */
        [GPIO_CH224K_CFG2] = {
                .name = "CH224K_CFG2",
                .mode = IO_MODE_OUTPUT,
                .port = CH224K_CFG2_GPIO_Port,
                .pin = CH224K_CFG2_Pin,
        },
        /* PA2 */
        [GPIO_CH224K_CFG3] = {
                .name = "CH224K_CFG3",
                .mode = IO_MODE_OUTPUT,
                .port = CH224K_CFG3_GPIO_Port,
                .pin = CH224K_CFG3_Pin,
        },
        /* PA6 */
        // [GPIO_FAN_PWM] = {
        //         .name = "FAN_PWM",
        //         .mode = IO_MODE_OUTPUT,
        //         .port = FAN_PWM_GPIO_Port,
        //         .pin = FAN_PWM_Pin,
        // },
        /* PA7 */
        [GPIO_FAN_TACH] = {
                .name = "FAN_TACH",
                .mode = IO_MODE_INPUT,
                .port = FAN_TACH_GPIO_Port,
                .pin = FAN_TACH_Pin,
        },
        /* PB0 */
        [GPIO_FAN_DC_CTRL] = {
                .name = "FAN_DC_CTRL",
                .mode = IO_MODE_OUTPUT,
                .port = FAN_DC_CTRL_GPIO_Port,
                .pin = FAN_DC_CTRL_Pin,
        },
        /* PB1 */
        [GPIO_FAN2_DC_CTR] = {
                .name = "FAN2_DC_CTR",
                .mode = IO_MODE_OUTPUT,
                .port = FAN2_DC_CTR_GPIO_Port,
                .pin = FAN2_DC_CTR_Pin,
        },
};

__override void board_error(int e) {
        led_update_behavior(LED_RED, ARRAY_SIZE(led_blinking_behavior),
                        (struct led_behavior *) &led_blinking_behavior);
}

