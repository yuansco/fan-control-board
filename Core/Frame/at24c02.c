
#include "at24c02.h"
#include "console.h"
#include "util.h"

#ifdef CONFIG_EEPROM_AT24C02

#ifdef CONFIG_EEPROM_AT24C02_DEBUG
#define CPRINTF(format, args...) PRINTF("AT24C02: " format, ##args)
#define CPRINTS(format, args...) PRINTS("AT24C02: " format, ##args)
#else
#define CPRINTF(format, args...)
#define CPRINTS(format, args...)
#endif

static int at24c02_set_reg_pointer(uint8_t reg)
{
        uint8_t re;

        re = HAL_I2C_Master_Transmit(&AT24C02_I2C_PORT,
                (AT24C02_I2C_ADDR << 1),  &reg, sizeof(reg), 1000);

        if (re != EC_SUCCESS) {
                CPRINTS("setup reg pointer fail! (%d)", re);
        }


        return re;
}

int at24c02_read(uint8_t reg, uint8_t *data, int size) {

        int re;

        /* set offset*/
        at24c02_set_reg_pointer(reg);

        re = HAL_I2C_Master_Receive(&AT24C02_I2C_PORT, (AT24C02_I2C_ADDR << 1),
                        data, size, 1000);

        if (re != EC_SUCCESS) {
                CPRINTS("read failed");
        }

        return re;
}

int at24c02_write(uint8_t reg, uint8_t *data, int size) {

        uint8_t data_write[2];
        int i, re;

        /* 
         * always write 1 byte once to support different page size EEPROM.
         * e.g. AT24C02 is 32 pages of  8 bytes
         *      AT24C04 is 32 pages of 16 bytes
         */
        for (i = 0; i < size; i++) {

                /* byte[0] is reg address */
                data_write[0] = reg + i;
                data_write[1] = data[i];

                re = HAL_I2C_Master_Transmit(&AT24C02_I2C_PORT, (AT24C02_I2C_ADDR << 1),
                        data_write, sizeof(data_write), 1000);
                
                if (re != EC_SUCCESS) {
                        CPRINTS("i2c write failed! (%d)", re);
                        return re;
                }
        }

        return EC_SUCCESS;
}

int __at24c02_write(uint8_t reg, uint8_t *data, int size) {

        int i, re;
        uint8_t data_write[size + 1];

        data_write[0] = reg;

        for (i = 0; i < size; i++)
                data_write[i + 1] = data[i];

        for(int e=0; e<sizeof(data_write); e++) {
                CPRINTF("%02x ",data_write[e]);
        }

        re = HAL_I2C_Master_Transmit(&AT24C02_I2C_PORT, (AT24C02_I2C_ADDR << 1),
                        data_write, sizeof(data_write), 1000);

        return re;
}

void at24c02_dump(uint8_t size) {

        uint8_t vlue[size];

        if (size > 0xff) {
                CPRINTS("dump size over 0xFF!");
                return;
        }

        at24c02_read(0x00, vlue, sizeof(vlue));

        hexdump(vlue, size);

/*
        for (i = 0; i < size; i++) {

                if (i % 8 == 0)
                        PRINTF("\r\n0x%02x: ", i);

                if (at24c02_read(i, &v, sizeof(v)) != EC_SUCCESS)
                        PRINTF("\r\n 0x%2x read fail\r\n", i);
                else
                        PRINTF("%02x ", v);
        }
*/
        PRINTF("\r\n");
}


#ifdef CONFIG_EEPROM_AT24C02_SELF_TEST
static int at24c02_read_write_test(void) {

        int i;
        uint8_t reg = 0x00;
        uint8_t buf[] = {0x53, 0x54, 0x4d, 0x33, 0x32}; //STM32
        uint8_t buf_read[sizeof(buf)];

        CPRINTS("read_write_test");

        CPRINTF("reg: %02x, data: ", reg);
        for (i = 0; i < sizeof(buf); i++)
                PRINTF("%02x ", buf[i]);
        PRINTS("");

        at24c02_write(reg, buf, sizeof(buf));
        HAL_Delay(10);
        at24c02_read(reg, buf_read, sizeof(buf_read));

        for (i = 0; i < sizeof(buf); i++) {
                if (buf[i] != buf_read[i]) {
                        CPRINTS("reg: %02x, test fail: %02x %02x",
                                        i, buf[i], buf_read[i]);
                        return EC_ERROR_UNKNOWN;
                }
        }

        reg = 0x30;

        CPRINTF("reg: %02x, data: ", reg);
        for (i = 0; i < sizeof(buf); i++)
                PRINTF("%02x ", buf[i]);
        CPRINTS("");

        at24c02_write(reg, buf, sizeof(buf));
        HAL_Delay(10);
        at24c02_read(reg, buf_read, sizeof(buf_read));

        for (i = 0; i < sizeof(buf); i++) {
                if (buf[i] != buf_read[i]) {
                        CPRINTS("reg: %02x, test fail: %02x %02x",
                                        i, buf[i], buf_read[i]);
                        return EC_ERROR_UNKNOWN;
                }
        }

        CPRINTS("test pass");
        return EC_SUCCESS;
}
#endif

/**
 * return 0 ,if AT24C02 no respond
 * return 1 ,if AT24C02 exist
 */
int is_at24c02_ready(void)
{
        return !(HAL_I2C_IsDeviceReady(&AT24C02_I2C_PORT,
                                (AT24C02_I2C_ADDR << 1), 3, 10));
}

int at24c02_init(void)
{
        CPRINTS("init");

        if (!is_at24c02_ready()) {
                CPRINTS("Can not found AT24C02!");
                return EC_ERROR_UNKNOWN;
        }

#ifdef CONFIG_EEPROM_AT24C02_SELF_TEST
        if (at24c02_read_write_test()) {
                return EC_ERROR_UNKNOWN;
        }
#endif

        return EC_SUCCESS;
}

#endif /* CONFIG_EEPROM_AT24C02 */
