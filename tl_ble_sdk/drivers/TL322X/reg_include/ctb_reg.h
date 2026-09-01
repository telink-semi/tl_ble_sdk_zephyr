/********************************************************************************************************
 * @file    ctb_reg.h
 *
 * @brief   This is the header file for tl322x
 *
 * @author  Driver Group
 * @date    2026
 *
 * @par     Copyright (c) 2026, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#ifndef CTB_REG_H
#define CTB_REG_H
#include "soc.h"

#define areg_0x17 0x17

#define areg_0x4b 0x4b

#define areg_0x64 0x64

typedef enum
{
    CTB_ERROR_IRQ = BIT(4),
    CTB_IRQ       = BIT(5),
} ctb_irq_e;

#define areg_0x73 0x73

#define areg_0x83 0x83

#define areg_0x85 0x85

#define areg_0x87 0x87

#define areg_0x89 0x89

#define areg_0x8b 0x8b

#define areg_0x8d 0x8d

#define areg_0x8f 0x8f

#define areg_0x91 0x91

#define areg_0x92 0x92

#define areg_0x93 0x93

#define areg_0x94 0x94

typedef enum
{
    CTB_CLK_EN   = BIT(0),
    CTB_RST_SYNC = BIT(1),
} ctb_ctrl_e;

#define areg_0x95 0x95

#define areg_0x97 0x97

#define areg_0x99 0x99

typedef enum
{
    FLD_CTB_CALBR_EN                  = BIT(0),         /**< Enable the calibration state machine */
    FLD_CTB_CALBR_FINISHED_FLAG_RST   = BIT(1),         /**< Reset the calibration finished flag */
    FLD_CTB_CALBR_TYPE                = BIT(2),         /**< 0: output value equals the updated threshold value, 1: output value equals the updated switch_cap_val */
    FLD_CTB_CALBR_RST                 = BIT(3),         /**< Synchronous reset for the calibration block */
    FLD_CTB_CALBR_ITERATION_NUM       = BIT_RNG(4, 5),  /**< Number of iterations: 00-4, 01-8, 10-16 */
}ctb_calbr_e;

#define areg_0x9a 0x9a

#define areg_0x9c 0x9c

#define areg_0x9d 0x9d

#define areg_0x9f 0x9f

#define areg_0xab 0xab

#define areg_0xb0 0xb0

#define areg_0x19a 0x19a

#define areg_0x1b6 0x1b6

#define areg_0x1d2 0x1d2

#define areg_0x1ee 0x1ee

#define areg_0x1ef 0x1ef

#define areg_0x1f7 0x1f7

#endif