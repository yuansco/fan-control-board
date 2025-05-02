
#include "adc.h"
#include "at24c02.h"
#include "board.h"
#include "build_info.h"
#include "console.h"
#include "ch224k.h"
#include "fan.h"
#include "i2c.h"
#include "ina3221.h"
#include "task.h"
#include "gpio.h"
#include "led.h"

#include "util.h"

#include <string.h>


int command_version(int argc, const char **argv) {

        PRINTS("\r\n%s Ver: %s-%s-%s", BOARDNAME, VERSION, BUILD_COMMIT, BUILD_DATE);
        return EC_SUCCESS;
}

int command_i2cscan(int argc, const char **argv) {

        i2c_scan();
        return EC_SUCCESS;
}

int command_ina(int argc, const char **argv) {

        ina3221_print();
        return EC_SUCCESS;
}

int system_reboot(void) {

        PRINTS("rebooting...");

        /* disable interrupts */
        __disable_irq();
        /* reset chip */
        HAL_NVIC_SystemReset();
        /* waitting for NVIC reset system core */
        while(1);
        /* will not run to here */
        return EC_ERROR_UNKNOWN;
}

int command_reboot(int argc, const char **argv) {

        char *e;
        int delay;

        if (argc > 1) {
                delay = strtoi(argv[1], &e, 10);

                if (*e || delay < 0 || delay > 100000)
                        return EC_ERROR_INVAL;
                
                PRINTS("system reboot after %d ms", delay);

                hook_call_deferred(&system_reboot, delay);
                return EC_SUCCESS;
        }

        system_reboot();
        return EC_SUCCESS;
}

int command_task(int argc, const char **argv) {

        task_print();
        return EC_SUCCESS;
}


int command_hook(int argc, const char **argv) {

        char *e;
        int debug_level;

        if (argc > 1) {
                debug_level = strtoi(argv[1], &e, 10);

                if (*e || debug_level < 0 || debug_level > 1)
                        return EC_ERROR_INVAL;

                PRINTS("Set hook debug level=%d", debug_level);

                hook_debug(debug_level);
                return EC_SUCCESS;
        }

        hook_print();
        return EC_SUCCESS;
}

int command_help(int argc, const char **argv) {

        PRINTS("\r\nKnown commands:\r\n");

        if (argc == 1) {
                for (int i = 0; i < ARRAY_SIZE(command); i++)
                        PRINTS(" %8s  %s", command[i].name, command[i].depiction);
                return EC_SUCCESS;
        }

        for (int i = 0; i < ARRAY_SIZE(command); i++) {
                PRINTS(" %s\r\n     Usage:%s\r\n     %s",
                        command[i].name, command[i].arg_desc, command[i].depiction);
        }

        return EC_SUCCESS;
}

int command_eeprom(int argc, const char **argv) {

        char *e;
        int size;

        if (argc > 1) {
                size = strtoi(argv[1], &e, 10);

                if (*e || size < 0 || size > 256)
                        return EC_ERROR_INVAL;

                PRINTS("dump eeprom %d Bytes", size);

                at24c02_dump((uint8_t) size);
                return EC_SUCCESS;
        }

        PRINTS("dump eeprom");

        at24c02_dump(0xff);
        return EC_SUCCESS;
}

int command_fan(int argc, const char **argv) {

        fan_print();
        return EC_SUCCESS;
}

int command_gpioget(int argc, const char **argv) {

        char *e;
        int id;

        if (argc > 2)
                return EC_ERROR_INVAL;

        // gpioget
        if (argc == 1 || !strcasecmp(argv[1], "all")) {
                gpio_print();
                return EC_SUCCESS;
        }

        // gpioget 0
        if (argc == 2) {

                id = strtoi(argv[1], &e, 10);

                if (*e || (id < 0) || (id >= GPIO_COUNT)) {
                        PRINTS("gpio number (%d) error!", id);
                        return EC_ERROR_INVAL;
                }

                PRINTS("%d", gpio_get(id));
                return EC_SUCCESS;
        }

        return EC_ERROR_INVAL;
}

