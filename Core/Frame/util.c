
#include "console.h"
#include "util.h"
#include <ctype.h>
#include <string.h>
#include <stdint.h>

/**
 * copy from https://github.com/coreboot/chrome-ec (common/util.c)
 */
int find_base(int base, int *c, const char **nptr)
{
        if ((base == 0 || base == 16) && *c == '0' &&
            (**nptr == 'x' || **nptr == 'X')) {
                *c = (*nptr)[1];
                (*nptr) += 2;
                base = 16;
        } else if (base == 0) {
                base = *c == '0' ? 8 : 10;
        }
        return base;
}

/**
 * copy from https://github.com/coreboot/chrome-ec (common/util.c)
 * Like strtol(), but for integers
 */
int strtoi(const char *nptr, char **endptr, int base)
{
        int result = 0;
        int neg = 0;
        int c = '\0';

        while ((c = *nptr++) && isspace(c))
                ;

        if (c == '+') {
                c = *nptr++;
        } else if (c == '-') {
                neg = 1;
                c = *nptr++;
        }

        base = find_base(base, &c, &nptr);

        while (c) {
                if (c >= '0' && c < '0' + MIN(base, 10))
                        result = result * base + (c - '0');
                else if (c >= 'A' && c < 'A' + base - 10)
                        result = result * base + (c - 'A' + 10);
                else if (c >= 'a' && c < 'a' + base - 10)
                        result = result * base + (c - 'a' + 10);
                else
                        break;

                c = *nptr++;
        }

        if (endptr)
                *endptr = (char *)nptr - 1;
        return neg ? -result : result;
}

int parse_bool(const char *s, int *dest)
{
        /* off, disable, false, no */
        if (!strcasecmp(s, "off") || !strncasecmp(s, "dis", 3) ||
            tolower((unsigned char)*s) == 'f' ||
            tolower((unsigned char)*s) == 'n') {
                *dest = 0;
                return 1;
        }

        /* on, enable, true, yes */
        if (!strcasecmp(s, "on") || !strncasecmp(s, "ena", 3) ||
            tolower((unsigned char)*s) == 't' ||
            tolower((unsigned char)*s) == 'y') {
                *dest = 1;
                return 1;
        }

        /* dunno */
        return 0;
}

int strreplace(char *buff, int size, char target, char replace) {

        /* Do not replace '\n' in buff[0], since we need reply '>' */
        for (int i=1; i < size; i++)
                if(buff[i] == target) {
                        buff[i] = replace;
                        return i;
                }
        return EC_COMMAND_NOT_FOUND;
}

int strfindchar(char * strin, int size, char c, int offset) {
    
    int i;

    for (i=offset ;i<size; i++)
        if (strin[i] == c)
            return i;
    return -1;
}

/**
 * copy from https://github.com/coreboot/chrome-ec (common/util.c)
 */

void hexdump(const uint8_t *data, int len)
{
        int i, j;

        if (!data || !len)
                return;

        for (i = 0; i < len; i += 16) {
                /* Left column (Hex) */
                for (j = i; j < i + 16; j++) {
                        if (j < len)
                                PRINTF(" %02x", data[j]);
                        else
                                PRINTF("   ");
                }
                /* Right column (ASCII) */
                PRINTF(" |");
                for (j = i; j < i + 16; j++) {
                        int c = j < len ? data[j] : ' ';
                        PRINTF("%c", isprint(c) ? c : '.');
                }
                PRINTS("|");
        }
}

