#!/bin/bash

# fan_rpm_target 2000
# fan_temp_target 45
# setup_item[4] 00 01 01 02

DATA=(0x32 0x01 0x00 0x01 0x01 0x02 0x00 0x08 0xd0 0x07 0x00 0x00 0x2d 0x00 0x00 0x00)
EEPROM_ADDR_7BIT="0x50"

for((i=0; i<${#DATA[@]}; i=i+1))
do
    # printf "i2cwrite 0x50 0x00 0xff\n" > /dev/ttyUSB0
    printf "i2cwrite ${EEPROM_ADDR_7BIT} 0x%X ${DATA[i]}\n" ${i} > /dev/ttyUSB0
    sleep 1
done

printf "eeprom 60\n" > /dev/ttyUSB0


