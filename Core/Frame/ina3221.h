
// INA3221 - Ti current sense amplifiers

#ifndef __INA3221_H__
#define __INA3221_H__

#include <stdint.h>

#include "main.h"

/* should define in board.h if needed */
//#define CONFIG_INA3221
//#define CONFIG_INA3221_DEBUG
//#define CONFIG_INA3221_COMMAND


#define INA3221_ADDR_0 0x40 // A0 connected to GND
#define INA3221_ADDR_1 0x41 // A0 connected to VS
#define INA3221_ADDR_2 0x42 // A0 connected to SDA
#define INA3221_ADDR_3 0x43 // A0 connected to SCL

extern I2C_HandleTypeDef hi2c1;

#ifndef INA3221_I2C_PORT
#define INA3221_I2C_PORT hi2c1
#endif

#ifndef INA3221_I2C_ADDR
#define INA3221_I2C_ADDR INA3221_ADDR_0
#endif

#define INA3221_REG_CONFIG         0x00
#define INA3221_REG_SHUNT_VOLT     0x01
#define INA3221_REG_BUS_VOLT       0x02
#define INA3221_REG_MNFT_ID        0xFE
#define INA3221_REG_DIE_ID         0xFF

/* current sense resistor */

#ifndef INA3221_CH1_RSEN_MOHM
#define INA3221_CH1_RSEN_MOHM 30
#endif

#ifndef INA3221_CH2_RSEN_MOHM
#define INA3221_CH2_RSEN_MOHM 30
#endif

#ifndef INA3221_CH3_RSEN_MOHM
#define INA3221_CH3_RSEN_MOHM 30
#endif

enum ina3221_ch_id {
        INA3221_CH1 = 0,
        INA3221_CH2,
        INA3221_CH3,
        INA3221_CH_COUNT
};

/* 
 * Configuration Register Field :
 * Reset = disable
 * CH1en = enable
 * CH2en = disable
 * CH3en = disable
 * Average = 1
 * VBUSCT = 1.1 ms
 * VSHCT = 1.1 ms
 * Mode = shunt and bus, continuous
 */
#define INA3221_CONFIG_REG	0x4127

// 0: disable reset
// 1: enable reset
#define INA3221_REG_RST BIT(15)

/* Bus voltage: lower 3 bits clear, LSB = 8 mV */
#define INA3221_BUS_MV(reg) (reg)
/* Shunt voltage: lower 3 bits clear, LSB = 40 uV */
#define INA3221_SHUNT_UV(reg) ((reg) * (40/8))


int ina3221_gat_data(enum ina3221_ch_id ch, int *bus_curr, int *bus_volt);
int ina3221_print(void);
int ina3221_reset(void);
int ina3221_init(void);



#endif // __INA3221_H__