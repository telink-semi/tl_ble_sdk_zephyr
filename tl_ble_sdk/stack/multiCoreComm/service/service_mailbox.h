/********************************************************************************************************
 * @file    service_mailbox.h
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

#ifndef STACK_MULTICORECOMM_SERVICE_SERVICE_MAILBOX_H_
#define STACK_MULTICORECOMM_SERVICE_SERVICE_MAILBOX_H_
#include "../comm.h"
#include "stack/ble/ble.h"

typedef enum
{
    TLK_MESSAGE_FROM_D25F_TO_N22_SM_ADDRESS = 0,
    TLK_MESSAGE_FROM_D25F_TO_N22_WAKEUP,
    TLK_MESSAGE_FROM_D25F_TO_N22_CLOCK_INFO,
    TLK_MESSAGE_FROM_D25F_TO_N22_PM_SHARE_INFO,
    TLK_MESSAGE_FROM_D25F_TO_N22_FLASH_WR_ADDRESS,
    TLK_MESSAGE_FROM_D25F_TO_N22_SYNC,

#if 0
    /* not used */
    TLK_MESSAGE_FROM_D25F_TO_N22_TEST,
    TLK_MESSAGE_FROM_D25F_TO_N22_SM_HCISCO_RX_ADDRESS,
    TLK_MESSAGE_FROM_D25F_TO_N22_TWS_SYNC_USBID,
#endif

    TLK_MESSAGE_FROM_D25F_TO_N22_MAX,
}tlk_mailbox_from_d25f_to_n22_e;

typedef enum
{
    TLK_MESSAGE_FROM_N22_TO_D25F_SYNC_DATA_READY = 0,
    TLK_MESSAGE_FROM_N22_TO_D25F_SM_DATA_READY,
    TLK_MESSAGE_FROM_N22_TO_D25F_WAKEUP,
    TLK_MESSAGE_FROM_N22_TO_D25F_SYNC,

#if 0
    /* not used */
    TLK_MESSAGE_FROM_N22_TO_D25F_TEST,
    TLK_MESSAGE_FROM_N22_TO_D25F_REQ_HANDLE_SYNC_DATA,
    TLK_MESSAGE_FROM_N22_TO_D25F_STIMER_START_EVT,
    TLK_MESSAGE_FROM_N22_TO_D25F_HANDOVER_MASK_SET_EVT,
    TLK_MESSAGE_FROM_N22_TO_D25F_HANDOVER_MASK_CLEAR_EVT,
    TLK_MESSAGE_FROM_N22_TO_D25F_HANDOVER_INFO_EXTRACT,
#endif

    /*Channel Sounding */
    TLK_MESSAGE_FROM_N22_TO_D25F_CS_RAW_PCT_ADDRESS,
    TLK_MESSAGE_FROM_N22_TO_D25F_CS_RAW_PCT_DATA_READY,
    TLK_MESSAGE_FROM_N22_TO_D25F_CS_FPU_CALC_TRIGGER,
    TLK_MESSAGE_FROM_N22_TO_D25F_CS_CALC_FREQ_TRIGGER,
    TLK_MESSAGE_FROM_N22_TO_D25F_CS_PES_COLLECT_DATA_INIT_SDK_TRIGGER,
    TLK_MESSAGE_FROM_N22_TO_D25F_CS_CALC_PES_INFO_FINE_TRIGGER,
    TLK_MESSAGE_FROM_N22_TO_D25F_CS_CALC_PES_INFO_SDK_TRIGGER,
    TLK_MESSAGE_FROM_N22_TO_D25F_CS_NADM_DETECT_TRIGGER,
    TLK_MESSAGE_FROM_N22_TO_D25F_CS_CALC_TES_INFO_ASIC_HARD_FIX_TRIGGER,
    TLK_MESSAGE_FROM_N22_TO_D25F_CS_CALC_TES_INFO_ASIC_SOFT_TRIGGER,
    TLK_MESSAGE_FROM_N22_TO_D25F_CS_COMPRESS_TEST_INFO_TRIGGER,

    TLK_MESSAGE_FROM_N22_TO_D25F_MAX,
}tlk_mailbox_from_n22_to_d25f_e;

typedef enum
{
    TLK_SM_DATA_READY_TYPE_HCI = 0,
    TLK_SM_DATA_READY_TYPE_LOG,
    TLK_SM_DATA_READY_TYPE_MAX_NUM,
}tlk_sm_data_ready_type_e;

typedef void(*tlk_mailbox_receive_cb_t)(uint8_t*);

/* change according to tlk_mailbox_from_xxx_to_xxx_e */
#if TLK_MESSAGE_D25F
    #define TLK_MAILBOX_RECEIVE_CB_NUM       TLK_MESSAGE_FROM_N22_TO_D25F_MAX
#elif TLK_MESSAGE_N22
    #define TLK_MAILBOX_RECEIVE_CB_NUM       TLK_MESSAGE_FROM_D25F_TO_N22_MAX
#endif

/**
 * @brief      This function servers to init mailbox service.
 * @param[in]  none
 * @return     none
 */
void tlk_mailbox_service_init(void);

/**
 * @brief      This function servers to send cmd
 * @param[in]  cmd   - in d25f,search in tlk_mailbox_from_d25f_to_n22_e.
 *                     in n22, search in tlk_mailbox_from_n22_to_d25f_e.
 * @param[in]  data  - data corresponding to cmd,7 bytes at most.
 * @return     none
 */
void tlk_mailbox_send_data(uint8_t cmd,uint8_t* data);

/**
 * @brief      This function servers to register rx callback in mailbox.
 * @param[in]  cmd - in d25f,search in tlk_mailbox_from_n22_to_d25f_e.
 *                   in n22, search in tlk_mailbox_from_d25f_to_n22_e.
 * @param[in]  cb  - receive callback corresponding to cmd.
 * @return     none
 */
void tlk_mailbox_register_message_cb(uint8_t cmd,tlk_mailbox_receive_cb_t cb);

#endif /* STACK_MULTICORECOMM_SERVICE_SERVICE_MAILBOX_H_ */
