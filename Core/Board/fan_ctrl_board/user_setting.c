#include "at24c02.h"
#include "board.h"
#include "ch224k.h"
#include "console.h"
#include "fan.h"
#include "fan_control.h"
#include "page_control.h"

#include <string.h>

#ifdef CONFIG_BOARD_INFO_DEBUG
#define CPRINTF(format, args...) PRINTF("DATA: " format, ##args)
#define CPRINTS(format, args...) PRINTS("DATA: " format, ##args)
#else
#define CPRINTF(format, args...)
#define CPRINTS(format, args...)
#endif

/**
 *  save user setting to EEPROM and loading it when reboot
 */

struct eeprom_data def_data = {
        /* data_id */
        .data_id = 0x32,
        /* eeprom data format version */
        .version = 1,
        /* default setup_item:  */
        .setup_item = {0, 1, 1 ,2},
        /* default fan target rpm */
        .fan_rpm_target = 2000,
        /* default fan target temperature */
        .fan_temp_target = 50,
};


/* set setup_item */
void save_setting(void) {

        int fan_rpm_target = fan_control_get_target_rpm();
        int fan_temp_target = fan_control_get_target_temp();

        CPRINTS("save setting...");

        // CPRINTS("fan_rpm_target:%d", fan_rpm_target);
        // CPRINTS("fan_temp_target:%d", fan_temp_target);

        /* setup setup_item */
        ch224k_select_pd_power(setup_item[0]);
        fan_set_duty_step(FAN_1, setup_subitem_pwm_step[setup_item[1]].value);
        fan_set_rpm_step(setup_subitem_rpm_step[setup_item[2]].value);
        fan_set_temp_step(setup_subitem_temp_step[setup_item[3]].value);

        /* save setup_item to eeprom */
        struct eeprom_data curr_data = {
                .data_id = 0x32,                    // data_id = 0x32
                .version = 1,                       // eeprom data format version
                .setup_item = {0},                  // default setup_item
                .fan_rpm_target = fan_rpm_target,   // default fan target rpm
                .fan_temp_target = fan_temp_target, // default fan target temperature
        };

        // default setup_item
        memcpy(curr_data.setup_item, setup_item, ARRAY_SIZE(setup_item));

        // for(int e=0; e<sizeof(curr_data); e++) {
        //         PRINTF("%02x ",buf[e]);
        // }
        // PRINTF("\n");

        /* convert struct to byte array */ 
        uint8_t buf[sizeof(curr_data)];
        memcpy(&buf, &curr_data, sizeof(curr_data));

        for(int e=0; e<sizeof(buf); e++) {
                PRINTF("%02x ",buf[e]);
        }
        CPRINTS("");

        at24c02_write(0, buf, sizeof(buf));

        CPRINTS("save_setting Done!");
}

int verify_data_header(void) {

        uint8_t buf[] = {0x32, 0x01}; // header
        uint8_t buf_read[sizeof(buf)];

        at24c02_read(0, buf_read, sizeof(buf_read));

        for (int i = 0; i < sizeof(buf); i++) {
                if (buf[i] != buf_read[i]) {
                        CPRINTS("eeprom data verify fail! EEPROM[%02x] is %02x, not %02x",
                                        i, buf_read[i], buf[i]);
                        return EC_ERROR_UNKNOWN;
                }
        }
        return EC_SUCCESS;
}

/* load setting data from EEPROM */
void load_setting(void) {

        CPRINTS("load setting...");

        /* Do not use EEPROM data if verify header fail */
        if (verify_data_header() == EC_SUCCESS) {

                /* load EEPROM data to def_data */
                uint8_t buf[sizeof(def_data)];

                /* read data from EEPROM */
                at24c02_read(0, buf, sizeof(buf));

                /* convert byte array to struct */ 
                memcpy(&def_data, buf, sizeof(def_data));
        }

        CPRINTS("fan_rpm_target %d", def_data.fan_rpm_target);
        CPRINTS("fan_temp_target %d", def_data.fan_temp_target);
        fan_control_set_target_rpm(def_data.fan_rpm_target);
        fan_control_set_target_temp(def_data.fan_temp_target);

        /* setup setup_item */
        for(int i=0; i<4; i++) {
                CPRINTS("setup_item[%d]:%02x", i, def_data.setup_item[i]);
                setup_item[i] = def_data.setup_item[i];
        }

        ch224k_select_pd_power(setup_item[0]);
        fan_set_duty_step(FAN_1, setup_subitem_pwm_step[setup_item[1]].value);
        fan_set_rpm_step(setup_subitem_rpm_step[setup_item[2]].value);
        fan_set_temp_step(setup_subitem_temp_step[setup_item[3]].value);
}
