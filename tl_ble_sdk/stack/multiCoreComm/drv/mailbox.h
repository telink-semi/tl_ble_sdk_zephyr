/********************************************************************************************************
 * @file    mailbox.h
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
#pragma once
#define BTBLE_MAILBOX_TX_MAX_BLOCK_TIME_US (100 * 1000)

void tlk_mailbox_receive_hook(uint8_t* cmd);

void mailbox_send_data(uint8_t* data, uint32_t maxBlockTimeUs);

bool mailbox_is_trans_busy(void);

void mailbox_n22_to_d25_irq_handler(void);
