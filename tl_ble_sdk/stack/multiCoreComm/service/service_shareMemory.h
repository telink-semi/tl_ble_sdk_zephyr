/********************************************************************************************************
 * @file    service_shareMemory.h
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

#include"../comm.h"
#include"../drv/shareMemory.h"
#include "stack/ble/ble.h"

#ifndef TLK_SM_HCI_TX_BUFFER_SIZE
    #define TLK_SM_HCI_TX_BUFFER_SIZE (8 * 1024)
#endif

#ifndef TLK_SM_HCI_RX_BUFFER_SIZE
    #define TLK_SM_HCI_RX_BUFFER_SIZE (8 * 1024)
#endif

#ifndef TLK_SM_LOG_BUFFER_SIZE
    #define TLK_SM_LOG_BUFFER_SIZE    (5 * 512)
#endif

#ifndef TLK_SM_SYNC_BUFFER_SIZE
    #define TLK_SM_SYNC_BUFFER_SIZE   (4 * 1024)
#endif

/**
 *  @brief Connection Security Buffers
 */
# ifndef TLK_SM_CS_RAW_PCT_BUFFER_SIZE
    #define TLK_SM_CS_RAW_PCT_BUFFER_SIZE   16   /**< Raw PCT buffer size for connection security */
# endif

# ifndef TLK_SM_CS_RAW_PCT_BUFFER_NUM
    #define TLK_SM_CS_RAW_PCT_BUFFER_NUM    8    /**< Number of raw PCT buffers */
# endif



#if HOST_PROFILE_SUPPORT_CONNECTED_ISOCHRONOUS_STREAM
u32 tlk_sm_get_cs_raw_buff_addr(void);

int tlk_sm_cs_raw_buff_is_ready(void);
#endif /* HOST_PROFILE_SUPPORT_CONNECTED_ISOCHRONOUS_STREAM */

void tlk_share_memory_service_init(void);

void tlk_share_memory_service_loop(void);

void tlk_share_memory_service_hci_handler(void);

void tlk_share_memory_service_log_handler(void);

uint8_t tlk_share_memory_service_hci_isOverTargetPercent(uint32_t percent);

#if(TLK_MESSAGE_D25F == 1)

void tlk_d25f_register_hci_receive_cb(tlk_sm_message_type_e type,tlk_sm_rx_cb_f cb);

void tlk_d25f_register_sync_receive_cb(tlk_sm_message_type_e type,tlk_sm_rx_cb_f cb);

void tlk_d25f_register_log_receive_cb(tlk_sm_message_type_e type,tlk_sm_rx_cb_f cb);

tlk_sm_ret_e tlk_d25f_hci_send_message(tlk_sm_message_type_e type,uint8_t *data, uint32_t dataLen);

#elif(TLK_MESSAGE_N22 == 1)

void tlk_share_memory_n22_get_address_handler(uint32_t addr);

tlk_sm_ret_e tlk_n22_hci_send_message(tlk_sm_message_type_e type,uint8_t *data, uint32_t dataLen);

tlk_sm_ret_e tlk_n22_sync_send_message(tlk_sm_message_type_e type,uint8_t *data, uint32_t dataLen);

tlk_sm_ret_e tlk_n22_log_send_message(tlk_sm_message_type_e type,uint8_t *data, uint32_t dataLen);

void tlk_n22_register_hci_receive_cb(tlk_sm_message_type_e type,tlk_sm_rx_cb_f cb);

uint8_t  tlk_n22_hci_is_tx_over_threshold(uint32_t percent);
#endif
