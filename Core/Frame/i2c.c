
#include "i2c.h"
#include "util.h"
#include <stdint.h>

#ifndef CONFIG_I2C
#undef CONFIG_I2C_DEBUG
#undef CONFIG_I2C_SCAN
#endif

#ifdef CONFIG_I2C_DEBUG
#define CPRINTF(format, args...) PRINTF("I2C: " format, ##args)
#else
#define CPRINTF(format, args...)
#endif


int i2c_read(uint16_t addr, uint8_t reg, uint8_t *data, uint16_t size) {

        int re;

        /* set offset*/
        re = HAL_I2C_Master_Transmit(&hi2c1, (addr << 1),
                                                &reg, sizeof(reg), 1000);

        if (re != EC_SUCCESS) {
                CPRINTF("i2c read failed\r\n");
                return re;
        }

        re = HAL_I2C_Master_Receive(&hi2c1, (addr << 1), data, size, 1000);

        if (re != EC_SUCCESS)
                CPRINTF("i2c read failed! (%d)\r\n", re);

        return re;
}

int i2c_write(uint16_t addr, uint8_t reg, uint8_t *data, uint16_t size) {

        int i, re;
        uint8_t data_write[size + 1];

        data_write[0] = reg;

        for (i = 0; i < size; i++)
                data_write[i + 1] = data[i];

        re = HAL_I2C_Master_Transmit(&hi2c1, (addr << 1),
                        data_write, sizeof(data_write), 1000);

        if (re != EC_SUCCESS)
                CPRINTF("i2c write failed! (%d)\n\r", re);

        return re;
}


/******************************************************************************/
/* I2C Scan function */

/**
 * scan 0x00 ~ 0x7f (0~127) 
 * retries: 2 times
 * timeout: 10 mS
 */

#ifdef CONFIG_I2C_SCAN

int i2c_scan(void) {

        uint8_t i, re;

        PRINTF("\r\nScanning I2C bus:\r\n");

        for (i = 1; i < 128; i++) {

                re = HAL_I2C_IsDeviceReady(&hi2c1, (i << 1),
                        I2C_SCAN_RETRIES, I2C_SCAN_TIMEOUT_MS);

                if (re == EC_SUCCESS) {
                        // received ACK
                        PRINTF("\r\n0x%02X\r\n", i);
                        continue;
                }

                // No ACK received at that address
                 PRINTF(".");
        }

        PRINTF("\r\n");
        return EC_SUCCESS;
}
#else
int i2c_scan(void) { return EC_SUCCESS; }
#endif /* CONFIG_I2C_SCAN */