/********************************************************************************************************
 * @file    TL322X_C1T382A102.h
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
#ifndef VENDOR_COMMON_BOARDS_TL322X_C1T382A102_H_
#define VENDOR_COMMON_BOARDS_TL322X_C1T382A102_H_

#if (defined(HOST_V2_ENABLE))
#define STACK_SUPPORT_FLASH_PROTECTION_ENABLE      1
#define LL_FEATURE_SUPPORT_LE_EXTENDED_ADVERTISING 0
#define LL_FEATURE_SUPPORT_LE_EXTENDED_SCANNING    0
#define LL_FEATURE_SUPPORT_LE_EXTENDED_INITIATE    1
#define LL_FEATURE_SUPPORT_LE_PERIODIC_ADVERTISING_SYNC 0
#define LL_FEATURE_SUPPORT_LE_PAST_SENDER          0
#define LL_FEATURE_SUPPORT_LE_PERIODIC_ADVERTISING 0
#define LL_FEATURE_SUPPORT_LE_PAST_RECIPIENT       0
#define OS_SUP_EN                                  0
#endif
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
     |  Button  |   SW2   |   SW3   |   SW4   |   SW5   |
     +----------+---------+---------+---------+---------+
     | Function |   Vol+  |   Vol-  |   Pair  |  Unpair |
     +----------+---------+---------+---------+---------+
     */
    #define KB_DRIVE_PINS {GPIO_PB2, GPIO_PC4}
    #define KB_SCAN_PINS  {GPIO_PB0, GPIO_PB1}

    //drive pin as gpio
    #define PB2_FUNC AS_GPIO
    #define PC4_FUNC AS_GPIO

    //drive pin need 100K pulldown
    #define PULL_WAKEUP_SRC_PB2 MATRIX_ROW_PULL
    #define PULL_WAKEUP_SRC_PC4 MATRIX_ROW_PULL

    //drive pin open input to read gpio wakeup level
    #define PB2_INPUT_ENABLE 1
    #define PC4_INPUT_ENABLE 1

    //scan pin as gpio
    #define PB0_FUNC AS_GPIO
    #define PB1_FUNC AS_GPIO

    //scan  pin need 10K pullup
    #define PULL_WAKEUP_SRC_PB0 MATRIX_COL_PULL
    #define PULL_WAKEUP_SRC_PB1 MATRIX_COL_PULL

    //scan pin open input to read gpio level
    #define PB0_INPUT_ENABLE 1
    #define PB1_INPUT_ENABLE 1
#endif

/**
 *  @brief  LED Configuration
 */
#if UI_LED_ENABLE
    /**
     *  @brief  Definition gpio for led
     */
    #define GPIO_LED_RED      GPIO_PC3
    #define GPIO_LED_GREEN    GPIO_PC2
    #define GPIO_LED_BLUE     GPIO_PC1
    #define GPIO_LED_WHITE    GPIO_PC0


    #define PC3_FUNC          AS_GPIO
    #define PC2_FUNC          AS_GPIO
    #define PC1_FUNC          AS_GPIO
    #define PC0_FUNC          AS_GPIO

    #define PC3_OUTPUT_ENABLE 1
    #define PC2_OUTPUT_ENABLE 1
    #define PC1_OUTPUT_ENABLE 1
    #define PC0_OUTPUT_ENABLE 1

    #define LED_ON_LEVEL      1 //gpio output high voltage to turn on led
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
        #define ADC_INPUT_PIN_CHN_P SD_ADC_GPIO_PB4P
        #define ADC_INPUT_PIN_CHN_N SD_ADC_GNDN
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

/**
 *  @brief  Antenna Switch Configuration
 */
#ifndef ANTENNA_SWITCHING_AUTO_EN
    #define ANTENNA_SWITCHING_AUTO_EN 1
#endif

#ifndef NUM_ANT_SUPPORT
    #define NUM_ANT_SUPPORT 0x02
#endif

#ifndef MAX_ANT_PATHS_SUPPORT
    #define MAX_ANT_PATHS_SUPPORT 0X04
#endif

/**
 *  @brief  Antenna switch configuration for channel sounding
 */
#define ANTENNA_SWITCHING_SEL_0_PIN GPIO_PB3
#define ANTENNA_SWITCHING_SEL_1_PIN GPIO_PD0
#define ANTENNA_SWITCHING_CTRL_BASE     (0x11111111)


#define TLKAPI_DEBUG_GPIO_PIN        GPIO_PF4
#define TLKAPI_DEBUG_GSUART_BAUDRATE 1000000

#ifndef DEBUG_GPIO_CHAN_ENABLE
    #define DEBUG_GPIO_CHAN_ENABLE 0
#endif


#if DEBUG_CS_GPIO_ENABLE
    #define GPIO_CHN0         GPIO_PD0
    #define GPIO_CHN1         GPIO_PG0
    #define GPIO_CHN2         GPIO_PG1
    #define GPIO_CHN3         GPIO_PG2
    #define GPIO_CHN4         GPIO_PG7
    #define GPIO_CHN5         GPIO_PH0
    #define GPIO_CHN6         GPIO_PD6
    #define GPIO_CHN7         GPIO_PD7

    #define GPIO_CHN8         GPIO_PD4
    #define GPIO_CHN9         GPIO_PD5
//    #define GPIO_CHN10        GPIO_PA1
//    #define GPIO_CHN11        GPIO_PA0
    #define GPIO_CHN12        GPIO_PB7
    #define GPIO_CHN13        GPIO_PA2
