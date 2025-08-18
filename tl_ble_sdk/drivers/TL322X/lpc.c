/********************************************************************************************************
 * @file    lpc.c
 *
 * @brief   This is the source file for tl322x
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
#include "lpc.h"

/**
 * @brief       This function selects input reference voltage for low power comparator.
 * @param[in]   mode    - lower power comparator working mode includes normal mode and low power mode.
 * @param[in]   ref     - selected input reference voltage.
 * @return      none.
 */
void lpc_set_input_ref(lpc_mode_e mode, lpc_reference_e ref)
{
    if (mode == LPC_LOWPOWER) {
        analog_write_reg8(0x0e, analog_read_reg8(0x0e) & 0xf7);//0x0e[3] = 0
        //switch uvlo vref
        analog_write_reg8(0x0b, analog_read_reg8(0x0b) & 0xf7);//0x0b[3] = 0, ref by uvlo
    } else if (mode == LPC_NORMAL) {
        analog_write_reg8(0x0e, analog_read_reg8(0x0e) | 0x08);//0x0e[3] = 1
        //switch uvlo vref
        analog_write_reg8(0x0b, analog_read_reg8(0x0b) | 0x08);//0x0b[3] = 1, ref by BG
    }
    analog_write_reg8(0x0d, (analog_read_reg8(0x0d) & 0x7f) | (ref << 4));
}
