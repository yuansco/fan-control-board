#include "console.h"
#include "pac1954.h"
#include "util.h"

#ifdef CONFIG_ADC_PAC1954

#ifdef CONFIG_ADC_PAC1954_DEBUG
#define CPRINTF(format, args...) PRINTF("PAC1954: " format, ##args)
#define CPRINTS(format, args...) PRINTS("PAC1954: " format, ##args)
#else
#define CPRINTF(format, args...)
#define CPRINTS(format, args...)
#endif


float shunt_resistors[PAC1954_CHANNEL_COUNT] = {0.01f, 0.01f, 0.01f, 0.01f};

PAC1954_Handle pac1954_dev;

PAC1954_Measurements channel_data[PAC1954_CHANNEL_COUNT];

int pac1954_refresh(void) {

        uint8_t cmd = PAC1954_REFRESH_CMD;

        CPRINTS("Refresh");

        return HAL_I2C_Master_Transmit(&PAC1954_I2C_PORT, PAC1954_I2C_ADDR, &cmd, 1, PAC1954_I2C_TIMEOUT);
}

int pac1954_get_voltage_current(void) {
        
        int status;

        uint8_t vbus_buffer[PAC1954_CHANNEL_COUNT * 2];
        status = HAL_I2C_Mem_Read(&PAC1954_I2C_PORT, PAC1954_I2C_ADDR, PAC1954_VBUS1_REG, I2C_MEMADD_SIZE_8BIT, vbus_buffer, sizeof(vbus_buffer), PAC1954_I2C_TIMEOUT);
        
        if (status != EC_SUCCESS) {
                return status;
        }

        uint8_t vsense_buffer[PAC1954_CHANNEL_COUNT * 2];
        status = HAL_I2C_Mem_Read(&PAC1954_I2C_PORT, PAC1954_I2C_ADDR, PAC1954_VSENSE1_REG, I2C_MEMADD_SIZE_8BIT, vsense_buffer, sizeof(vsense_buffer), PAC1954_I2C_TIMEOUT);
        if (status != EC_SUCCESS) {
                return status;
        }

        for (int i = 0; i < PAC1954_CHANNEL_COUNT; ++i) {
                uint16_t raw_vbus = (uint16_t)((vbus_buffer[i * 2] << 8) | vbus_buffer[i * 2 + 1]);
                uint16_t raw_vsense = (uint16_t)((vsense_buffer[i * 2] << 8) | vsense_buffer[i * 2 + 1]);

                channel_data[i].voltage = (raw_vbus / ADC_RESOLUTION) * VBUS_FSR;
                float vsense_voltage = (raw_vsense / ADC_RESOLUTION) * VSENSE_FSR;

                if (shunt_resistors[i] > 0.0f) {
                        channel_data[i].current = vsense_voltage / shunt_resistors[i];
                } else {
                        channel_data[i].current = 0.0f;
                }
        }

        return EC_SUCCESS;
}

void pac1954_print(void) {

        int status;

        status = pac1954_get_voltage_current();

        if (status == EC_SUCCESS) {
                CPRINTS("Channel 1 -> Voltage: %.3f V, Current: %.3f A", channel_data[0].voltage, channel_data[0].current);
                CPRINTS("Channel 2 -> Voltage: %.3f V, Current: %.3f A", channel_data[1].voltage, channel_data[1].current);
                CPRINTS("Channel 3 -> Voltage: %.3f V, Current: %.3f A", channel_data[2].voltage, channel_data[2].current);
                CPRINTS("Channel 4 -> Voltage: %.3f V, Current: %.3f A", channel_data[3].voltage, channel_data[3].current);

        } else {
                CPRINTS("Failed to read from PAC1954!");
        }
}

int pac1954_Init(void) {

        int status;

        status = HAL_I2C_IsDeviceReady(&PAC1954_I2C_PORT, PAC1954_I2C_ADDR, 2, PAC1954_I2C_TIMEOUT);

        if (status != EC_SUCCESS) {
                CPRINTS("Can not found device!");
                return HAL_ERROR;
        }

        /*
        High Byte (MSB) = 0x07 (Binary 0000 0111):

        SAMPLE_MODE[15:12]: Set to 0000, representing the selection of "1024 SPS adaptive accumulation"
                            (1024 samples per second adaptive accumulation mode), which is the default
                            sampling mode.
        GPIO_ALERT2[11:10]: Set to 01, representing the configuration of the GPIO/ALERT2 pin as a GPIO
                            digital input function.
        SLOW_ALERT1[9:8]: Set to 11, representing the configuration of the SLOW/ALERT1 pin as a SLOW
                          function. When this pin is pulled high, the sampling rate will decrease to 8 SPS.

        Low Byte (LSB) = 0x00 (Binary 0000 0000):

        CHANNEL_N_OFF[7:4]: Set to 0000, representing that channels 1, 2, 3, and 4 are all enabled.
        Bits 3-0: These are unused bits, which read as '0'.
        */
        uint8_t ctrl_data[2] = {0x07, 0x00};
        status = HAL_I2C_Mem_Write(&PAC1954_I2C_PORT, PAC1954_I2C_ADDR, PAC1954_CTRL_REG, I2C_MEMADD_SIZE_8BIT, ctrl_data, 2, PAC1954_I2C_TIMEOUT);

        if (status != EC_SUCCESS) {
                CPRINTS("Set CTRL register fail!");
                return status;
        }

        status = pac1954_refresh();

        if (status != EC_SUCCESS) {
                CPRINTS("Refresh fail!");
                return status;
        }
        HAL_Delay(PAC1954_REFRESH_DELAY_MS);

#ifdef CONFIG_ADC_PAC1954_TEST
        pac1954_print();
#endif /* CONFIG_ADC_PAC1954_TEST */

        return EC_SUCCESS;
}

#endif /* CONFIG_ADC_PAC1954 */