int command_gpioset(int argc, const char **argv) {

        char *e;
        int id, state;

        if (argc != 3)
                return EC_ERROR_INVAL;

        id = strtoi(argv[1], &e, 10);
        if (*e || (id < 0) || (id >= GPIO_COUNT)) {
                PRINTS("gpio number error!");
                return EC_ERROR_INVAL;
        }

        if(gpio_list[id].mode == IO_MODE_INPUT) {
                PRINTF("%s is input mode!\r\n", gpio_list[id].name);
                return EC_ERROR_INVAL;
        }

        state = !!strtoi(argv[2], &e, 10);
        if (*e) {
                PRINTS("gpio state error!");
                return EC_ERROR_INVAL;
        }

        PRINTS("Set %s (%d) to %d", gpio_list[id].name, id, state);
        gpio_set(id, state);

        return EC_SUCCESS;
}

int command_time(int argc, const char **argv) {

        int time;

        time = (int) HAL_GetTick();

        PRINTS(" system time: %02d.%03d (s)", (time / 1000), (time % 1000));
        return EC_SUCCESS;
}

// [0100.916]>test 11 22 33 
// [0100.918] argc: 4
// [0100.920] argv[0]: "test"
// [0100.922] argv[1]: "11"
// [0100.925] argv[2]: "22"
// [0100.927] argv[3]: "33"
//
int command_test(int argc, const char **argv) {

        int i;

        PRINTS(" argc: %d", argc);

        for (i = 0; i < argc; i++)
                PRINTS(" argv[%d]: \"%s\"", i, argv[i]);

        return EC_SUCCESS;
}


int command_md(int argc, const char **argv) {

        char *e;
        int size = 1;
        uint32_t *p ,addr;

        if (argc < 2 || argc > 3)
                return EC_ERROR_INVAL;

        addr = (uint32_t) strtoi(argv[1], &e, 16);

        if (*e)
                return EC_ERROR_INVAL;

        if (argc == 3) {
                size = strtoi(argv[2], &e, 10);

                if (*e || size > 32)
                        return EC_ERROR_INVAL;
        }

        p = (uint32_t *) addr;

        while (size-- > 0) {
                PRINTS("  0x%08lx is 0x%08lx",
                        (long unsigned int)p, *p);
                p++;
        }

        return EC_SUCCESS;
}

int command_led(int argc, const char **argv) {

        char *e;
        int id;

        if (argc > 4)
                return EC_ERROR_INVAL;

        // led get all
        // led get 0
        if (argc == 3 && !strcasecmp(argv[1], "get")) {

                if (!strcasecmp(argv[2], "all")) {
                        led_print();
                        return EC_SUCCESS;
                }

                id = strtoi(argv[2], &e, 10);
                if (*e || (id < 0) || (id >= LED_COUNT))
                        return EC_ERROR_INVAL;

                PRINTS(" led id:%d name:%s read:%d",id,
                                        led_list[id].name, led_get(id));
                return EC_SUCCESS;
        }

        // led set 0 on
        // led set 0 off
        if (argc == 4 && !strcasecmp(argv[1], "set")) {

                id = strtoi(argv[2], &e, 10);

                if (*e || (id < 0) || (id >= LED_COUNT))
                        return EC_ERROR_INVAL;

                if (!strcasecmp(argv[3], "on")) {
                        /* 'on' */
                        led_command_set(id, LED_ON);
                        return EC_SUCCESS;
                } else if (!strcasecmp(argv[3], "off")) {
                        /* 'off' */
                        led_command_set(id, LED_OFF);
                        return EC_SUCCESS;
                } else if (!strcasecmp(argv[3], "reset")) {
                        /* 'off' */
                        led_command_reset(id);
                        return EC_SUCCESS;
                }
        }

        // led
        led_print();
        if (argc == 1)
                return EC_SUCCESS;
        else
                return EC_ERROR_UNKNOWN;
}

int command_adc(int argc, const char **argv) {

        adc_print();
        return EC_SUCCESS;
}


int command_i2cread(int argc, const char **argv) {

        uint16_t addr, reg, size;
        char *e;
        int rv;

        if (argc != 4)
                return EC_ERROR_INVAL;

        addr = strtoi(argv[1], &e, 16);
        if (*e || addr > 0x7f)
                return EC_ERROR_INVAL;

        reg = strtoi(argv[2], &e, 16);
        if (*e || reg > 0xff)
                return EC_ERROR_INVAL;

        size = strtoi(argv[3], &e, 10);
        if (*e || size > 0xff)
                return EC_ERROR_INVAL;

        uint8_t data[size];

        rv = i2c_read(addr, reg, data, size);
        if (rv != EC_SUCCESS)
                return rv;

        PRINTF("received:");
        for (int i = 0; i < ARRAY_SIZE(data); i++) {
                if (i % 8 == 0)
                        PRINTF("\r\n %02x ", data[i]);
                else
                        PRINTF("%02x ", data[i]);
        }
        PRINTF("\r\n");

        return EC_SUCCESS;
}

