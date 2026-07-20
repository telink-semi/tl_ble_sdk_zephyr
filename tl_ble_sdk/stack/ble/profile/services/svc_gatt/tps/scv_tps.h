/********************************************************************************************************
 * @file    svc_tps.h
 *
 * @brief   This is the header file for BLE SDK
 *
 * @author  BLE GROUP
 * @date    12,2025
 *
 * @par     Copyright (c) 2025, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#pragma once

//TPS: TX Power Service

/**
 * @brief      for user add default TPS service in all GAP server.
 * @param[in]  none.
 * @return     none.
 */
void blc_svc_addTpsGroup(void);

/**
 * @brief      for user remove default TPS service in all GAP server.
 * @param[in]  none.
 * @return     none.
 */
void blc_svc_removeTpsGroup(void);


/**
 * @brief      for use set tx power level value.
 * @param[in]  tpPowerLevel - the value of tx power level.
 * @return     none.
 */
void blc_svc_tpsSetTxPowerLevel(u8 txPowerLevel);
