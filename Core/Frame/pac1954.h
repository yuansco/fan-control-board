#ifndef __PAC1954_H
#define __PAC1954_H

#include "main.h"
#include <stdint.h>

/* should define in board.h if needed */
//#define CONFIG_ADC_PAC1954
//#define CONFIG_ADC_PAC1954_DEBUG
//#define CONFIG_ADC_PAC1954_COMMAND
//#define CONFIG_ADC_PAC1954_TEST


extern I2C_HandleTypeDef hi2c1;

#ifndef PAC1954_I2C_PORT
#define PAC1954_I2C_PORT hi2c1
#endif

#ifndef PAC1954_I2C_ADDR
#define PAC1954_I2C_ADDR (0x10 << 1)
#endif

#define PAC1954_CHANNEL_COUNT 4

#define PAC1954_REFRESH_CMD         0x00
#define PAC1954_CTRL_REG            0x01
#define PAC1954_VBUS1_REG           0x07
#define PAC1954_VSENSE1_REG         0x0B

#define PAC1954_I2C_TIMEOUT         100
#define PAC1954_REFRESH_DELAY_MS    2

#define VBUS_FSR                    32.0f
#define VSENSE_FSR                  0.1f
#define ADC_RESOLUTION              65535.0f

typedef struct {
        float voltage;
        float current;
} PAC1954_Measurements;

typedef struct {
        I2C_HandleTypeDef* i2c_handle;
        uint16_t           device_address;
} PAC1954_Handle;

// Shunt resistor values ​​for the four channels (unit: ohms)
// For example, 10 mΩ for each
extern float shunt_resistors[PAC1954_CHANNEL_COUNT];
// PAC1954 driver example
extern PAC1954_Handle pac1954_dev;

// Array for storing measurement results
extern PAC1954_Measurements channel_data[PAC1954_CHANNEL_COUNT];

int pac1954_Init(void);
int pac1954_get_voltage_current(void);
void pac1954_print(void);

#endif // __PAC1954_H
