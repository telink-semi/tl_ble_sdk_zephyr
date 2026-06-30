/********************************************************************************************************
 * @file    cmpt.h
 *
 * @brief   This is the header file for tl322x
 *
 * @author  Driver Group
 * @date    2024
 *
 * @par     Copyright (c) 2024, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#ifndef DRIVERS_CMPT_H_
#define DRIVERS_CMPT_H_
#include "../gpio.h"
#include "../lib/include/stimer.h"

/**********************************************************************************************************************
 *                                         global constants                                                           *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *                                           global macro                                                             *
 *********************************************************************************************************************/


/**********************************************************************************************************************
 *                                         global data type                                                           *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *                                     global variable declaration                                                    *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *                                      global function prototype                                                     *
 *********************************************************************************************************************/


/**********************************************************************************************************************
 *                                     gpio  compatibility                                                            *
 *********************************************************************************************************************/

/**
 * @brief     This function set the pin's output level.
 * @param[in] pin - the pin needs to set its output level
 * @param[in] value - value of the output level(1: high 0: low)
 * @author    BLE group .
 * @return    none
 */
#define gpio_write(pin, value) gpio_set_level(pin, value)


/**
 * @brief      This function enable the output function of a pin.
 * @param[in]  pin - the pin needs to set the output function(1: enable,0: disable)
 * @author     BLE group .
 * @return     none
 */
#define gpio_set_output_en(pin, value) gpio_set_output(pin, value)


/**
 * @brief     This function read the pin's input/output level.
 * @param[in] pin - the pin needs to read its level
 * @author    BLE group .
 * @return    the pin's level(1: high 0: low)
 */
#define gpio_read(pin) gpio_get_level(pin)


/**
 * @brief      This function servers to enable gpio function.
 * @param[in]  pin - the selected pin.
 * @author     BLE group .
 * @return     none
 */
#define gpio_set_gpio_en(pin) gpio_function_en(pin)

/**
 * @brief      This function set the input function of a pin.
 * @param[in]  pin - the pin needs to set the input function
 * @param[in]  value - enable or disable the pin's input function(1: enable,0: disable )
 * @author     BLE group .
 * @return     none
 */
#define gpio_set_input_en(pin, value) gpio_set_input(pin, value)

typedef enum {
    PM_PIN_UP_DOWN_FLOAT    = 0,
    PM_PIN_PULLUP_1M        = 1,
    PM_PIN_PULLDOWN_100K    = 2,
    PM_PIN_PULLUP_10K       = 3,
}gpio_pull_type;

/**
 * @brief     This function set a pin's pull-up/down resistor.
 * @param[in] pin - the pin needs to set its pull-up/down resistor.
 * @param[in] up_down_res - the type of the pull-up/down resistor.
 * @return    none.
 */
#define gpio_setup_up_down_resistor(gpio, up_down)  gpio_set_up_down_res(gpio, up_down)
/**********************************************************************************************************************
 *                                     stimer  compatibility                                                          *
 *********************************************************************************************************************/

/*
 * @brief     This function performs to get system timer tick.
 * @return    system timer tick value.
 * @author    BLE group .
 */
#define clock_time stimer_get_tick


#endif
