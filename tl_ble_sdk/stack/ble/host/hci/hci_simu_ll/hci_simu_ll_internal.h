/********************************************************************************************************
 * @file    hci_simu_ll_acl_conn_internal.h
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

u16 blt_llms_get_connEffectiveMaxTxOctets_by_connIdx(u16 conn_idx);



u8  blt_ll_getAclConnPeerAddrType(u16 connHandle);

u8 *blt_ll_getAclConnPeerAddr(u16 connHandle);


u8 blt_ll_getMaxAclCentralNumber(void);


u8 blt_ll_pushTxfifoHold(u16 connHandle, u8 *p);

ble_sts_t blt_ll_isAclhdlInvalid(u16 connHandle);

int blt_ll_isEncryptionBusy(u16 connHandle);

u8 blt_llms_get_tx_fifo_max_num(u16 connHandle);

void blt_gap_registerEventHandler(hci_event_handler_t handler);

u32 blc_ll_getConnectionStartTick(u16 connHandle);

void ble_host_set_encryption_busy(u16 connHandle, u8 enc_busy);
