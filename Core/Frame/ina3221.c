#include "console.h"
#include "ina3221.h"
#include "util.h"

#ifdef CONFIG_INA3221_DEBUG
#define CPRINTF(format, args...) PRINTF("INA3221: " format, ##args)
#else
#define CPRINTF(format, args...) 
#endif

/* INA3221 - Ti current sense amplifiers */


static int ina3221_set_reg_pointer(uint8_t reg)
{
	uint8_t re;

	re = HAL_I2C_Master_Transmit(&INA3221_I2C_PORT, (INA3221_I2C_ADDR << 1),
						&reg, sizeof(reg), 1000);

	if (re != HAL_OK)
		CPRINTF("setup reg pointer fail! (%d)\r\n", re);

	return re;
}

static int ina3221_read16(uint16_t reg, uint16_t *data)
{
	int re;
	uint16_t val;

	ina3221_set_reg_pointer(reg);

	re = HAL_I2C_Master_Receive(&INA3221_I2C_PORT, (INA3221_I2C_ADDR << 1),
					(uint8_t *) &val, sizeof(val), 1000);

	if (re != HAL_OK) {
		CPRINTF("read16 reg:0x%02x fail!\r\n", reg);
		return re;
	}

        *data =  (val >> 8) | ((val & 0xff) << 8);
        return EC_SUCCESS;
}

static int ina3221_write16(uint16_t reg, uint16_t data)
{
	int re;
	uint16_t val = (data >> 8) | ((data & 0xff) << 8);

	ina3221_set_reg_pointer(reg);

	re = HAL_I2C_Master_Transmit(&INA3221_I2C_PORT, (INA3221_I2C_ADDR << 1),
						(uint8_t*) &val, sizeof(val), 1000);

	if (re)
		CPRINTF("write16 reg:0x%02x data:0x%04x fail!\r\n", reg, data);

	return re;
}

int ina3221_gat_data(enum ina3221_ch_id ch, int *bus_curr, int *bus_volt)
{
	int re;
	uint16_t volt, shunt_volt, rsen;
	uint16_t shunt_volt_reg ,bus_volt_reg;

	shunt_volt_reg = INA3221_REG_SHUNT_VOLT + (2*(ch));
	bus_volt_reg = shunt_volt_reg + 1;

	re = ina3221_read16(shunt_volt_reg, &volt);

	if (re) {
		return EC_ERROR_UNKNOWN;
	}

	//CPRINTF("read16 SHUNT_VOLT 0x%02x (%d)\r\n", volt, volt);

	shunt_volt = INA3221_SHUNT_UV((int)volt);

	
	switch (ch)
	{
	case INA3221_CH1:
		rsen = INA3221_CH1_RSEN_MOHM;
		break;
	case INA3221_CH2:
		rsen = INA3221_CH2_RSEN_MOHM;
		break;
	case INA3221_CH3:
		rsen = INA3221_CH3_RSEN_MOHM;
		break;
	default:
		break;
	}

	//  curr (mA) = shunt_volt (uV) / Rsen (mohm)
	*bus_curr =  shunt_volt / rsen;

	re = ina3221_read16(bus_volt_reg, &volt);

	if (re) {
		return EC_ERROR_UNKNOWN;
	}

	//CPRINTF("read16 BUS_VOLT 0x%02x (%d)\r\n", volt, volt);

	*bus_volt = INA3221_BUS_MV(volt);

	return re;
}

int ina3221_reset(void)
{
	uint16_t config = INA3221_CONFIG_REG | INA3221_REG_RST;

	CPRINTF("reset\r\n");

	return ina3221_write16(INA3221_REG_CONFIG, config);
}

/**
 * return 0 ,if ina3221 no respond
 * return 1 ,if ina3221 exist
 */
int is_ina3221_ready(void)
{
	return !(HAL_I2C_IsDeviceReady(&INA3221_I2C_PORT, (INA3221_I2C_ADDR << 1), 3, 10));
}

int ina3221_init(void)
{
	CPRINTF("init\r\n");

	if (!is_ina3221_ready()) {
		CPRINTF("Can not found ina3221!\r\n");
		return EC_ERROR_UNKNOWN;
	}

	return ina3221_write16(INA3221_REG_CONFIG, INA3221_CONFIG_REG);
}

/******************************************************************************/
/* ina3221 console command */



#ifdef CONFIG_INA3221_COMMAND
int ina3221_print(void)
{
	int volt = 0, curr = 0, watt = 0;

	PRINTF("ina3221 read: \r\n");

	for(int i=0; i<INA3221_CH_COUNT; i++) {

		ina3221_gat_data(i, &curr, &volt);

		watt = (ABS(volt) * curr) / 1000;

		PRINTF(" Ch%d %5dmV, %4dmA, %5dmW\r\n" ,(i+1), volt, curr, watt);
	}

	return EC_SUCCESS;
}

#endif /* CONFIG_INA3221_COMMAND */