int command_i2cwrite(int argc, const char **argv) {

        uint16_t addr, reg, data;
        char *e;
        int rv;

        if (argc != 4)
                return EC_ERROR_INVAL;

        /* device address */
        addr = strtoi(argv[1], &e, 16);
        if (*e || addr > 0x7f)
                return EC_ERROR_INVAL;

        /* target reg */
        reg = strtoi(argv[2], &e, 16);
        if (*e || reg > 0xff)
                return EC_ERROR_INVAL;

        /* data */
        data = strtoi(argv[3], &e, 16);
        if (*e || data > 0xff)
                return EC_ERROR_INVAL;

        rv = i2c_write(addr, reg, (uint8_t *)&data, 1);

        if (rv == EC_SUCCESS)
                PRINTS("i2c write success!");

        return rv;
}

int command_pd(int argc, const char **argv) {

        int volt;
        char *e;

        if (argc > 3)
                return EC_ERROR_INVAL;

        // pd
        // pd get
        if (argc == 1 || (argc == 2 && !strcasecmp(argv[1], "get"))) {
                pd_print();
                return EC_SUCCESS;
        }

        // pd set 15
        if (argc == 3 && !strcasecmp(argv[1], "set")) {

                volt = strtoi(argv[2], &e, 10);

                switch (volt)
                {
                case 5:
                        ch224k_select_pd_power(PD_VOLT_5V);
                        break;
                case 9:
                        ch224k_select_pd_power(PD_VOLT_9V);
                        break;
                case 12:
                        ch224k_select_pd_power(PD_VOLT_12V);
                        break;
                case 15:
                        ch224k_select_pd_power(PD_VOLT_15V);
                        break;
                case 20:
                        ch224k_select_pd_power(PD_VOLT_20V);
                        break;
                default:
                        return EC_ERROR_INVAL;
                        break;
                }
                return EC_SUCCESS;
        }
        return EC_ERROR_INVAL;
}

#ifdef CONFIG_CONSOLE_HISTORY

int history_cnt;
char history_buff[CONSOLE_HISTORY_BUFF_SIZE][CONFIG_CONSOLE_COMMAND_BUFFER_SIZE];

#define BUFF_SIZE CONSOLE_HISTORY_BUFF_SIZE

int command_history(int argc, const char **argv) {

        if (argc == 2 && !strcasecmp(argv[1], "clear")) {

                PRINTS("clear history command.");

                history_cnt = 0;

                memset(history_buff, 0, BUFF_SIZE * CONFIG_CONSOLE_COMMAND_BUFFER_SIZE);
                
                return EC_SUCCESS;
        }

        int offset = (history_cnt > BUFF_SIZE) ? (history_cnt - BUFF_SIZE):0;
        for (int i = 0; i < MIN(history_cnt, ARRAY_SIZE(history_buff)); i++)
                PRINTS(" [%d] %s", (offset + i), history_buff[i]);

        return EC_SUCCESS;
}
#endif

