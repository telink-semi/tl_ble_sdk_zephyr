/********************************************************************************************************
 * @file    comm.c
 *
 * @brief   This is the source file for TLSR/TL
 *
 * @author  Bluetooth Group
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

#ifndef STACK_MULTICORECOMM_COMM_H_
#define STACK_MULTICORECOMM_COMM_H_

#include "common/config/user_config.h"
#include "tl_common.h"
#ifndef TLK_MESSAGE_N22
    #define TLK_MESSAGE_N22  0
#endif

#ifndef TLK_MESSAGE_D25F
    #define TLK_MESSAGE_D25F 0
#endif


void tlk_multi_core_communication_init(void);


void tlk_multi_core_communication_loop(void);

#endif /* STACK_MULTICORECOMM_COMM_H_ */
