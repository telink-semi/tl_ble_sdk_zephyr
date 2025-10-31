/********************************************************************************************************
 * @file    mailbox.h
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
#ifndef PORT_MAILBOX_H__
#define PORT_MAILBOX_H__

#define MB_TX_MAX_BLOCK_TIME_US  (100 * 1000)

typedef void(*mb_cb_t)(void *msg);

/*
 * @brief   Initialize the mailbox
 * @param   cb: callback function for mailbox
 */
void mb_init(mb_cb_t cb);

/*
 * @brief   Send message via mailbox in a blocking manner
 * @param   msg: pointer to the message to be sent
 * @note    Timeout parameter is `MB_TX_MAX_BLOCK_TIME_US`
 */
void mb_send_sync(void* msg);

/*
 * @brief   Send message via mailbox with polling
 * @param   msg: pointer to the message to be sent
 * @note    This function will poll for a response after sending the message.
 *          If no response is received within `MAILBOX_SYNC_TIMEOUT`, the system will reboot.
 */
void mb_send_with_polling(void *msg);

/*
 * @brief   Polling for a message from the mailbox
 * @param   msg: pointer to store the received message
 * @note    This function will wait until a message is received.
 *          If no message is received within `MAILBOX_SYNC_TIMEOUT`, the system will reboot.
 */
void mb_polling(void* msg);

#endif /* PORT_MAILBOX_H__ */
