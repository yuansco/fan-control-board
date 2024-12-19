#include "adc.h"
#include "at24c02.h"
#include "board.h"
#include "build_info.h"
#include "ch224k.h"
#include "led.h"
#include "console.h"
#include "fan.h"
#include "gpio.h"
#include "i2c.h"
#include "ina3221.h"
#include "main.h"
#include "task.h"
#include "ssd1306.h"
#include "ssd1306_show.h"
#include "util.h"

#include <stdio.h>


/******************************************************************************/
/**
 * overridable board init function
 * declare board level task or hook here
 */ 
__overridable void board_init(void)
{
}

/******************************************************************************/
/* default init function */

void init() {

        __maybe_unused int i2c_fail = FALSE;

        PRINTF("\r\n%s Ver: %s-%s-%s\r\n", BOARDNAME, VERSION, BUILD_COMMIT, BUILD_DATE);

#ifdef CONFIG_PD_CH224K
        ch224k_init();
#endif
#ifdef SSD1306_OLED
        if (ssd1306_init() != EC_SUCCESS)
                i2c_fail = TRUE;
        else
                ssd1306_show();
#endif
#ifdef CONFIG_CONSOLE_COMMAND
        console_command_init();
#endif
#ifdef CONFIG_EEPROM_AT24C02
        if (at24c02_init() != EC_SUCCESS)
                i2c_fail = TRUE;
#endif
#ifdef CONFIG_I2C_SCAN
        if (i2c_fail)
                i2c_scan();
#endif
#ifdef CONFIG_TASK
        task_init();
#endif
#ifdef CONFIG_GPIO
        gpio_init();
#endif
#ifdef CONFIG_LED
        led_init();
#endif
#ifdef CONFIG_ADC
        adc_init();
#endif
#ifdef CONFIG_FAN
        fan_init();
#endif
#ifdef CONFIG_INA3221
        ina3221_init();
#endif

        board_init();

        PRINTF("Init Done\r\n");

#ifdef CONFIG_TASK
        task_run();
#endif

        PRINTF("ERROR: running with no task mode!\r\n");
}


