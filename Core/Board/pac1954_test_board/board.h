
#ifndef __BOARD_H
#define __BOARD_H

/* pac1954_test_board */

#define BOARDNAME "pac1954_test_board"


/**
 * Support debug print and console command
 */
#define CONFIG_UART_DEBUG
#define CONFIG_CONSOLE_COMMAND
#define CONFIG_CONSOLE_BOARD_COMMAND

/**
 * Support Task
 */
#define CONFIG_TASK
#define CONFIG_TASK_DEBUG
#define CONFIG_TASK_HOOK
#define CONFIG_TASK_HOOK_DYNAMIC
//#define CONFIG_TASK_HOOK_TEST
#define CONFIG_TASK_HOOK_COMMAND

/**
 *  Support I2C function
 * 
 *  Support I2C read, write, scan function.
 */
#define CONFIG_I2C
#define CONFIG_I2C_DEBUG
#define CONFIG_I2C_READ_COMMAND
#define CONFIG_I2C_WRITE_COMMAND
#define CONFIG_I2C_SCAN

/**
 * ADC PAC1954
 */
#define CONFIG_ADC_PAC1954
#define CONFIG_ADC_PAC1954_DEBUG
#define CONFIG_ADC_PAC1954_COMMAND
#define CONFIG_ADC_PAC1954_TEST

/**
 * High-Speed External Oscillator
 * 
 */
#define OSC_16M

#undef HSE_VALUE
#ifdef OSC_16M
#define HSE_VALUE    (16000000UL)  
#else
#define HSE_VALUE    (8000000UL)   
#endif


#endif /* __BOARD_H */

