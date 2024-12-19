
#ifndef __BOARD_H
#define __BOARD_H

/* fan ctrl board */

#define BOARDNAME "fan_ctrl_board"


/**
 * Support debug print and console command
 */

#define CONFIG_UART_DEBUG
#define CONFIG_CONSOLE_COMMAND
#define CONFIG_CONSOLE_BOARD_COMMAND

/**
 * Support Task
 */

#define CONFIG_TASK
#define CONFIG_TASK_DEBUG
#define CONFIG_TASK_HOOK
//#define CONFIG_TASK_HOOK_TEST
#define CONFIG_TASK_HOOK_COMMAND

/**
 * Support GPIO
 */

#define CONFIG_GPIO
#define CONFIG_GPIO_LIST
#define CONFIG_GPIO_DEBUG
#define CONFIG_GPIO_GET_COMMAND
#define CONFIG_GPIO_SET_COMMAND

enum gpio_id {
        GPIO_SW_LEFT = 0,
        GPIO_SW_RIGHT,
        GPIO_SW_FAN_PWR,
        GPIO_SW_UP,
        GPIO_SW_DOWN,
        GPIO_SW_START_STOP,
        GPIO_CH224K_CFG1,
        GPIO_CH224K_CFG2,
        GPIO_CH224K_CFG3,
        //GPIO_FAN_PWM,
        GPIO_FAN_TACH,
        GPIO_FAN_DC_CTRL,
        GPIO_FAN2_DC_CTR,
        GPIO_COUNT
};


/**
 * LED
 */
#define CONFIG_LED
#define CONFIG_LED_DEBUG
#define CONFIG_LED_HOOK
#define CONFIG_LED_COMMAND
#define CONFIG_LED_RED
#define CONFIG_LED_YELLOW
#define CONFIG_LED_GREEN
/* LED pin define */
#define LED_RED_Pin LED_ERR_Pin
#define LED_RED_GPIO_Port LED_ERR_GPIO_Port
#define LED_YELLOW_Pin LED_WORK_Pin
#define LED_YELLOW_GPIO_Port LED_WORK_GPIO_Port
#define LED_GREEN_Pin LED_FAN_PWR_PG_Pin
#define LED_GREEN_GPIO_Port LED_FAN_PWR_PG_GPIO_Port

/**
 *  Support I2C function
 * 
 *  Support I2C read, write, scan function.
 */
#define CONFIG_I2C
#define CONFIG_I2C_DEBUG
#define CONFIG_I2C_READ_COMMAND
#define CONFIG_I2C_WRITE_COMMAND
#define CONFIG_I2C_SCAN

/**
 * EEPROM AT24C02
 */

#define CONFIG_EEPROM_AT24C02
#define CONFIG_EEPROM_AT24C02_DEBUG
#define CONFIG_EEPROM_AT24C02_COMMAND
// #define CONFIG_EEPROM_AT24C02_SELF_TEST


/**
 * Button
 */
#define CONFIG_BUTTON
#define CONFIG_BUTTON_DEBUG
#define CONFIG_BUTTON_COMMAND
/* Button pin define */
#define BUTTON_1_Pin SW_LEFT_Pin
#define BUTTON_1_GPIO_Port SW_LEFT_GPIO_Port
#define BUTTON_2_Pin SW_RIGHT_Pin
#define BUTTON_2_GPIO_Port SW_RIGHT_GPIO_Port
#define BUTTON_3_Pin SW_FAN_PWR_Pin
#define BUTTON_3_GPIO_Port SW_FAN_PWR_GPIO_Port
#define BUTTON_4_Pin FAN_TACH_Pin
#define BUTTON_4_GPIO_Port FAN_TACH_GPIO_Port

/**
 * PD
 */
#define CONFIG_PD_CH224K
#define CONFIG_PD_CH224K_DEBUG
#define CONFIG_PD_CH224K_COMMAND
#define CONFIG_PD_CH224K_ERR_DETECT
/* PD config pin define */
#define CH224K_CFG_IO1 GPIO_CH224K_CFG1
#define CH224K_CFG_IO2 GPIO_CH224K_CFG2
#define CH224K_CFG_IO3 GPIO_CH224K_CFG3

/**
 * ADC
 */
#define CONFIG_ADC
#define CONFIG_ADC_DEBUG
#define CONFIG_ADC_COMMAND
/* ADC channel name define */
enum adc_ch {
        TEMP_AMB = 0,
        TEMP_TARGET,
        VBUS_MON,
        FAN_PWR_MON,
        ADC_CH_COUNT
};

/**
 * FAN
 */
#define CONFIG_FAN
#define CONFIG_FAN_DEBUG
// #define CONFIG_FAN_PWM_TEST
#define CONFIG_FAN_COMMAND

/**
 * OLED SSD1306
 */

#define SSD1306_OLED
#define SSD1306_USE_I2C

// I2C Configuration
#define SSD1306_I2C_PORT        hi2c1
#define SSD1306_I2C_ADDR        (0x3C << 1)

// Mirror the screen if needed
// #define SSD1306_MIRROR_VERT
// #define SSD1306_MIRROR_HORIZ

// Set inverse color if needed
// # define SSD1306_INVERSE_COLOR

// Include only needed fonts
//#define SSD1306_INCLUDE_FONT_6x8
#define SSD1306_INCLUDE_FONT_7x10
//#define SSD1306_INCLUDE_FONT_11x18
//#define SSD1306_INCLUDE_FONT_16x26

// Some OLEDs don't display anything in first two columns.
// In this case change the following macro to 130.
// The default value is 128.
// #define SSD1306_WIDTH           130

// The height can be changed as well if necessary.
// It can be 32, 64 or 128. The default value is 64.
// #define SSD1306_HEIGHT          32

/**
 * Thermistors
 */
#define CONFIG_THERMISTORS


/**
 * High-Speed External Oscillator
 * 
 */
#define OSC_16M

#undef HSE_VALUE 
#ifdef OSC_16M
#define HSE_VALUE    (16000000UL)  
#else
#define HSE_VALUE    (8000000UL)   
#endif


#endif /* __BOARD_H */

