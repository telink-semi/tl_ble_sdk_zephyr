/********************************************************************************************************
 * @file    service_n22.h
 *
 * @brief   This is the source file for TLSR/TL
 *
 * @author  BLE Group
 * @date    2025
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
#ifndef SERVICE_N22_H__
#define SERVICE_N22_H__

#include "stack/ble/ble.h"
#include "../port/sharememory.h"
#include "../port/mailbox.h"
#include "mcc.h"

#if TLK_MESSAGE_N22
/* 
 * @brief   N22 service initialization 
 */
void mcc_n22_service_init(void);

/* 
 * @brief   Send HCI message to D25F
 * @param   data: pointer to the data to be sent
 * @param   data_len: length of the data to be sent
 * @return  status of the operation
 */
shm_fifo_status_e mcc_n22_hci_send_msg(uint8_t *data, uint32_t data_len);

/* 
 * @brief   N22 main loop to process incoming messages
 */
void mcc_n22_loop(void);

void mcc_n22_register_shm_recv_cb(tlk_shm_msg_type_e type, shm_recv_cb_t cb);

/* 
 * @brief   Check if HCI TX buffer usage is over a certain threshold
 * @param   percent: threshold percentage (0-100)
 * @return  1 if over threshold, 0 otherwise
 */
uint8_t mcc_n22_hci_is_tx_over_threshold(uint32_t percent);

void mcc_n22_wakeup_d25f(void);

void mcc_n22_mb_send_data(uint8_t cmd,uint8_t* data);

shm_fifo_status_e mcc_n22_shm_send_msg(uint8_t *data, uint32_t data_len, tlk_shm_msg_type_e type);
#endif /* TLK_MESSAGE_N22 */
#endif /* SERVICE_N22_H__ */
