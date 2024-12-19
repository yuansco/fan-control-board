
#ifndef __I2C_H__
#define __I2C_H__

#include "console.h"
#include "main.h"


/* should define in board.h if needed */
//#define CONFIG_I2C_DEBUG
//#define CONFIG_I2C_READ_COMMAND
//#define CONFIG_I2C_WRITE_COMMAND
//#define CONFIG_I2C_SCAN

#ifndef CONFIG_I2C
#undef CONFIG_I2C_DEBUG
#undef CONFIG_I2C_READ_COMMAND
#undef CONFIG_I2C_WRITE_COMMAND
#undef CONFIG_I2C_SCAN
#endif

extern I2C_HandleTypeDef hi2c1;


#define I2C_SCAN_TIMEOUT_MS 10
#define I2C_SCAN_RETRIES 2

int i2c_read(uint16_t addr, uint8_t reg, uint8_t *data, uint16_t size);
int i2c_write(uint16_t addr, uint8_t reg, uint8_t *data, uint16_t size);
int i2c_scan(void);

#endif /* __I2C_H__ */
