/********************************************************************************************************
 * @file    TL322X_C1T382A20.h
 *
 * @brief   This is the header file for BLE SDK
 *
 * @author  BLE GROUP
 * @date    06,2022
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
#ifndef VENDOR_COMMON_BOARDS_TL322X_C1T382A20_H_
#define VENDOR_COMMON_BOARDS_TL322X_C1T382A20_H_

/**
 *  @brief  Keyboard Configuration
 */
#if (UI_KEYBOARD_ENABLE)
    #define MATRIX_ROW_PULL    GPIO_PIN_PULLDOWN_100K
    #define MATRIX_COL_PULL    GPIO_PIN_PULLUP_10K

    #define KB_LINE_HIGH_VALID 0 //drive pin output 0 when scan key, scan pin read 0 is valid

    #define BTN_PAIR           0x01
    #define BTN_UNPAIR         0x02

    #define CR_VOL_UP          0xf0 ////
    #define CR_VOL_DN          0xf1

    /**
     *  @brief  Normal keyboard map
     */
    #define KB_MAP_NORMAL {     \
        {CR_VOL_UP, CR_VOL_DN }, \
        {BTN_PAIR,  BTN_UNPAIR}, \
    }

    //////////////////// KEY CONFIG (EVK board) ///////////////////////////
    /* Description
     +--------------------------------------------------+
     |  Button  |   SW3   |   SW6   |   SW4   |   SW7   |
     +----------+---------+---------+---------+---------+
     | Function |   Vol+  |   Vol-  |   Pair  |  Unpair |
     +----------+---------+---------+---------+---------+
     */
    #define KB_DRIVE_PINS {GPIO_PF6, GPIO_PF7}
    #define KB_SCAN_PINS  {GPIO_PF4, GPIO_PF5}

    //drive pin as gpio
    #define PF6_FUNC AS_GPIO
    #define PF7_FUNC AS_GPIO

    //drive pin need 100K pulldown
    #define PULL_WAKEUP_SRC_PF6 MATRIX_ROW_PULL
    #define PULL_WAKEUP_SRC_PF7 MATRIX_ROW_PULL

    //drive pin open input to read gpio wakeup level
    #define PF6_INPUT_ENABLE 1
    #define PF7_INPUT_ENABLE 1

    //scan pin as gpio
    #define PF4_FUNC AS_GPIO
    #define PF5_FUNC AS_GPIO

    //scan  pin need 10K pullup
    #define PULL_WAKEUP_SRC_PF4 MATRIX_COL_PULL
    #define PULL_WAKEUP_SRC_PF5 MATRIX_COL_PULL

    //scan pin open input to read gpio level
    #define PF4_INPUT_ENABLE 1
    #define PF5_INPUT_ENABLE 1
#endif

/**
 *  @brief  LED Configuration
 */
#if UI_LED_ENABLE
    /**
     *  @brief  Definition gpio for led
     */
    #define GPIO_LED_RED      GPIO_PC4
    #define GPIO_LED_GREEN    GPIO_PC7
    #define GPIO_LED_BLUE     GPIO_PC5
    #define GPIO_LED_WHITE    GPIO_PC6


    #define PC4_FUNC          AS_GPIO
    #define PC5_FUNC          AS_GPIO
    #define PC6_FUNC          AS_GPIO
    #define PC7_FUNC          AS_GPIO

    #define PC4_OUTPUT_ENABLE 1
    #define PC5_OUTPUT_ENABLE 1
    #define PC6_OUTPUT_ENABLE 1
    #define PC7_OUTPUT_ENABLE 1

    #define LED_ON_LEVEL      1 //gpio output high voltage to turn on led
#endif

#ifndef JTAG_DEBUG_DISABLE
    #define JTAG_DEBUG_DISABLE 1
#endif
/**
 *  @brief  GPIO definition for JTAG
 */
#if (0) //(JTAG_DEBUG_DISABLE)
    //JTAG will cost some power
    #define PD4_FUNC            AS_GPIO
    #define PD5_FUNC            AS_GPIO
    #define PD6_FUNC            AS_GPIO
    #define PD7_FUNC            AS_GPIO

    #define PD4_INPUT_ENABLE    0
    #define PD5_INPUT_ENABLE    0
    #define PD6_INPUT_ENABLE    0
    #define PD7_INPUT_ENABLE    0

    #define PULL_WAKEUP_SRC_PD4 0
    #define PULL_WAKEUP_SRC_PD5 0
    #define PULL_WAKEUP_SRC_PD6 0
    #define PULL_WAKEUP_SRC_PD7 0
#endif

/**
 *  @brief  Battery_check Configuration
 */
#if (BATT_CHECK_ENABLE)
    #define VBAT_CHANNEL_EN 0

    #if VBAT_CHANNEL_EN
        /**     The battery voltage sample range is 1.8~3.5V    **/
    #else
        /**     if the battery voltage > 3.6V, should take some external voltage divider    **/
        #define ADC_INPUT_PIN_CHN ADC_GPIO_PB0
    #endif
#endif


/**
 *  @brief  GPIO definition for debug_io
 */
#if (DEBUG_GPIO_ENABLE)
    #define GPIO_CHN0              GPIO_PC0
    #define GPIO_CHN1              GPIO_PC1
    #define GPIO_CHN2              GPIO_PC2
    #define GPIO_CHN3              GPIO_PC3
    #define GPIO_CHN4              GPIO_PD4
    #define GPIO_CHN5              GPIO_PD5
    #define GPIO_CHN6              GPIO_PD6
    #define GPIO_CHN7              GPIO_PD7

    #define GPIO_CHN8              GPIO_PH0
    #define GPIO_CHN9              GPIO_PH1
    #define GPIO_CHN10             GPIO_PH2
    #define GPIO_CHN11             GPIO_PH3
    #define GPIO_CHN12             GPIO_PH4
    #define GPIO_CHN13             GPIO_PH5
    #define GPIO_CHN14             GPIO_PH6
    #define GPIO_CHN15             GPIO_PH7

    #define PC0_OUTPUT_ENABLE      1
    #define PC1_OUTPUT_ENABLE      1
    #define PC2_OUTPUT_ENABLE      1
    #define PC3_OUTPUT_ENABLE      1
    #define PD4_OUTPUT_ENABLE      1
    #define PD5_OUTPUT_ENABLE      1
    #define PD6_OUTPUT_ENABLE      1
    #define PD7_OUTPUT_ENABLE      1

    #define PH0_OUTPUT_ENABLE      1
    #define PH1_OUTPUT_ENABLE      1
    #define PH2_OUTPUT_ENABLE      1
    #define PH3_OUTPUT_ENABLE      1
    #define PH4_OUTPUT_ENABLE      1
    #define PH5_OUTPUT_ENABLE      1
    #define PH6_OUTPUT_ENABLE      1
    #define PH7_OUTPUT_ENABLE      1
#endif //end of DEBUG_GPIO_ENABLE

#define TLKAPI_DEBUG_GPIO_PIN        GPIO_PE0
#define TLKAPI_DEBUG_GSUART_BAUDRATE 1000000

#endif /* VENDOR_COMMON_BOARDS_TL322X_C1T382A20_H_ */
