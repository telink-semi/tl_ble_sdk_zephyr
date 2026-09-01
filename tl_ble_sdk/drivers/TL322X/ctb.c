/********************************************************************************************************
 * @file    ctb.c
 *
 * @brief   This is the source file for tl322x CTB (Capacitive Touch Button) driver
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
#include "ctb.h"
#include "gpio.h"
#include "lpc.h"
#include "lib/include/stimer.h"

/** @brief  Mapping from CTB channel number to GPIO pin.
 *  @note   Channels 0-7 use GPIO_PC0-PC7, channels 8-15 use GPIO_PB0-PB7.
 */
volatile const gpio_pin_e ctb_in_pins[CTB_CHN_NUM_MAX] = {
    GPIO_PC0,   // CTB_IN0  - Channel 0
    GPIO_PC1,   // CTB_IN1  - Channel 1
    GPIO_PC2,   // CTB_IN2  - Channel 2
    GPIO_PC3,   // CTB_IN3  - Channel 3
    GPIO_PC4,   // CTB_IN4  - Channel 4
    GPIO_PC5,   // CTB_IN5  - Channel 5
    GPIO_PC6,   // CTB_IN6  - Channel 6
    GPIO_PC7,   // CTB_IN7  - Channel 7
    GPIO_PB0,   // CTB_IN8  - Channel 8
    GPIO_PB1,   // CTB_IN9  - Channel 9
    GPIO_PB2,   // CTB_IN10 - Channel 10
    GPIO_PB3,   // CTB_IN11 - Channel 11
    GPIO_PB4,   // CTB_IN12 - Channel 12
    GPIO_PB5,   // CTB_IN13 - Channel 13
    GPIO_PB6,   // CTB_IN14 - Channel 14
    GPIO_PB7    // CTB_IN15 - Channel 15
};

/**
 * @brief       The threshold value for each channel.
 */
static volatile unsigned int ctb_threshold[CTB_CHN_NUM_MAX] = {
    100,    // Channel 0 threshold
    100,    // Channel 1 threshold
    100,    // Channel 2 threshold
    100,    // Channel 3 threshold
    100,    // Channel 4 threshold
    100,    // Channel 5 threshold
    100,    // Channel 6 threshold
    100,    // Channel 7 threshold
    100,    // Channel 8 threshold
    100,    // Channel 9 threshold
    100,    // Channel 10 threshold
    100,    // Channel 11 threshold
    100,    // Channel 12 threshold
    100,    // Channel 13 threshold
    100,    // Channel 14 threshold
    100     // Channel 15 threshold
};

unsigned int g_ctb_error_timeout_us = 1000000; /**< Timeout value (in microseconds) for CTB calibration wait loops */

/**
 * @brief     This function serves to record the api status.
 * @param[in] ctb_error_timeout_code - ctb_api_error_code_e.
 * @return    none.
 * @note      This function can be rewritten according to the application scenario,The parameters of the interface are useless(only one reason for an error,do not need to use enumeration to distinguish it),
 *            if record the details of the reason, can implement it by yourself,trng_hw_reset must be called.
 */
__attribute__((weak)) void ctb_timeout_handler(unsigned int ctb_error_timeout_code)
{
    (void)ctb_error_timeout_code;
}

/**
 * @brief       This function serves to calibrate the channel switch capacitor.
 * @param[in]   chn - channel number.
 * @return      none
 * @note        Reads the calibrated value from register areg_0x1ef (reference value),
 *              subtracts 30 as offset, and writes to the channel's switch_cap_val register:
 *                - Channel 0: register areg_0x87
 *                - Channel 1: register areg_0x89
 *                - Channels 2-15: register areg_0x1d2 + (chn-2)*2
 */
static void ctb_calib_chn_switch_cap(ctb_chn_num_e chn)
{

    if(chn == CTB_CHN_NUM_0)
    {
        analog_write_reg16(areg_0x87,(analog_read_reg16(areg_0x1ef)-30));
    }else if(chn == CTB_CHN_NUM_1)
    {
        analog_write_reg16(areg_0x89,(analog_read_reg16(areg_0x1ef)-30));
    }else
    {
        analog_write_reg16(areg_0x1d2 + (chn-2)*2,(analog_read_reg16(areg_0x1ef)-30));
    }
}