struct console_command command[] = {
        [COMMAND_HELP] = {
                .name = "help",
                .arg_desc = "help [list]",
                .depiction = "Show this help message.",
                .handler = &command_help,
        },
        [COMMAND_VERSION] = {
                .name = "version",
                .arg_desc = "version",
                .depiction = "Show the firmware version.",
                .handler = &command_version,
        },
        [COMMAND_REBOOT] = {
                .name = "reboot",
                .arg_desc = "reboot [delay (ms)]",
                .depiction = "Reboot controller.",
                .handler = &command_reboot,
        },
        [COMMAND_TEST] = {
                .name = "test",
                .arg_desc = "test",
                .depiction = "console command argument test.",
                .handler = &command_test,
        },
        [COMMAND_TIME] = {
                .name = "time",
                .arg_desc = "time",
                .depiction = "print system time (ms).",
                .handler = &command_time,
        },
        [COMMAND_MD] = {
                .name = "md",
                .arg_desc = "md <address> [size]",
                .depiction = "memory dump",
                .handler = &command_md,
        },
#ifdef CONFIG_ADC_COMMAND
        [COMMAND_ADC] = {
                .name = "adc",
                .arg_desc = "adc [all]",
                .depiction = "Read all adc channels.",
                .handler = &command_adc,
        },
#endif /* CONFIG_ADC_COMMAND */
#ifdef CONFIG_CONSOLE_HISTORY
        [COMMAND_HISTORY] = {
                .name = "history",
                .arg_desc = "history [clear]",
                .depiction = "print the history command list.",
                .handler = &command_history,
        },
#endif /* CONFIG_CONSOLE_HISTORY */
#ifdef CONFIG_TASK
        [COMMAND_TASK] = {
                .name = "task",
                .arg_desc = "task",
                .depiction = "Print all task information.",
                .handler = &command_task,
        },
#ifdef CONFIG_TASK_HOOK_COMMAND
        [COMMAND_HOOK] = {
                .name = "hook",
                .arg_desc = "hook [0|1]",
                .depiction = "Print hook information or set debug level.",
                .handler = &command_hook,
        },
#endif /* CONFIG_TASK_HOOK && CONFIG_TASK_HOOK_COMMAND */
#endif /* CONFIG_TASK */
#ifdef CONFIG_I2C_SCAN
        [COMMAND_I2CSCAN] = {
                .name = "i2cscan",
                .arg_desc = "i2cscan",
                .depiction = "Scan and list all responsive i2c devices.",
                .handler = &command_i2cscan,
        },
#endif /* CONFIG_I2C_SCAN */
#ifdef CONFIG_INA3221_COMMAND
        [COMMAND_INA3221] = {
                .name = "ina",
                .arg_desc = "ina",
                .depiction = "Read and show all channel voltage and current.",
                .handler = &command_ina,
        },
#endif /* CONFIG_INA3221_COMMAND */
#ifdef CONFIG_LED_COMMAND
        [COMMAND_LED] = {
                .name = "led",
                .arg_desc = "led <get|set> <id|all> [on|off]",
                .depiction = "Set or get led state.",
                .handler = &command_led,
        },
#endif /* CONFIG_LED_COMMAND */
#ifdef CONFIG_EEPROM_AT24C02_COMMAND
        [COMMAND_EEPROM] = {
                .name = "eeprom",
                .arg_desc = "eeprom [dump_size]",
                .depiction = "dump eeprom data",
                .handler = &command_eeprom,
        },
#endif /* CONFIG_EEPROM_AT24C02_COMMAND */
#ifdef CONFIG_FAN_COMMAND
        [COMMAND_FAN] = {
                .name = "fan",
                .arg_desc = "fan",
                .depiction = "Print all fan info",
                .handler = &command_fan,
        },
#endif /* CONFIG_FAN_COMMAND */

#ifdef CONFIG_GPIO_GET_COMMAND
        [COMMAND_GPIOGET] = {
                .name = "gpioget",
                .arg_desc = "gpioget [id|all]",
                .depiction = "dump gpio state",
                .handler = &command_gpioget,
        },
#endif /* CONFIG_GPIO_GET_COMMAND */
#ifdef CONFIG_GPIO_SET_COMMAND
        [COMMAND_GPIOSET] = {
                .name = "gpioset",
                .arg_desc = "gpioset <id> <0|1>",
                .depiction = "set gpio state",
                .handler = &command_gpioset,
        },
#endif /* CONFIG_GPIO_SET_COMMAND */
#ifdef CONFIG_I2C_READ_COMMAND
        [COMMAND_I2CREAD] = {
                .name = "i2cread",
                .arg_desc = "i2cread <addr7> <reg> <size>",
                .depiction = "read data from i2c device.",
                .handler = &command_i2cread,
        },
#endif /* CONFIG_I2C_READ_COMMAND */
#ifdef CONFIG_I2C_WRITE_COMMAND
        [COMMAND_I2CWRITE] = {
                .name = "i2cwrite",
                .arg_desc = "i2cwrite <addr7> <reg> <data>",
                .depiction = "write data to i2c device.",
                .handler = &command_i2cwrite,
        },
#endif /* CONFIG_I2C_WRITE_COMMAND */
#ifdef CONFIG_PD_CH224K_COMMAND
        [COMMAND_PD] = {
                .name = "pd",
                .arg_desc = "pd <get|set> [5|9|12|15|20]",
                .depiction = "get or set pd state",
                .handler = &command_pd,
        },
#endif /* CONFIG_PD_CH224K_COMMAND */
#ifdef CONFIG_CONSOLE_BOARD_COMMAND
        [COMMAND_BOARD_COMMAND] = {
                .name = "board",
                .arg_desc = "TODO",
                .depiction = "TODO",
                .handler = NULL,
        },
#endif /* CONFIG_CONSOLE_BOARD_COMMAND */


};

