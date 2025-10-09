/********************************************************************************************************
 * @file    hci_simu_ll_acl_conn.h
 *
 * @brief   This is the source file for TLSR/TL
 *
 * @author  Bluetooth Group
 * @date    2025
 *
 * @par     Copyright (c) 2025, Telink Semiconductor (Shanghai) Co., Ltd.
 *          All rights reserved.
 *
 *          The information contained herein is confidential property of Telink
 *          Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *          of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *          Co., Ltd. and the licensee or the terms described here-in. This heading
 *          MUST NOT be removed from this file.
 *
 *          Licensee shall not delete, modify or alter (or permit any third party to delete, modify, or
 *          alter) any information contained herein in whole or in part except as expressly authorized
 *          by Telink semiconductor (shanghai) Co., Ltd. Otherwise, licensee shall be solely responsible
 *          for any claim to the extent arising out of or relating to such deletion(s), modification(s)
 *          or alteration(s).
 *
 *          Licensees are granted free, non-transferable use of the information in this
 *          file under Mutual Non-Disclosure Agreement. NO WARRANTY of ANY KIND is provided.
 *
 *******************************************************************************************************/
#pragma once

#include "stack/ble/hci/hci_cmd.h"

/**
 * @brief      this function is used to change the ACL connection parameters.
 * @param[in]  connHandle - Connection_Handle
 * @param[in]  conn_min - the minimum allowed connection interval.
 * @param[in]  conn_max - the maximum allowed connection interval.
 * @param[in]  conn_latency - the maximum allowed connection latency.
 * @param[in]  timeout - the link supervision timeout for the LE link.
 * @param[in]  ce_min - information parameters providing the Controller with a hint about the expected minimum length of the connection events.
 * @param[in]  ce_max - information parameters providing the Controller with a hint about the expected maximum length of the connection events.
 * @return     status, 0x00:  succeed
 *                     other: failed
 */
ble_sts_t blc_ll_updateConnection(u16 connHandle, conn_inter_t conn_min, conn_inter_t conn_max, u16 conn_latency, conn_tm_t timeout, u16 ce_min, u16 ce_max);


/**
 * @brief      for user to initialize ACL connection module, this is must if user want use ACL central role or ACL peripheral role.
 * @param      none
 * @return     none
 */
void blc_ll_initAclConnection_module(void);


/**
 * @brief      for user to initialize LinkLayer ACL connection RX FIFO.
 *             all connection will share the FIFO.
 * @param[in]  pRxbuf - RX FIFO buffer address.
 * @param[in]  fifo_size - RX FIFO size
 * @param[in]  fifo_number - RX FIFO number, can only be 4, 8, 16 or 32
 * @return     status, 0x00:  succeed
 *                     other: failed
 */
ble_sts_t blc_ll_initAclConnRxFifo(u8 *pRxbuf, int fifo_size, int fifo_number);


/**
 * @brief      for user to initialize ACL connection peripheral role.
 * @param      none
 * @return     none
 */
void blc_ll_initAclPeriphrRole_module(void);


/**
 * @brief      for user to initialize LinkLayer ACL connection peripheral role TX FIFO.
 * @param[in]  pRxbuf - TX FIFO buffer address.
 * @param[in]  fifo_size - TX FIFO size
 * @param[in]  fifo_number - TX FIFO number, can only be 4, 8, 16 or 32
 * @param[in]  conn_number - Number of supported ACL peripheral connections
 * @return     status, 0x00:  succeed
 *                     other: failed
 */
ble_sts_t blc_ll_initAclPeriphrTxFifo(u8 *pTxbuf, int fifo_size, int fifo_number, int conn_number);


/**
 * @brief      This function is used to configure the number of central and peripheral connections that the protocol stack can support.
 * @param[in]  max_central_num - Number of central ACL connections supported.
 * @param[in]  max_peripheral_num - Number of peripheral ACL connections supported.
 * @return     status, 0x00:  succeed
 *                     other: failed
 */
ble_sts_t blc_ll_setMaxConnectionNumber(int max_central_num, int max_peripheral_num);

/**
 * @brief      This function is used to obtain the currently available TX FIFO numbers according to the ACL handle.
 * @param[in]  connHandle - ACL connection handle.
 * @return     available TX FIFO numbers
 */
u8 blc_ll_getTxFifoNumber(u16 connHandle);


/**
 * @brief      This function is used to obtain the number of ACL connections of the peripheral role.
 * @param[in]  none.
 * @return     The number of currently connected peripheral ACLs.
 */
int blc_ll_getCurrentPeripheralRoleNumber(void);


ble_sts_t blc_ll_disconnect(u16 connHandle, u8 reason);


/**
 * @brief      for user to read current ACL connection interval
 * @param[in]  connHandle - ACL connection handle.
 * @return     0    :  connHandle invalid, not match a connection
 *             other:  connection interval, unit: 1.25mS
 */
u16 blc_ll_getAclConnectionInterval(u16 connHandle);