/**
 * @brief       This function serves to calibrate the channel threshold.
 * @param[in]   chn - channel number.
 * @return      none
 * @note        Reads the calibrated value from register areg_0x1ef (reference value),
 *              adds ctb_threshold as margin, and writes to the channel's threshold register:
 *                - Channel 0: register areg_0x83
 *                - Channel 1: register areg_0x85
 *                - Channels 2-15: register areg_0x1b6 + (chn-2)*2
 */
static void ctb_calib_chn_threshold(ctb_chn_num_e chn)
{
    if(chn == CTB_CHN_NUM_0)
    {
        analog_write_reg16(areg_0x83,(analog_read_reg16(areg_0x1ef) + ctb_threshold[chn]));
    }else if(chn == CTB_CHN_NUM_1)
    {
        analog_write_reg16(areg_0x85,(analog_read_reg16(areg_0x1ef) + ctb_threshold[chn]));
    }else
    {
        analog_write_reg16(areg_0x1b6 + (chn-2)*2,(analog_read_reg16(areg_0x1ef) + ctb_threshold[chn]));
    }
}

/**
 * @brief       This function serves to set the large step and fine step capacitor values.
 * @param[in]   chn - channel number.
 * @return      none
 * @note        Writes 0x1216 to the channel's step capacitor config register:
 *                - Channel 0: register areg_0x8b (large step & fine step combined)
 *                - Channel 1: register areg_0x8d
 *                - Channels 2-15: register areg_0x19a + (chn-2)*2
 *              The value 0x1216 encodes both large_step and fine_step capacitor settings.
 */
static void ctb_set_lstep_fstep_cap(ctb_chn_num_e chn)
{
    if(chn == CTB_CHN_NUM_0)
    {
        analog_write_reg16(areg_0x8b ,0x1216);
    }else if(chn == CTB_CHN_NUM_1)
    {
        analog_write_reg16(areg_0x8d ,0x1216);
    }else
    {
        analog_write_reg16(areg_0x19a + (chn-2)*2,0x1216);
    }
}

/**
 * @brief       This function serves to set the default switch capacitor value (pre-calibration).
 * @param[in]   chn - channel number.
 * @return      none
 * @note        Writes 0x1fff (maximum value) to the channel's switch_cap_val register:
 *                - Channel 0: register areg_0x87
 *                - Channel 1: register areg_0x89
 *                - Channels 2-15: register areg_0x1d2 + (chn-2)*2
 */
static void ctb_set_default_switch_cap(ctb_chn_num_e chn)
{
    if(chn == CTB_CHN_NUM_0)
    {
        analog_write_reg16(areg_0x87,0x1fff);
    }else if(chn == CTB_CHN_NUM_1)
    {
        analog_write_reg16(areg_0x89,0x1fff);
    }else
    {
        analog_write_reg16(areg_0x1d2 + (chn-2)*2,0x1fff);
    }
}

/**
 * @brief       This function serves to perform the first stage calibration (switch capacitor calibration).
 * @param[in]   chn - channel number.
 * @return      none
 * @note        Configures the channel and calibration engine, enables scan,
 *              sets default threshold and switch capacitor values, clears IRQ
 *              mask during calibration, and starts calibration with
 *              FLD_CTB_CALBR_CAP type. The result is read back in ctb_calib().
 */
