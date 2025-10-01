
#ifndef __CONSOLE_H__
#define __CONSOLE_H__


#include <stdio.h>

#include "main.h"
#include "task.h"

/* should define in board.h if needed */
//#define CONFIG_UART_DEBUG
//#define CONFIG_CONSOLE_COMMAND
//#define CONFIG_CONSOLE_BOARD_COMMAND

#ifdef CONFIG_CONSOLE_COMMAND
#define CONFIG_CONSOLE_HISTORY
#define CONFIG_CONSOLE_COMMAND_ECHO
#define CONFIG_CONSOLE_TIMESTAMP
#endif


#ifdef CONFIG_UART_DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINTS(...) {printf(__VA_ARGS__);printf("\r\n");}
#else
#define PRINTF(args...)
#define PRINTFLN(args...)
#endif

/**
 * console command function
 */

#ifdef CONFIG_UART_DEBUG

extern UART_HandleTypeDef huart1;

#ifndef CONSOLE_UART_PORT
#define CONSOLE_UART_PORT huart1
#endif

#endif /* CONFIG_UART_DEBUG */


#if defined(CONFIG_CONSOLE_COMMAND) && !defined(CONFIG_UART_DEBUG)
#error "You should define CONFIG_UART_DEBUG if you want enable CONSOLE_COMMAND too!"
#endif

#ifdef CONFIG_CONSOLE_COMMAND

#define split_char " "
#define ARGS_MAX_NUM 5
#define ARGS_BUFF_SIZE (ARGS_MAX_NUM + 2)

#define CONFIG_CONSOLE_COMMAND_TIME_MS 5
#define CONFIG_CONSOLE_COMMAND_BUFFER_SIZE 32

/* Console command */
struct console_command {
        /* Command name */
        const char *name;
        /* Command depiction */
        const char *depiction;
        /* Command argument depiction*/
        const char *arg_desc;
        /* Handler for the command.  argv[0] will be the command name. */
        int (*handler)(int argc, const char **argv);
};


/* Console Command List */
enum command_list {
#ifdef CONFIG_ADC_COMMAND
        COMMAND_ADC,
#endif
#ifdef CONFIG_EEPROM_AT24C02
        COMMAND_EEPROM,
#endif
#ifdef CONFIG_FAN_COMMAND
        COMMAND_FAN,
#endif
#ifdef CONFIG_GPIO_GET_COMMAND
        COMMAND_GPIOGET,
#endif
#ifdef CONFIG_GPIO_SET_COMMAND
        COMMAND_GPIOSET,
#endif
        COMMAND_HELP,
#ifdef CONFIG_CONSOLE_HISTORY
        COMMAND_HISTORY,
#endif
#ifdef CONFIG_TASK_HOOK_COMMAND
        COMMAND_HOOK,
#endif
#ifdef CONFIG_I2C_READ_COMMAND
        COMMAND_I2CREAD,
#endif
#ifdef CONFIG_I2C_WRITE_COMMAND
        COMMAND_I2CWRITE,
#endif
#ifdef CONFIG_I2C_SCAN
        COMMAND_I2CSCAN,
#endif
#ifdef CONFIG_INA3221_COMMAND
        COMMAND_INA3221,
#endif
#ifdef CONFIG_ADC_PAC1954_COMMAND
        COMMAND_ADC_PAC1954,
#endif
#ifdef CONFIG_LED_COMMAND
        COMMAND_LED,
#endif
        COMMAND_MD,
#ifdef CONFIG_PD_CH224K_COMMAND
        COMMAND_PD,
#endif
        COMMAND_REBOOT,
#ifdef CONFIG_TASK
        COMMAND_TASK,
#endif
        COMMAND_TEST,
        COMMAND_TIME,
        COMMAND_VERSION,
#ifdef CONFIG_CONSOLE_BOARD_COMMAND
        COMMAND_BOARD_COMMAND,
#endif
        COMMAND_LIST_COUNT
};

extern struct console_command command[COMMAND_LIST_COUNT];

int console_command_init(void);
int console_command_task(enum task_list task_id);


#ifdef CONFIG_CONSOLE_HISTORY
#define CONSOLE_HISTORY_BUFF_SIZE 32
extern int history_cnt;
extern char history_buff[CONSOLE_HISTORY_BUFF_SIZE][CONFIG_CONSOLE_COMMAND_BUFFER_SIZE];
#endif


#endif /* CONFIG_CONSOLE_COMMAND */

#endif /* __CONSOLE_H__ */
