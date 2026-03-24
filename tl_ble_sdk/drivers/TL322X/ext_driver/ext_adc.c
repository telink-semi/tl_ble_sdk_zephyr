/********************************************************************************************************
 * @file    ext_adc.c
 *
 * @brief   This is the source file for BLE SDK
 *
 * @author  BLE GROUP
 * @date    06,2024
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
#include "../driver.h"
#include "ext_adc.h"

#define SD_ADC_SAMPLE_CNT  16
/*
 *  The length of sd_adc_sample_buffer must be >= SD_ADC_FIFO_DEPTH, otherwise there is a risk of array overflow.
 */
signed int sd_adc_sample_buffer[SD_ADC_SAMPLE_CNT] __attribute__((aligned(4))) = {0};
signed int sd_adc_sort_and_get_average_code(signed int *sample_buffer);


/**
 * @brief Legacy SD ADC GPIO configuration (for reference, replaced by hal_adc_common_cfg_t)
 */
sd_adc_gpio_cfg_t sd_adc_gpio_cfg =
{
    .chn                = SD_ADC_DC0,
    .clk_freq           = SD_ADC_SAPMPLE_CLK_2M,  // Note: "SAPMPLE" may be a typo for "SAMPLE"
    .downsample_rate    = SD_ADC_DOWNSAMPLE_RATE_128,
    .gpio_div           = SD_ADC_GPIO_CHN_DIV_1F4,
    .input_p            = SD_ADC_GPIO_PC0P,
    .input_n            = SD_ADC_GNDN
};

/**
 * @brief Legacy SD ADC GPIO pin configuration (for reference, replaced by hal_adc_common_cfg_t)
 */
sd_adc_gpio_pin_cfg_t sd_adc_dc0_pin_cfg =
{
    .input_p            = SD_ADC_GPIO_PC0P,
    .input_n            = SD_ADC_GNDN
};

static sd_adc_vbat_div_e        sd_adc_vbat_div = SD_ADC_VBAT_DIV_1F4; // SD ADC VBAT division ratio (e.g., 1/4)

static sd_adc_result_type_e     sd_adc_result_type = SD_ADC_VOLTAGE_MV; // SD ADC output result type (e.g., mV, temperature)

/**
 * @brief Global buffer for ADC sampling data (defined in the project)
 */


// -----------------------------------------------------------------------------
// Public HAL Layer Functions (External Interface)
// -----------------------------------------------------------------------------
/**
 * @brief ADC HAL layer initialization (external interface)
 * @param adc_cfg [in] Pointer to ADC main configuration struct
 * @return hal_adc_status_e Initialization result (HAL_ADC_OK if successful)
 */
hal_adc_status_e ext_adc_init(const hal_adc_cfg_t *adc_cfg) {


    // 1. Validate input parameters first
    // 1. Basic parameter check (null pointer/invalid mode)
      if (adc_cfg == NULL) {
          return HAL_ADC_ERR_PARAM;
      }

    // 2. Parse hw_priv (including GPIO config): Use defaults if not provided
    if(adc_cfg->adc_mode == HAL_ADC_MODE_GPIO)
    {
        sd_adc_dc0_pin_cfg.input_p  = adc_cfg->gpio_cfg.input_p;
        sd_adc_dc0_pin_cfg.input_n  = adc_cfg->gpio_cfg.input_n;
    }

    if (adc_cfg->hw_priv != NULL) {
         hal_adc_common_cfg_t *user_cfg = ( hal_adc_common_cfg_t *)adc_cfg->hw_priv;
        // Overwrite default config with user-provided values

        sd_adc_gpio_cfg.chn                = user_cfg->chn;
        sd_adc_gpio_cfg.clk_freq           = user_cfg->clk_freq;
        sd_adc_gpio_cfg.downsample_rate    = user_cfg->downsample_rate;
        sd_adc_gpio_cfg.gpio_div           = user_cfg->gpio_div;

        sd_adc_vbat_div                    = user_cfg->sd_adc_vbat_div;
        sd_adc_result_type                 = user_cfg->sd_adc_result_type;
    }

    //--------------------------The following are all based on the driver demo.----------------------------------------------

    //  Initialize underlying SD ADC (single DC mode)
    sd_adc_init(SD_ADC_SINGLE_DC_MODE);

    //  Initialize ADC based on working mode
    switch (adc_cfg->adc_mode) {
        case HAL_ADC_MODE_GPIO: {
            // GPIO mode: Reuse legacy GPIO config (replaceable with s_adc_common_cfg for consistency)
            sd_adc_gpio_sample_init( &sd_adc_gpio_cfg);
            break;
        }
        case HAL_ADC_MODE_VBAT: {
            // VBAT mode: Use config from s_adc_common_cfg
            sd_adc_vbat_sample_init(sd_adc_gpio_cfg.clk_freq,sd_adc_vbat_div,sd_adc_gpio_cfg.downsample_rate);
            break;
        }
        #if EXT_ADC_TEMP_MODE_EN
        case HAL_ADC_MODE_TEMP: {
            // Temperature mode: Use default clock/downsample (replaceable with s_adc_common_cfg)
            sd_adc_temp_init(SD_ADC_SAPMPLE_CLK_2M,  SD_ADC_DOWNSAMPLE_RATE_128);
        }
        #endif
        default:
            return HAL_ADC_ERR_MODE; // Unsupported mode
    }

    // 5. Initialize DMA if enabled (controlled by SAMPLE_MODE and trans_mode)
#if EXT_ADC_DMA_FUNC_EN
#error "adc not compatible"
    if (adc_cfg->trans_mode == HAL_ADC_TRANS_DMA) {
        // Configure SD ADC DMA channel
        sd_adc_set_dma_config(adc_cfg->dma_chn);
        // Enable DMA transfer complete interrupt mask
        dma_set_irq_mask(adc_cfg->dma_chn, TC_MASK);
        // Enable DMA interrupt in PLIC controller
        plic_interrupt_enable(IRQ_DMA);

        // Start ADC sampling with DMA (buffer size = sample_cnt * 4 bytes per int32_t)
        sd_adc_start_sample_dma(
            (signed int *)adc_cfg->dma_cfg.sample_buf,
            adc_cfg->dma_cfg.sample_cnt << 2  // Equivalent to sample_cnt * 4
        );
    }
#endif

    return HAL_ADC_OK;
}