static void ctb_cap_calib(ctb_chn_num_e chn)
{
    ctb_clk_rst_cfg(0x00);
    ctb_channel_en1(BIT(chn));
    ctb_set_calib_ctrl(0x2f);
    ctb_set_calib_ctrl(0x2d);
    ctb_set_srl_step_size(0x25);
    ctb_clk_32k_en();
    ctb_clk_rst_cfg(0x01);
    ctb_clk_32k_en();
    ctb_set_scan_time();
    ctb_irq_en(chn);
    ctb_set_cycles_per_sample();
    ctb_set_debounce_srl_filter(0x2, 0x2);
    ctb_set_srl_step_size(0x25);
    ctb_set_filter_cfg(0x0, 0x0, 0x0);
    ctb_set_filter_gain_sel(0x00);
    ctb_error_irq_en();
    ctb_calib_chn_threshold(chn);
    ctb_set_lstep_fstep_cap(chn);
    ctb_set_default_switch_cap(chn);
    ctb_set_irq_mask();
    ctb_scan_en();
    ctb_clk_rst_cfg(0x03);
    /* Wait for RST to take effect*/
    delay_us(65);
    ctb_clk_rst_cfg(0x01);
    ctb_set_calib_ctrl(0x25);
    ctb_clr_all_irq();
}

/**
 * @brief       This function serves to perform the second stage calibration (threshold calibration).
 * @param[in]   chn - channel number.
 * @return      none
 * @note        Similar to ctb_cap_calib(), but enables min/max tracking and
 *              sets default threshold value. Uses FLD_CTB_CALBR_THRESHOLD type.
 *              The result is read back in ctb_calib().
 */
static void ctb_threshold_calib(ctb_chn_num_e chn)
{
    ctb_clk_rst_cfg(0x00);
    ctb_channel_en1(BIT(chn));
    ctb_set_calib_ctrl(0x2f);
    ctb_set_calib_ctrl(0x29);
    ctb_set_srl_step_size(0x25);
    ctb_clk_32k_en();
    ctb_min_max_en();
    ctb_clk_rst_cfg(0x01);
    ctb_set_scan_time();
    ctb_irq_en(chn);
    ctb_set_cycles_per_sample();
    ctb_set_debounce_srl_filter(0x2, 0x2);
    ctb_set_srl_step_size(0x25);
    ctb_set_filter_cfg(0x0, 0x0, 0x0);
    ctb_set_filter_gain_sel(0x00);
    ctb_error_irq_en();
    ctb_set_lstep_fstep_cap(chn);
    ctb_scan_en();
    ctb_clk_rst_cfg(0x03);
    /* Wait for RST to take effect*/
    delay_us(65);
    ctb_clk_rst_cfg(0x01);
    ctb_set_calib_ctrl(0x21);
    ctb_clr_all_irq();
}

/**
 * @brief       This function serves to enable CTB for normal operation.
 * @param[in]   chn - channel number.
 * @return      none
 * @note        Disables calibration, configures filter parameters for normal
 *              touch detection (SRL step=0x05, gain=/8, BW filter coeff=0x03),
 *              and enables scan.
 */
void ctb_enable(ctb_chn_num_e chn)
{
    ctb_set_calib_ctrl(0x00);
    ctb_set_scan_time();
    ctb_channel_en(BIT(chn));
    ctb_irq_en(chn);
    ctb_set_cycles_per_sample();
    ctb_set_debounce_srl_filter(0x2, 0x2);
    ctb_set_srl_step_size(0x05);
    ctb_set_filter_gain_sel(0x03);
    ctb_set_filter_cfg(0x0, 0x0, 0x03);
    ctb_error_irq_en();
    ctb_set_lstep_fstep_cap(chn);
    ctb_scan_en();
    ctb_clk_rst_cfg(0x03);
    ctb_clk_32k_en();
    /* Wait for RST to take effect*/
    delay_us(65);
    ctb_clk_rst_cfg(0x01);
    ctb_clr_all_irq();
}

/**
 * @brief       This function serves to calibrate CTB for a single channel.
 * @param[in]   chn - channel number.
 * @return      none
 * @note        Two-stage calibration:
 *              1. ctb_cap_calib(): switch capacitor calibration, waits for done,
 *                 then reads back the result via ctb_calib_chn_switch_cap().
 *              2. ctb_threshold_calib(): threshold calibration, waits for done,
 *                 then reads back the result via ctb_calib_chn_threshold().
 */
