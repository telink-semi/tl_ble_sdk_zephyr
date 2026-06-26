/********************************************************************************************************
 * @file    TL_802_15_4.h
 *
 * @brief   This is the source file for Bluetooth SDK
 *
 * @author  BLE GROUP
 * @date    06,2022
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd.
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
#ifndef TL_802_15_4_H_
#define TL_802_15_4_H_

#include "stack/ble/controller/ll/ll_stack.h"
#include "drivers.h"
#include "stack/system/system_internal.h"

#if THREAD_LL_FUNCTION_ENABLE

/**
 * @brief   Resume OpenThread stack thread scheduling.
 *          Called after BLE task slot ends to allow Thread tasks to run again.
 * @return  none
 */
void tlksdk_resume_openthread_threads(void);

/**
 * @brief   Suspend OpenThread stack thread scheduling.
 *          Called before RF switches to BLE to prevent Thread and BLE
 *          from accessing RF hardware concurrently.
 * @return  none
 */
void tlksdk_suspend_openthread_threads(void);

/**
 * @brief   Switch RF hardware context to 802.15.4 (Thread/Zigbee) mode.
 *          Configures RF registers for Zigbee 250K PHY, sets 802.15.4
 *          channel and TX power.
 * @return  none
 */
void tlksdk_switch_to_802154_rf_ctx(void);

/**
 * @brief   Switch RF hardware context to BLE mode.
 *          Configures RF registers for BLE 1M PHY, restores BLE channel
 *          and TX power.
 * @return  none
 */
void tlksdk_switch_to_ble_rf_ctx(void);

/**
 * @brief   802.15.4 (Thread/Zigbee) RF interrupt service routine.
 *          Handles RX and TX IRQ flags: increments RX counter and clears
 *          RX IRQ on receive, switch to RX mode and clears TX IRQ on transmit.
 * @return  none
 */
void tlksdk_rf_802154_isr(void);


#endif

#endif /* TL_802_15_4_H_ */