//    #define GPIO_CHN14        GPIO_PD0
//    #define GPIO_CHN15        GPIO_PD1


    #define PD0_OUTPUT_ENABLE 1
    #define PG0_OUTPUT_ENABLE 1
    #define PG1_OUTPUT_ENABLE 1
    #define PG2_OUTPUT_ENABLE 1
    #define PG7_OUTPUT_ENABLE 1
    #define PH0_OUTPUT_ENABLE 1
    #define PD6_OUTPUT_ENABLE 1
    #define PD7_OUTPUT_ENABLE 1

    #define PD4_OUTPUT_ENABLE 1
    #define PD5_OUTPUT_ENABLE 1
//    #define PA1_OUTPUT_ENABLE 1
//    #define PA0_OUTPUT_ENABLE 1
    #define PB7_OUTPUT_ENABLE 1
    #define PA2_OUTPUT_ENABLE 1
//    #define PD0_OUTPUT_ENABLE 1
//    #define PD1_OUTPUT_ENABLE 1

#endif

#if DEBUG_GPIO_CHAN_ENABLE
    #define gpio_write(pin, value) gpio_set_level(pin, value)

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

    #define DBG_CHN0_LOW           gpio_write(GPIO_CHN0, 0)
    #define DBG_CHN0_HIGH          gpio_write(GPIO_CHN0, 1)
    #define DBG_CHN0_TOGGLE        gpio_toggle(GPIO_CHN0)

    #define DBG_CHN1_LOW           gpio_write(GPIO_CHN1, 0)
    #define DBG_CHN1_HIGH          gpio_write(GPIO_CHN1, 1)
    #define DBG_CHN1_TOGGLE        gpio_toggle(GPIO_CHN1)

    #define DBG_CHN2_LOW           gpio_write(GPIO_CHN2, 0)
    #define DBG_CHN2_HIGH          gpio_write(GPIO_CHN2, 1)
    #define DBG_CHN2_TOGGLE        gpio_toggle(GPIO_CHN2)

    #define DBG_CHN3_LOW           gpio_write(GPIO_CHN3, 0)
    #define DBG_CHN3_HIGH          gpio_write(GPIO_CHN3, 1)
    #define DBG_CHN3_TOGGLE        gpio_toggle(GPIO_CHN3)

    #define DBG_CHN4_LOW           gpio_write(GPIO_CHN4, 0)
    #define DBG_CHN4_HIGH          gpio_write(GPIO_CHN4, 1)
    #define DBG_CHN4_TOGGLE        gpio_toggle(GPIO_CHN4)

    #define DBG_CHN5_LOW           gpio_write(GPIO_CHN5, 0)
    #define DBG_CHN5_HIGH          gpio_write(GPIO_CHN5, 1)
    #define DBG_CHN5_TOGGLE        gpio_toggle(GPIO_CHN5)

    #define DBG_CHN6_LOW           gpio_write(GPIO_CHN6, 0)
    #define DBG_CHN6_HIGH          gpio_write(GPIO_CHN6, 1)
    #define DBG_CHN6_TOGGLE        gpio_toggle(GPIO_CHN6)

    #define DBG_CHN7_LOW           gpio_write(GPIO_CHN7, 0)
    #define DBG_CHN7_HIGH          gpio_write(GPIO_CHN7, 1)
    #define DBG_CHN7_TOGGLE        gpio_toggle(GPIO_CHN7)

    #define DBG_CHN8_LOW           gpio_write(GPIO_CHN8, 0)
    #define DBG_CHN8_HIGH          gpio_write(GPIO_CHN8, 1)
    #define DBG_CHN8_TOGGLE        gpio_toggle(GPIO_CHN8)

    #define DBG_CHN9_LOW           gpio_write(GPIO_CHN9, 0)
    #define DBG_CHN9_HIGH          gpio_write(GPIO_CHN9, 1)
    #define DBG_CHN9_TOGGLE        gpio_toggle(GPIO_CHN9)

    #define DBG_CHN10_LOW          gpio_write(GPIO_CHN10, 0)
    #define DBG_CHN10_HIGH         gpio_write(GPIO_CHN10, 1)
    #define DBG_CHN10_TOGGLE       gpio_toggle(GPIO_CHN10)

    #define DBG_CHN11_LOW          gpio_write(GPIO_CHN11, 0)
    #define DBG_CHN11_HIGH         gpio_write(GPIO_CHN11, 1)
    #define DBG_CHN11_TOGGLE       gpio_toggle(GPIO_CHN11)

    #define DBG_CHN12_LOW          gpio_write(GPIO_CHN12, 0)
    #define DBG_CHN12_HIGH         gpio_write(GPIO_CHN12, 1)
    #define DBG_CHN12_TOGGLE       gpio_toggle(GPIO_CHN12)

    #define DBG_CHN13_LOW          gpio_write(GPIO_CHN13, 0)
    #define DBG_CHN13_HIGH         gpio_write(GPIO_CHN13, 1)
    #define DBG_CHN13_TOGGLE       gpio_toggle(GPIO_CHN13)

    #define DBG_CHN14_LOW          gpio_write(GPIO_CHN14, 0)
    #define DBG_CHN14_HIGH         gpio_write(GPIO_CHN14, 1)
    #define DBG_CHN14_TOGGLE       gpio_toggle(GPIO_CHN14)

    #define DBG_CHN15_LOW          gpio_write(GPIO_CHN15, 0)
    #define DBG_CHN15_HIGH         gpio_write(GPIO_CHN15, 1)
    #define DBG_CHN15_TOGGLE       gpio_toggle(GPIO_CHN15)
#endif /* DEBUG_GPIO_CHAN_ENABLE */

#endif /* VENDOR_COMMON_BOARDS_TL322X_C1T382A102_H_ */
