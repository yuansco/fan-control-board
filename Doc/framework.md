
## Framework function selection

The following configuration should be defined in board.h if needed.

Support debug print and console command:
```c
#define CONFIG_UART_DEBUG
#define CONFIG_CONSOLE_COMMAND
#define CONFIG_CONSOLE_BOARD_COMMAND
```
Support Task and Hook function:
```c
#define CONFIG_TASK
#define CONFIG_TASK_DEBUG
#define CONFIG_TASK_HOOK
//#define CONFIG_TASK_HOOK_TEST
#define CONFIG_TASK_HOOK_COMMAND
```
Support GPIO function:
```c
#define CONFIG_GPIO
#define CONFIG_GPIO_LIST
#define CONFIG_GPIO_DEBUG
#define CONFIG_GPIO_GET_COMMAND
#define CONFIG_GPIO_SET_COMMAND

enum gpio_id {
        GPIO_LED1 = 0,
        GPIO_LED2,
        GPIO_BUTTON1,
        GPIO_BUTTON2,
        GPIO_COUNT
};
```
Support LED function:
```c
#define CONFIG_LED
#define CONFIG_LED_DEBUG
#define CONFIG_LED_HOOK
#define CONFIG_LED_COMMAND
#define CONFIG_LED_RED
#define CONFIG_LED_YELLOW
//#define CONFIG_LED_GREEN
//#define CONFIG_LED_BLUE
/* LED pin define */
#define LED_RED_Pin LED1_Pin
#define LED_RED_GPIO_Port LED1_GPIO_Port
#define LED_YELLOW_Pin LED2_Pin
#define LED_YELLOW_GPIO_Port LED2_GPIO_Port
```
Support I2C read, write and scan function:
```c
#define CONFIG_I2C
#define CONFIG_I2C_DEBUG
#define CONFIG_I2C_READ_COMMAND
#define CONFIG_I2C_WRITE_COMMAND
#define CONFIG_I2C_SCAN
```
Support EEPROM AT24C02 function:
```c
#define CONFIG_EEPROM_AT24C02
#define CONFIG_EEPROM_AT24C02_DEBUG
#define CONFIG_EEPROM_AT24C02_COMMAND
//#define CONFIG_EEPROM_AT24C02_SELF_TEST
```
Support Button function:
```c
#define CONFIG_BUTTON
#define CONFIG_BUTTON_DEBUG
#define CONFIG_BUTTON_COMMAND
/* Button pin define */
#define BUTTON_1_Pin BUTTON1_Pin
#define BUTTON_1_GPIO_Port BUTTON1_GPIO_Port
#define BUTTON_2_Pin BUTTON2_Pin
#define BUTTON_2_GPIO_Port BUTTON2_GPIO_Port
```
Support CH224K based PD function:
```c
#define CONFIG_PD_CH224K
#define CONFIG_PD_CH224K_DEBUG
#define CONFIG_PD_CH224K_COMMAND
#define CONFIG_PD_CH224K_ERR_DETECT
/* PD config pin define */
#define CH224K_CFG_IO1 GPIO_CH224K_CFG1
#define CH224K_CFG_IO2 GPIO_CH224K_CFG2
#define CH224K_CFG_IO3 GPIO_CH224K_CFG3
```
Support ADC function:
```c
#define CONFIG_ADC
#define CONFIG_ADC_DEBUG
#define CONFIG_ADC_COMMAND
//#define CONFIG_ADC_SMOOTH_VALUE
/* ADC channel name define */
enum adc_ch {
        VBUS_MON = 0,
        VBAT_MON,
        ADC_CH_COUNT
};
```
Support FAN function:
```c
#define CONFIG_FAN
#define CONFIG_FAN_DEBUG
// #define CONFIG_FAN_PWM_TEST
#define CONFIG_FAN_COMMAND
```
Support OLED SSD1306 function:
```c
#define SSD1306_OLED
#define SSD1306_USE_I2C

// I2C Configuration
#define SSD1306_I2C_PORT hi2c1
#define SSD1306_I2C_ADDR (0x3C << 1)

// Include only needed fonts
//#define SSD1306_INCLUDE_FONT_6x8
#define SSD1306_INCLUDE_FONT_7x10
//#define SSD1306_INCLUDE_FONT_11x18
//#define SSD1306_INCLUDE_FONT_16x26
```
Support Thermistors function:
```c
#define CONFIG_THERMISTORS
```
Support INA3221:
```c
#define CONFIG_INA3221
#define CONFIG_INA3221_DEBUG
#define CONFIG_INA3221_COMMAND
```