void ctb_calib(ctb_chn_num_e chn)
{
    ctb_cap_calib(chn);
    wait_condition_fails_or_timeout(ctb_get_calib_done, g_ctb_error_timeout_us, ctb_timeout_handler, (unsigned int)0);
    ctb_calib_chn_switch_cap(chn);
    ctb_threshold_calib(chn);
    wait_condition_fails_or_timeout(ctb_get_calib_done, g_ctb_error_timeout_us, ctb_timeout_handler, (unsigned int)0);
    ctb_calib_chn_threshold(chn);
}

/**
 * @brief       This function serves to configure the GPIO pin for CTB function.
 * @param[in]   pin - the GPIO pin to be configured as CTB input.
 * @return      none
 * @note        Sets the pin's analog function register based on the pin's bit
 *              position, clears digital input, and sets output enable.
 */
void ctb_set_pin(gpio_pin_e pin)
{
    unsigned char bit = pin & 0xff;
    unsigned int addr = areg_0x17 + (pin >> 8) * 2;
    unsigned short data = analog_read_reg16(addr) & ~(0x03 << (bit == 0x80 ? 14 : bit == 0x40 ? 12 : bit == 0x20 ? 10 : bit == 0x10 ? 8 : bit == 0x08 ? 6 : bit == 0x04 ? 4 : bit == 0x02 ? 2 : 0));
    BM_SET(reg_gpio_func(pin), bit);
    BM_CLR(reg_gpio_ie(pin), bit);
    BM_SET(reg_gpio_oen(pin), bit);
    analog_write_reg16(addr, data);
}

/**
 * @brief       This function serves to manually configure the sensitivity threshold for specified CTB channels.
 * @param[in]   chn_mask - bitmask of channels to configure (e.g., BIT(0) | BIT(1) for channels 0 and 1).
 * @param[in]   sensitivity - the sensitivity threshold value to set for each channel in chn_mask.
 * @return      none
 * @note        If this API is not called, the default values will be used.
 *              Smaller sensitivity value → higher sensitivity, but also more susceptible to false triggers.
 *              Sensitivity config must come before ctb_init.
 *              This API supports multiple calls for separate channel sensitivity setup.
 */
void ctb_sensitivity_manual_config(unsigned int chn_mask, unsigned int sensitivity)
{
    unsigned int mask = chn_mask;
    // Set threshold for all channels
    while (mask) {
        unsigned char i = (unsigned char)CTB_BIT_POSITION(mask);
        ctb_threshold[i] = sensitivity;
        mask &= mask - 1;   // Clear the lowest set bit
    }
}

/**
 * @brief       This function serves to initialize the CTB module with specified channels.
 * @param[in]   chn_mask - bitmask of channels to initialize (e.g., BIT(0) | BIT(1) for channels 0 and 1).
 * @return      none
 * @note        Performs the full initialization sequence:
 *              1. Enable NVM IO.
 *              2. For each channel in chn_mask: configure GPIO pin.
 *              3. For each channel in chn_mask: perform calibration.
 *              4. For each channel in chn_mask: enable CTB operation.
 *              5. Set IRQ mask and wakeup enable.
 */
void ctb_init(unsigned int chn_mask)
{
    // Enable NVM register access
    reg_nvm_ctrl0 |= FLD_REG_EN_IO_NVM;

    unsigned int mask = chn_mask;
    // Configure input pins for all channels
    while (mask) {
        unsigned char i = (unsigned char)CTB_BIT_POSITION(mask);
        ctb_set_pin(ctb_in_pins[i]);
        mask &= mask - 1;   // Clear the lowest set bit
    }

    mask = chn_mask;
    // Calibrate all channels
    while (mask) {
        unsigned char i = (unsigned char)CTB_BIT_POSITION(mask);
        ctb_calib(i);
        mask &= mask - 1;
    }

    mask = chn_mask;
    // Enable all channels
    while (mask) {
        unsigned char i = (unsigned char)CTB_BIT_POSITION(mask);
        ctb_enable(i);
        mask &= mask - 1;
    }

    // Set interrupt mask and enable wakeup
    ctb_set_irq_mask();
    ctb_wakeup_enable();
}