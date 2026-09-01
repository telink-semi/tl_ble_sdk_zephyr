/********************************************************************************************************
 * @file    ctb.h
 *
 * @brief   This is the header file for tl322x CTB (Capacitive Touch Button) driver
 *
 * @author  Driver Group
 * @date    2026
 *
 * @par     Copyright (c) 2023, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#include "gpio.h"
#include "reg_include/register.h"
#include <stdbool.h>

/** @brief  Extract the bit position (0-indexed) from a val with a single bit set.
 *          Returns -1 if val is 0.
 */
#define CTB_BIT_POSITION(val) (val ? __builtin_ctz(val) : -1)

/**
 * @brief   Definition of CTB channel bits (bitmask).
 * @note    Use these values with ctb_channel_en() to enable channels by bitmask.
 */
typedef enum
{
    CTB_CHANNEL_0 = BIT(0),
    CTB_CHANNEL_1 = BIT(1),
    CTB_CHANNEL_2 = BIT(2),
    CTB_CHANNEL_3 = BIT(3),
    CTB_CHANNEL_4 = BIT(4),
    CTB_CHANNEL_5 = BIT(5),
    CTB_CHANNEL_6 = BIT(6),
    CTB_CHANNEL_7 = BIT(7),
    CTB_CHANNEL_8 = BIT(8),
    CTB_CHANNEL_9 = BIT(9),
    CTB_CHANNEL_10 = BIT(10),
    CTB_CHANNEL_11 = BIT(11),
    CTB_CHANNEL_12 = BIT(12),
    CTB_CHANNEL_13 = BIT(13),
    CTB_CHANNEL_14 = BIT(14),
    CTB_CHANNEL_15 = BIT(15),
} ctb_channel_e;

/**
 * @brief   Definition of CTB channel numbers (ordinal, 0-15).
 * @note    Use these values with ctb_irq_en(), ctb_enable(), ctb_calib() etc.
 *          CTB_CHN_NUM_MAX marks the total number of channels (16).
 */
typedef enum
{
    CTB_CHN_NUM_0 =  0,
    CTB_CHN_NUM_1 =  1,
    CTB_CHN_NUM_2 =  2,
    CTB_CHN_NUM_3 =  3,
    CTB_CHN_NUM_4 =  4,
    CTB_CHN_NUM_5 =  5,
    CTB_CHN_NUM_6 =  6,
    CTB_CHN_NUM_7 =  7,
    CTB_CHN_NUM_8 =  8,
    CTB_CHN_NUM_9 =  9,
    CTB_CHN_NUM_10 =  10,
    CTB_CHN_NUM_11 =  11,
    CTB_CHN_NUM_12 =  12,
    CTB_CHN_NUM_13 =  13,
    CTB_CHN_NUM_14 =  14,
    CTB_CHN_NUM_15 =  15,
    CTB_CHN_NUM_MAX,
} ctb_chn_num_e;

/**
 * @brief   Definition of CTB calibration type selection.
 * @note    Used with ctb_calib_type() to select which value the calibration engine outputs.
 */
typedef enum
{
    FLD_CTB_CALBR_THRESHOLD    = 0,    /**< Output updated threshold value  */
    FLD_CTB_CALBR_CAP          = 1,    /**< Output updated switch_cap_val   */
}ctb_calbr_type_e;


/**
 * @brief       This function serves to enable the specified CTB channels.
 * @param[in]   chn - the channel bitmask to be enabled (registers areg_0x95/0x96, 16-bit access).
 * @return      none
 */
static inline void ctb_channel_en(ctb_channel_e chn)
{
    analog_write_reg16(areg_0x95,analog_read_reg16(areg_0x95) | chn);//channel  enable
}

/**
 * @brief       This function serves to directly set the CTB channel enable register (overwrite, not read-modify-write).
 * @param[in]   chn - the channel bitmask to write directly to register areg_0x95.
 * @return      none
 * @note        Unlike ctb_channel_en(), this replaces the entire register value instead of OR-ing.
 *              Used during calibration to ensure only the target channel is enabled.
 */
static inline void ctb_channel_en1(ctb_channel_e chn)
{
    analog_write_reg16(areg_0x95, chn);
}

/**
 * @brief       This function serves to get the calibration done status.
 * @param[in]   none
 * @return      calibration done status.
 * @retval      false  - calibration finished.
 * @retval      true   - calibration not finished.
 * @note        Reads bit[0] of register areg_0x1f7. The bit is active-low (1 = done),
 *              so the return value is inverted with ! operator.
 */
static inline bool ctb_get_calib_done(void)
{
    return !(analog_read_reg8(areg_0x1f7) & BIT(0));
}

/**
 * @brief       This function serves to clear the CTB interrupt for specified channels.
 * @param[in]   chn - the channel bitmask to be cleared (register areg_0x9a).
 * @return      none
 */
