/********************************************************************************************************
 * @file    sharememory.h
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
#ifndef PORT_SHAREMEMORY_H__
#define PORT_SHAREMEMORY_H__

#include <stdint.h>
#include <stdbool.h>

#define SHM_FIFO_MSG_MAX_LEN    1024

typedef void(*shm_cb_t)(uint8_t *msg, uint16_t msg_len, uint16_t msg_type);

/* sharememory fifo structure */
typedef struct {
    uint8_t  *p;
    uint32_t size;
    uint32_t wptr;
    uint32_t rptr;

    shm_cb_t recv_cb;
    bool inited;

    /* used for concurrent operation */
    volatile uint32_t nested_cnt;
    volatile uint32_t nested_ptr;
} shm_fifo_t;

/* sharememory fifo status */
typedef enum{
    SHM_FIFO_SUCCESS = 0,
    SHM_FIFO_NO_NEED_NOTIFY = SHM_FIFO_SUCCESS,
    SHM_FIFO_NEED_NOTIFY,
    SHM_FIFO_NO_INITED,
    SHM_FIFO_FULL,
    SHM_FIFO_LENGTH_TOO_LONG,
}shm_fifo_status_e;

/* 
 * @brief   Initialize the share memory FIFO
 * @param   fifo: pointer to the FIFO structure
 * @param   buffer: pointer to the buffer for the FIFO
 * @param   size: size of the buffer
 */
void shm_fifo_init(shm_fifo_t *fifo, uint8_t *buffer, uint32_t size);

/* 
 * @brief   Push a message into the share memory FIFO
 * @param   fifo: pointer to the FIFO structure
 * @param   msg: pointer to the message to be pushed
 * @param   msg_len: length of the message
 * @return  status of the operation
 */
shm_fifo_status_e shm_fifo_push(shm_fifo_t *fifo, uint8_t *msg, uint16_t msg_len, uint16_t msg_type);

/* 
 * @brief   Pop messages from the share memory FIFO and process them using the registered callback
 * @param   fifo: pointer to the FIFO structure
 */
void shm_fifo_pop_all(shm_fifo_t *fifo);

/* 
 * @brief   Pop messages from the share memory FIFO and process them using the registered callback
 * @param   fifo: pointer to the FIFO structure
 */
void shm_fifo_pop(shm_fifo_t *fifo);

/* 
 * @brief   Check if the used space in the FIFO exceeds a certain percentage
 * @param   fifo: pointer to the FIFO structure
 * @param   percent: percentage threshold to check against (0-100)
 * @return  1 if used space exceeds the percentage, 0 otherwise
 */
uint8_t shm_fifo_is_overpercent(shm_fifo_t *fifo, uint8_t percent);

/* 
 * @brief   Register a callback function for received messages
 * @param   fifo: pointer to the FIFO structure
 * @param   cb: callback function to be registered
 */
void shm_fifo_register_recv_cb(shm_fifo_t *fifo, shm_cb_t cb);

#endif /* PORT_SHAREMEMORY_H__ */
