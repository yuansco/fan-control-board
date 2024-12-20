
#include "console.h"
#include "util.h"
#include <string.h>
#include <stdint.h>

/******************************************************************************/
/* Console Command  */

#ifdef CONFIG_UART_DEBUG
#define CPRINTF(format, args...) PRINTF("COMMAND: " format, ##args)
#define CPRINTS(format, args...) PRINTS("COMMAND: " format, ##args)
#else
#define CPRINTF(format, args...)
#define CPRINTS(format, args...)
#endif


#ifdef CONFIG_CONSOLE_COMMAND

static uint16_t console_cnt = 0;
static uint8_t buffer_ready = 0;

volatile uint8_t console_buffer[CONFIG_CONSOLE_COMMAND_BUFFER_SIZE + 1];
volatile uint8_t buffer = '\0';

// Enable the console port UART interrupt
void console_enable_rx_interrupt(void) {

        HAL_UART_Receive_IT(&CONSOLE_UART_PORT, (uint8_t *) &buffer, sizeof(buffer));
}

// Disable the console port UART interrupt
void console_disable_rx_interrupt(void) {
        HAL_UART_AbortReceive_IT(&CONSOLE_UART_PORT);
}

void console_buffer_dump() {
        PRINTF((const char *) console_buffer);
}

void console_buffer_reset() {

        buffer = '\0';
        console_cnt = 0;
        buffer_ready = 0;

        memset((void *) console_buffer, 0, sizeof(console_buffer));

        console_enable_rx_interrupt();
}

/* UART receive callback function */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

        console_buffer[console_cnt++] = buffer;

        if(console_cnt >= CONFIG_CONSOLE_COMMAND_BUFFER_SIZE ||
            buffer == '\n' || buffer == '\r' ) {
                buffer_ready = 1;
                return;
        }

        console_enable_rx_interrupt();
}

#ifdef CONFIG_CONSOLE_HISTORY
void save_command(void) {

        // "\n" = 10, "\r" = 13 
        if (console_buffer[0] == 10 || console_buffer[0] == 13)
                return;

        if (history_cnt >= CONSOLE_HISTORY_BUFF_SIZE) {
                for (int i = 0; i < (ARRAY_SIZE(history_buff) - 1); i++) {
                        strncpy(history_buff[i], history_buff[i + 1],
                                        CONFIG_CONSOLE_COMMAND_BUFFER_SIZE);
                }
                strncpy(history_buff[CONSOLE_HISTORY_BUFF_SIZE - 1],
                (const char *) console_buffer, CONFIG_CONSOLE_COMMAND_BUFFER_SIZE);

        } else {
                strncpy(history_buff[history_cnt],
                (const char *) console_buffer, CONFIG_CONSOLE_COMMAND_BUFFER_SIZE);
        }

        history_cnt++;
        return;
}
#endif /* CONFIG_CONSOLE_HISTORY */

#ifdef CONFIG_CONSOLE_COMMAND_ECHO
void echo_command(void) {

        if (console_buffer[0] != 10 && console_buffer[0] != 13) {
                PRINTS("%s", (const char *) console_buffer);
        }
}
#endif /* CONFIG_CONSOLE_COMMAND_ECHO */


int split_command(int *argc, char **argv) {

        int i = 0;

        char *p = strtok((char *)console_buffer, split_char);

        while (p != NULL && i < ARGS_MAX_NUM)
        {
            argv[i++] = p;
            p = strtok(NULL, split_char);
        }

        *argc = i;

        return EC_SUCCESS;
}

int find_command(const char *name) {

        // NL or CR
        // '\n' = 10, '\r' = 13
        if (name[0] == 10 || name[0] == 13) {
                PRINTF("\n>");
                return EC_COMMAND_NOT_FOUND;
        }

        for (int i = 0; i < ARRAY_SIZE(command); i++) {

                if (name == NULL || command[i].name == NULL)
                        continue;

                // find the command
                char *index = strstr(name, command[i].name);

                if(index != NULL)
                        return i;
        }

        PRINTS("command not found: %s", name);
        return EC_COMMAND_NOT_FOUND;
}

int handle_command(void) {

        int argc, command_id;
        int rv = EC_SUCCESS;
        char *argv[ARGS_BUFF_SIZE];

        strreplace((char *) console_buffer,
                ARRAY_SIZE(console_buffer),'\n', ' ');

#ifdef CONFIG_CONSOLE_COMMAND_ECHO
        echo_command();
#endif /* CONFIG_CONSOLE_COMMAND_ECHO */

        strreplace((char *) console_buffer,
                ARRAY_SIZE(console_buffer),'\n', ' ');

#ifdef CONFIG_CONSOLE_HISTORY
        /* save command */
        save_command();
#endif /* CONFIG_CONSOLE_HISTORY */

        split_command(&argc, argv);

        command_id = find_command(argv[0]);

        if (command_id < 0 || command_id > ARRAY_SIZE(command))
                return EC_ERROR_UNKNOWN;

        // run command handler function
        if (command[command_id].handler != NULL)
                rv = command[command_id].handler(argc, (const char **) argv);

        if (rv == EC_ERROR_INVAL) {
                PRINTS("Invalid param.\r\nUsage:%s",
                                        command[command_id].arg_desc);
        } else if (rv != EC_SUCCESS) {
                PRINTS("command return fail! (%d)", rv);
        }

        return rv;
}

/******************************************************************************/
/* console command task */

int console_command_init(void)
{
        CPRINTS("init");

        CPRINTS("enable uart interrupt");
        console_enable_rx_interrupt();
        return EC_SUCCESS;
}

int console_command_task(enum task_list task_id) {

        if (!buffer_ready)
                return EC_SUCCESS;

        /* handle command */
        handle_command();
        console_buffer_reset();

        return EC_SUCCESS;
}

#endif /* CONFIG_CONSOLE_COMMAND */
