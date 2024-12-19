
#include "console.h"
#include "util.h"

#include <stdint.h>

/******************************************************************************/
/* UART Debug  */
#define TIMETAG_SIZE 15

char time_tag[TIMETAG_SIZE];

#if defined(__GNUC__) && defined(CONFIG_UART_DEBUG) && !defined(UART_DEBUG_GPIO)
int _write(int fd, char * ptr, int len)
{
#ifdef CONFIG_CONSOLE_TIMESTAMP

       int size;
       uint32_t time = HAL_GetTick();

       sprintf(time_tag, "[%04lu.%03lu] ",  (time / 1000), (time % 1000));

       size = strfindchar(time_tag, TIMETAG_SIZE, ' ', 10);

       HAL_UART_Transmit(&CONSOLE_UART_PORT,
                           (uint8_t *) time_tag, size, HAL_MAX_DELAY);

#endif
        HAL_UART_Transmit(&CONSOLE_UART_PORT, (uint8_t *) ptr, len, HAL_MAX_DELAY);

        return len;
}
#elif defined (__ICCARM__) && defined(CONFIG_UART_DEBUG)
#include "LowLevelIOInterface.h"
size_t __write(int handle, const unsigned char * buffer, size_t size)
{
        HAL_UART_Transmit(&CONSOLE_UART_PORT, (uint8_t *) buffer, size, HAL_MAX_DELAY);
        return size;
}
#elif defined (__CC_ARM) && defined(CONFIG_UART_DEBUG)
int fputc(int ch, FILE *f)
{
        HAL_UART_Transmit(&CONSOLE_UART_PORT, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
        return ch;
}
#endif

