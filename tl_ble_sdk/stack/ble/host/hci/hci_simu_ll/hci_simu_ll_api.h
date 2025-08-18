/********************************************************************************************************
 * @file    hci_simu_ll_api.h
 *
 * @brief   This is the header file for TLSR/TL
 *
 * @author  Bluetooth Group
 * @date    2024
 *
 * @par     Copyright (c) 2024, Telink Semiconductor (Shanghai) Co., Ltd.
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
#ifndef STACK_BLE_HOST_V1_HCI_SIM_LL_API_H_
#define STACK_BLE_HOST_V1_HCI_SIM_LL_API_H_

#if defined(TLK_ONLY_BLE_HOST)

#include "stack/ble/hci/hci.h"
#include "stack/ble/ble_common.h"
#include "stack/ble/hci/hci_cmd.h"

typedef bool (*ll_push_fifo_handler_t)(u16, u8 *);

extern ll_push_fifo_handler_t ll_push_tx_fifo_handler;

void blc_sdk_irq_handler(void) ;


/**
 * @brief      this function is used check if any controller buffer initialized by application incorrect.
 *             attention: this function must be called at the end of BLE LinkLayer Initialization.
 * @param      none
 * @return     status, 0x00:  succeed, no buffer error
 *                     other: buffer error code
 */
init_err_t blc_contr_checkControllerInitialization(void);


/**
 * @brief      for user to initialize MCU
 * @param      none
 * @return     none
 */
void blc_ll_initBasicMCU(void);


/**
 * @brief      for user to initialize link layer Standby state
 * @param      none
 * @return     none
 */
void blc_ll_initStandby_module(u8 *public_adr);



/**
 * @brief      this function is used to set the LE Random Device Address in the Controller
 * @param[in]  *randomAddr -  Random Device Address
 * @return     status, 0x00:  succeed
 *                     other: failed
 */
ble_sts_t blc_ll_setRandomAddr(u8 *randomAddr);


/**
 * @brief   main_loop for BLE stack, process data and event
 * @param   none
 * @return  none
 */
void blc_sdk_main_loop(void);
#endif /* #if defined(TLK_ONLY_BLE_HOST) */

#endif /* STACK_BLE_HOST_V1_HCI_SIM_LL_API_H_ */
