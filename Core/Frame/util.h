
/* util */

#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdint.h>

#ifndef __override
#define __override
#define __overridable	__attribute__((weak))
#endif


#ifndef __packed
#define __packed        __attribute__((packed))
#endif


#ifndef __maybe_unused
#define __maybe_unused  __attribute__((unused))
#endif

#define TRUE 1
#define FALSE 0

#define BIT(nr) (1U << (nr))

#define ABS(x) ((x) >= 0 ? (x) : -(x))

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

#define ARRAY_SIZE(x)  (sizeof(x) / sizeof((x)[0]))


/* List of common error codes that can be returned */
enum error_codes_list {
        /* Unknown - command not found */
        EC_COMMAND_NOT_FOUND = -1,
        /* Success - no error */
        EC_SUCCESS = 0,
        /* Unknown error */
        EC_ERROR_UNKNOWN = 1,
        /* Overflow error; too much input provided. */
        EC_ERROR_OVERFLOW = 3,
        /* Timeout */
        EC_ERROR_TIMEOUT = 4,
        /* Invalid argument */
        EC_ERROR_INVAL = 5,
        /* Already in use, or not ready yet */
        EC_ERROR_BUSY = 6,
        /* Access denied */
        EC_ERROR_ACCESS_DENIED = 7,
};

int find_base(int base, int *c, const char **nptr);
int strtoi(const char *nptr, char **endptr, int base);
int strreplace(char *buff, int size, char target, char replace);
void hexdump(const uint8_t *data, int len);
int strfindchar(char * strin, int size, char c, int offset);

#endif /* __UTIL_H__ */

