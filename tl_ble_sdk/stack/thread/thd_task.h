/********************************************************************************************************
 * @file    thd_task.h
 *
 * @brief   This is the header file for Bluetooth SDK
 *
 * @author  BLE GROUP
 * @date    06,2022
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#ifndef TLKSTK_THD_TASK_H_
#define TLKSTK_THD_TASK_H_

#include "common/types.h"
#include "stack/ble/controller/ll/ll_stack.h"
#include "stack/system/system_internal.h"

#if THREAD_LL_FUNCTION_ENABLE
/**
 * @brief Enumeration defining values for thread task enable or disable
 */
typedef enum {
    THD_TASK_DISABLE = 0x00,
    THD_TASK_ENABLE  = 0x01,
}thd_task_en_e;

/**
 * @brief    This function is used to initialize thread flexible task module
 * @param    none
 * @return    none
 */
void tlksdk_thd_initFlexibleTask_module(void);


/**
 * @brief       This function is used to set thread flexible task interval
 * @param[in]  interval_us - thread flexible task interval; unit: uS; must be in the range of 20000(10mS) ~ 4000000(4S)
 * @return       none
 */
void tlksdk_thd_setFlexibleTaskInterval(u32 interval_us);


/**
 * @brief       This function is used to set thread flexible task enable
 * @param[in]  enable - enable or disable
 * @return       none
 */
void tlksdk_thd_enableFlexibleTask(thd_task_en_e enable);

////////////////////////////////////// Insert Task 1 //////////////////////////////////////
/**
 * @brief    This function is used to initialize thread insert task 1 module
 * @param    none
 * @return    none
 */
void tlksdk_thd_initInsertTask1_module(void);
/**
 * @brief       This function is used to set thread insert task 1 interval
 * @param[in]  scan_interval - 625uS unit
 * @param[in]  scan_window - 625uS unit
 * @return       none
 */


/**
 * @brief       This function is used to set thread insert task 1 enable
 * @param[in]  enable - enable or disable
 * @return       none
 */
void tlksdk_thd_enableInsertTask1(thd_task_en_e enable);


/**
 * @brief       Get the post tick value of thread insert task 1
 * @param       none
 * @return      The post tick value (in system tick units) of insert task 1
 */
u32 tlksdk_thd_getInsertTask1PostTick(void);


/**
 * @brief       Check whether the task currently being executed by the
 *              scheduler is the thread insert task 1.
 *              Typically called in an IRQ/main-loop context to decide
 *              whether RF is currently owned by 802.15.4 (Thread/Zigbee).
 * @param       none
 * @return      true  - the current scheduler task is insert task 1
 *              false - otherwise
 */
bool tlksdk_thd_checkIsInsertTask1(void);

extern void (*thd_insertTsk_switch_to_802154_cb)(void);
extern void (*thd_insertTsk_switch_to_ble_cb)(void);
extern void (*tlksdk_switch_to_802154_rf_cb)(void);
extern void (*tlksdk_switch_to_ble_rf_cb)(void);
void (*tlksdk_switch_to_802154_rf_isr_cb)(const void *);
void (*tlksdk_switch_to_ble_rf_isr_cb)(const void *);


void tlksdk_thd_registerModeChangeCb(void (*to_802154_cb)(void), void (*to_ble_cb)(void));
void tlksdk_thd_registerSwitchTo802154RfCb(void (*switch_to_802154_rf_cb)(void), void (*switch_to_ble_rf_cb)(void));
void tlksdk_thd_registerRfIsrCb(void (*switch_to_802154_rf_isr_cb)(const void *), void (*switch_to_ble_rf_isr_cb)(const void *));

#endif


#endif /* TLKSTK_THD_TASK_H_ */
