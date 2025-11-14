/********************************************************************************************************
 * @file    service_d25f.h
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
#ifndef SERVICE_D25F_H__
#define SERVICE_D25F_H__

#include "stack/ble/ble.h"
#include "../port/sharememory.h"
#include "../port/mailbox.h"
#include "mcc.h"

#if TLK_MESSAGE_D25F

/* 
 * @brief   D25F service initialization 
 */
void mcc_d25f_service_init(void);

/* 
 * @brief   Send HCI message to N22
 * @param   data: pointer to the data to be sent
 * @param   data_len: length of the data to be sent
 * @return  status of the operation
 */
shm_fifo_status_e mcc_d25f_hci_send_msg(uint8_t *data, uint32_t data_len);

/* 
 * @brief   D25F main loop to process incoming messages
 */
void mcc_d25f_loop(void);

void mcc_d25f_register_shm_recv_cb(tlk_shm_msg_type_e type, shm_recv_cb_t cb);

void mcc_d25f_flash_start_operation_hook(void);

void mcc_d25f_flash_post_operation_hook(void);

void mcc_d25f_set_pm_info_addr(void);

void mcc_d25f_mb_send_data(uint8_t cmd,uint8_t* data);

shm_fifo_status_e mcc_d25f_shm_send_msg(uint8_t *data, uint32_t data_len, tlk_shm_msg_type_e type);
#endif /* TLK_MESSAGE_D25F */
#endif /* SERVICE_D25F_H__ */