/**
 * @brief       This function serves to sort and get average code.
 * @param[in]   sd_adc_data_buf -Pointer to sd_adc_data_buf
 * @return      average code
 * @note        If sd_adc_sort_and_get_average_code() interface is called, SD_ADC_SAMPLE_CNT must be a multiple of 4.
 */
signed int sd_adc_sort_and_get_average_code(signed int *sample_buffer)
{
    int i, j;
    signed int sd_adc_code_average = 0;
    signed int temp;

    /**** insert Sort and get average value ******/
    for(i = 1 ;i < SD_ADC_SAMPLE_CNT; i++)
    {
        if(sample_buffer[i] < sample_buffer[i-1])
        {
            temp = sample_buffer[i];
            sample_buffer[i] = sample_buffer[i-1];
            for(j=i-1; j>=0 && sample_buffer[j] > temp;j--)
            {
                sample_buffer[j+1] = sample_buffer[j];
            }
            sample_buffer[j+1] = temp;
        }
    }
    //get average value from raw data(abandon 1/4 small and 1/4 big data)
    for (i = SD_ADC_SAMPLE_CNT>>2; i < (SD_ADC_SAMPLE_CNT - (SD_ADC_SAMPLE_CNT>>2)); i++)
    {
        sd_adc_code_average += sample_buffer[i]/(SD_ADC_SAMPLE_CNT>>1);
    }
    return sd_adc_code_average;
}


/**
 * @brief Read ADC sampling data (external interface)
 * @return signed int Processed ADC result
 */
signed int ext_adc_read_data(void) {
    signed int code_average = 0;  // Average of sorted sampling codes (eliminates outliers)
    signed int sd_adc_result = 0; // Final processed result

//#if (SAMPLE_MODE == EXT_NDMA_POLLING_MODE)
#if 1
    // Polling mode: Read data from ADC FIFO until buffer is full
    int cnt = 0;
    while (cnt < SD_ADC_SAMPLE_CNT) {
        // Get number of available data in FIFO
        int sample_cnt = sd_adc_get_rxfifo_cnt();
        if (sample_cnt > 0) {
            // Read raw code from FIFO and store in buffer
            sd_adc_sample_buffer[cnt] = sd_adc_get_raw_code();
            cnt++;
        }
    }
#endif

    // Calculate average code after sorting
    code_average = sd_adc_sort_and_get_average_code(sd_adc_sample_buffer);

    // Generate final result based on configured result type
    switch (sd_adc_result_type) {
        case SD_ADC_VOLTAGE_10X_MV:
            // Result type: Voltage
            sd_adc_result = sd_adc_calculate_voltage(code_average, SD_ADC_VOLTAGE_10X_MV);
            break;
        case SD_ADC_VOLTAGE_MV:
            // Result type: Voltage
            sd_adc_result = sd_adc_calculate_voltage(code_average, SD_ADC_VOLTAGE_MV);
            break;
        #if EXT_ADC_TEMP_MODE_EN
        case TEMP_VALUE:
            // Result type: Temperature
            sd_adc_result = sd_adc_calculate_temperature(code_average);
            break;
        #endif
        default:
            // Invalid result type: Return 0
            sd_adc_result = 0;
            break;
    }


    return sd_adc_result;
}