static inline void ctb_clr_irq(ctb_channel_e chn)
{
    analog_write_reg16(areg_0x9a, chn);
    analog_write_reg16(areg_0x9a, analog_read_reg16(areg_0x9a) & (~chn));
}

/**
 * @brief       This function serves to clear all CTB channel interrupts.
 * @param[in]   none
 * @return      none
 * @note        Writes 0xffff to register areg_0x9a (16-bit) to set all channel IRQ clear bits,
 *              then writes 0x0000 to de-assert the clear pulse.
 */
static inline void ctb_clr_all_irq(void)
{
    analog_write_reg16(areg_0x9a, 0xffff);
    analog_write_reg16(areg_0x9a, 0x0);
}

/**
 * @brief       This function serves to enable CTB as a wakeup source.
 * @param[in]   none
 * @return      none
 * @note        Sets the CTB interrupt bits in register areg_0x64 and enables the
 *              CTB wakeup bit in register areg_0x4b.
 */
static inline void ctb_wakeup_enable(void)
{
    analog_write_reg8(areg_0x64, CTB_ERROR_IRQ | CTB_IRQ);
    analog_write_reg8(areg_0x4b, analog_read_reg8(areg_0x4b) | BIT(5));
}

/**
 * @brief       This function serves to get the CTB interrupt status.
 * @param[in]   none
 * @return      the interrupt status register value (register areg_0x64).
 * @note        Bit[4]: CTB_ERROR_IRQ, Bit[5]: CTB_IRQ.
 */
static inline unsigned char ctb_get_irq_status(void)
{
    return analog_read_reg8(areg_0x64);
}

/**
 * @brief       This function serves to clear the CTB interrupt status.
 * @param[in]   none
 * @return      none
 * @note        Write-1-to-clear to register areg_0x64.
 */
static inline void ctb_clear_irq_status(void)
{
    analog_write_reg8(areg_0x64, CTB_IRQ | CTB_ERROR_IRQ);
}

/**
 * @brief       This function serves to get the CTB interrupt status per channel.
 * @param[in]   none
 * @return      the channel interrupt status register value (register areg_0xab, 16-bit).
 * @note        Each bit corresponds to one channel's interrupt status.
 */
static inline unsigned short ctb_get_chn(void)
{
    return analog_read_reg16(areg_0xab);
}

/**
 * @brief       This function serves to enable the CTB interrupt for a specific channel.
 * @param[in]   chn - channel number (0-15).
 * @return      none
 * @note        Writes to registers areg_0x8f/0x90 (16-bit access, covers channels 0-15).
 */
static inline void ctb_irq_en(ctb_chn_num_e chn)
{
    analog_write_reg16(areg_0x8f,analog_read_reg16(areg_0x8f) | BIT(chn));
}

/**
 * @brief       This function serves to configure the CTB clock and reset control register.
 * @param[in]   clk_cfg - value to write to register areg_0x94.
 *                        bit[0]: CTB_CLK_EN (CTB clock enable),
 *                        bit[1]: CTB_RST_SYNC (synchronous reset for CTB registers).
 * @return      none
 */
static inline void ctb_clk_rst_cfg(unsigned char clk_cfg)
{
    analog_write_reg8(areg_0x94, clk_cfg);
}

/**
 * @brief       This function serves to enable the 32kHz clock for CTB.
 * @param[in]   none
 * @return      none
 * @note        Sets bit[1] in register areg_0x1ee.
 */
static inline void ctb_clk_32k_en(void)
{
    analog_write_reg8(areg_0x1ee, 0x02);
}

/**
 * @brief       This function serves to enable the min/max tracking for CTB calibration.
 * @param[in]   none
 * @return      none
 * @note        Sets bit[0] in register areg_0x1ee. Used during threshold calibration.
 */
static inline void ctb_min_max_en(void)
{
    analog_write_reg8(areg_0x1ee, analog_read_reg8(areg_0x1ee) | BIT(0));
}

/**
 * @brief       This function serves to set the CTB calibration control register.
 * @param[in]   calib - value to write to register areg_0x99.
 *                      bit[0]: FLD_CTB_CALBR_EN,
 *                      bit[1]: FLD_CTB_CALBR_FINISHED_FLAG_RST,
 *                      bit[2]: FLD_CTB_CALBR_TYPE,
 *                      bit[3]: FLD_CTB_CALBR_RST,
 *                      bits[4:5]: FLD_CTB_CALBR_ITERATION_NUM.
 * @return      none
 */
static inline void ctb_set_calib_ctrl(unsigned char calib)
{
    analog_write_reg8(areg_0x99, calib);
}

/**
 * @brief       This function serves to set the CTB scan time target.
 * @param[in]   none
 * @return      none
 * @note        Writes 0x0f to register areg_0x9d (scan time for channels 0-7).
 */
static inline void ctb_set_scan_time(void)
{
    analog_write_reg16(areg_0x9d, 0x0f);
}

