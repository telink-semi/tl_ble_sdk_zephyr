/********************************************************************************************************
 * @file    mcc.h
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
#ifndef MCC_H__
#define MCC_H__

#include "stack/ble/ble.h"

#if TLK_MESSAGE_D25F || TLK_MESSAGE_N22

#ifndef TLK_SM_HCI_TX_BUFFER_SIZE
    #define TLK_SM_HCI_TX_BUFFER_SIZE (8 * 1024)
#endif

#ifndef TLK_SM_HCI_RX_BUFFER_SIZE
    #define TLK_SM_HCI_RX_BUFFER_SIZE (8 * 1024)
#endif

/* mailbox from D25F to N22 */
typedef enum{
    TLK_MB_D25F_TO_N22_SYNC = 0,
    TLK_MB_D25F_TO_N22_AWAKEUP,
    TLK_MB_D25F_TO_N22_SM_DATA_READY,
    TLK_MB_D25F_TO_N22_SYNC_ADDRESS,
    TLK_MB_D25F_TO_N22_FLASH_WR_ADDRESS,

    /* for legacy compatible, changed later */
#if 0
    /* not used */
    TLK_MESSAGE_FROM_D25F_TO_N22_TEST,
    TLK_MESSAGE_FROM_D25F_TO_N22_SM_HCISCO_RX_ADDRESS,
    TLK_MESSAGE_FROM_D25F_TO_N22_TWS_SYNC_USBID,
#endif

    //2P4G MESSAGES
    TLK_MB_D25F_TO_N22_KM_DATA,
    TLK_MB_D25F_TO_N22_2P4G_KB_TX_ADDRESS,
    TLK_MB_D25F_TO_N22_2P4G_SPP_TX_ADDRESS,
    TLK_MB_D25F_TO_N22_MODE,
    TLK_MB_D25F_TO_N22_SET_CLK_INFO,

    TLK_MB_D25F_TO_N22_MAX,
}tlk_mb_d25f_to_n22_e;

/* mailbox from N22 to D25F */
typedef enum{
    TLK_MB_N22_TO_D25F_SYNC = 0,
    TLK_MB_N22_TO_D25F_SM_DATA_READY,
    TLK_MB_N22_TO_D25F_AWAKEUP,

    /* for legacy compatible, changed later */
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

    /*2P4G MESSAGES*/
    TLK_MB_N22_TO_D25F_KM_DATA,
    TLK_MB_N22_TO_D25F_KB_DATA,

    TLK_MB_N22_TO_D25F_MAX,
}tlk_mb_n22_to_d25f_e;

typedef enum{
    TLK_SHM_MSG_HCI,
    TLK_SHM_MSG_ZB,
    TLK_SHM_MSG_OT,
    TLK_SHM_MSG_2P4G,
    TLK_SHM_MSG_MAX
} tlk_shm_msg_type_e;

/* sync address */
typedef enum{
    TLK_MB_SYNC_ADDRESS_CLK_INFO = 0,
    TLK_MB_SYNC_ADDRESS_HCI_D25F_TO_N22,
    TLK_MB_SYNC_ADDRESS_HCI_N22_TO_D25F,
    TLK_MB_SYNC_ADDRESS_PM_INFO,

    TLK_MB_SYNC_ADDRESS_MAX_NUM,
}tlk_sync_address_e;

typedef enum
{
    FLASH_OPERATION_NONE = 0,       //D25F check NONE and set PREPARE, telling N22 no XIP
    FLASH_OPERATION_PREPARE,        //N22 wait PREPARE and set HOLDING, telling D25F already stop XIP
    FLASH_OPERATION_HOLDING,        //D25F wait HOLDING and set DONE, telling N22 flash op done and xip_en
    FLASH_OPERATION_DONE,           //N22 wait DONE and set NONE, telling D25F allow next mbox
}tlk_mb_flash_operation_step_e;

/* change according to tlk_mailbox_from_xxx_to_xxx_e */
#if TLK_MESSAGE_D25F
    #define TLK_MAILBOX_RECEIVE_CB_NUM       TLK_MB_N22_TO_D25F_MAX
#elif TLK_MESSAGE_N22
    #define TLK_MAILBOX_RECEIVE_CB_NUM       TLK_MB_D25F_TO_N22_MAX
#endif

#define TLK_SHM_MSG_RECEIVE_CB_NUM          TLK_SHM_MSG_MAX

typedef void(*mb_recv_cb_t)(void *);
typedef void(*shm_recv_cb_t)(uint8_t *pData, uint16_t len);

extern _attribute_data_retention_ mb_recv_cb_t mb_recv_cb[TLK_MAILBOX_RECEIVE_CB_NUM];
extern _attribute_data_retention_ shm_recv_cb_t shm_recv_cb[TLK_SHM_MSG_RECEIVE_CB_NUM];
/* 
 * @brief   Mailbox receive callback
 * @param   msg: pointer to the received message
 */
void mcc_mb_recv_cb(void* msg);

/* 
 * @brief   Register mailbox receive callback
 * @param   cmd: command identifier
 * @param   cb: callback function to be registered
 */
void mcc_mb_register_cb(uint8_t cmd, mb_recv_cb_t cb);

/* 
 * @brief   Shared memory receive callback
 * @param   msg: pointer to the received message
 * @param   msg_len: length of the received message
 * @param   msg_type: type of the received message
 */
void mcc_shm_recv_cb(unsigned char *msg, unsigned short msg_len, unsigned short msg_type);

/*
 * @brief   Register shared memory receive callback
 * @param   msg_type: type of the message
 * @param   cb: callback function to be registered
 */
void mcc_shm_register_cb(uint8_t msg_type, shm_recv_cb_t cb);

#endif /* TLK_MESSAGE_D25F || TLK_MESSAGE_N22 */
#endif /* MCC_H__ */