/**
 * @brief       This function serves to set the Slew Rate Limiter (SRL) filter step size.
 * @param[in]   step - step size value (register areg_0x93, bits[4:0]).
 * @return      none
 */
static inline void ctb_set_srl_step_size(unsigned char step)
{
    analog_write_reg8(areg_0x93, step);
}

/**
 * @brief       This function serves to set the number of cycles per sample.
 * @param[in]   none
 * @return      none
* @note        Writes 0x1f to register areg_0x97 (cycles per sample for channels 0-7).
 */
static inline void ctb_set_cycles_per_sample(void)
{
    analog_write_reg16(areg_0x97,0x1f);
}

/**
 * @brief       This function serves to set the debounce and SRL filter parameters.
* @param[in]   filter   - number of samples per reading for SRL filter (register areg_0x91, bits[7:4]).
 * @param[in]   debounce - number of successive touched readings before firing IRQ (register areg_0x91, bits[3:0]).
 * @return      none
 */
static inline void ctb_set_debounce_srl_filter(unsigned char filter, unsigned char debounce)
{
    analog_write_reg8(areg_0x91,(unsigned char)(filter << 4 | debounce));
}

/**
 * @brief       This function serves to configure the filter settings.
* @param[in]   mode - SRL scan history selection (0: use new sample, 1: use previous reading) (register areg_0x92, bit[7]).
 * @param[in]   sign - filter sign selection (0: add, 1: minus) (register areg_0x92, bit[6]).
 * @param[in]   coff - Butterworth filter coefficient masks (register areg_0x92, bits[5:0]).
 * @return      none
 */
static inline void ctb_set_filter_cfg(unsigned char mode, unsigned char sign, unsigned char coff)
{
    analog_write_reg8(areg_0x92, (unsigned char)(mode << 7 | sign << 6 | coff));
}

/**
 * @brief       This function serves to enable the CTB error interrupt.
 * @param[in]   none
 * @return      none
* @note        Writes 0x03 to register areg_0x9c (enables error IRQ bits[1:0]).
 */
static inline void ctb_error_irq_en(void)
{
    analog_write_reg8(areg_0x9c,0x03);
}

/**
 * @brief       This function serves to set the filter gain selection (output divider).
* @param[in]   gain - gain value (register areg_0x9f, bits[2:0]).
 *                    000: /1, 001: /2, 010: /4, 011: /8,
 *                    100: /16, 101: /32, 110: /64, 111: /128.
 * @return      none
 */
static inline void ctb_set_filter_gain_sel(unsigned char gain)
{
    analog_write_reg8(areg_0x9f, gain);
}

/**
 * @brief       This function serves to enable the CTB scan.
 * @param[in]   none
 * @return      none
* @note        Sets scan_en bit (bit[2]) in register areg_0xb0.
 */
static inline void ctb_scan_en(void)
{
    analog_write_reg8(areg_0xb0, analog_read_reg8(areg_0xb0) | BIT(2));
}

/**
 * @brief       This function serves to set the CTB interrupt mask.
 * @param[in]   none
 * @return      none
* @note        Sets bit[5] in register areg_0x73 to enable CTB interrupt in the top-level interrupt controller.
 */
static inline void ctb_set_irq_mask(void)
{
    analog_write_reg8(areg_0x73, analog_read_reg8(areg_0x73) | BIT(5));
}

/**
 * @brief       This function serves to enable CTB for normal operation.
 * @param[in]   chn - channel number.
 * @return      none
 */
void ctb_enable(ctb_chn_num_e chn);

/**
 * @brief       This function serves to calibrate CTB for a single channel
 *              (first stage: switch capacitor calibration, second stage: threshold calibration).
 * @param[in]   chn - channel number.
 * @return      none
 */
void ctb_calib(ctb_chn_num_e chn);

/**
 * @brief       This function serves to configure the GPIO pin for CTB function.
 * @param[in]   pin - the GPIO pin to be configured as CTB input.
 * @return      none
 */
void ctb_set_pin(gpio_pin_e pin);

/**
 * @brief       This function serves to manually configure the sensitivity threshold for specified CTB channels.
 * @param[in]   chn_mask - bitmask of channels to configure (e.g., BIT(0) | BIT(1) for channels 0 and 1).
 * @param[in]   sensitivity - the sensitivity threshold value to set for each channel in chn_mask.
 * @return      none
 * @note        If this API is not called, the default values will be used.
 *              Smaller sensitivity value → higher sensitivity, but also more susceptible to false triggers.
 */
void ctb_sensitivity_manual_config(unsigned int chn_mask, unsigned int sensitivity);

/**
 * @brief       This function serves to initialize the CTB module with specified channels.
 * @param[in]   chn_mask - bitmask of channels to initialize (e.g., BIT(0) | BIT(1) for channels 0 and 1).
 * @return      none
 * @note        This function performs GPIO setup, calibration, and enablement for all
 *              channels specified in the mask, then sets IRQ mask and wakeup.
 */
void ctb_init(unsigned int chn_mask);