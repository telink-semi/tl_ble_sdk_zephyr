/********************************************************************************************************
 * @file    ext_mailbox.h
 *
 * @brief   This is the header file for BLE SDK
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

#ifndef DRIVERS_TL322X_EXT_DRIVER_EXT_MAILBOX_H_
#define DRIVERS_TL322X_EXT_DRIVER_EXT_MAILBOX_H_

typedef enum
{
    FLD_MAILBOX_D25F_TO_DSP_IRQ = BIT(0), /*when d25f write two words into d25f_to_dsp register, dsp will generate an interrupt and this interrupt flag bit will be set.
                                            After dsp read two words, the hardware will automatically clear the interrupt flag bit (this function is hardware fixed line, not register configurable)*/

    FLD_MAILBOX_DSP_TO_D25F_IRQ = BIT(1), /*when dsp  write two words into dsp_to_d25f register, d25f will generate an interrupt and this interrupt flag bit will be set.
                                            After d25f read two words, the hardware will automatically clear the interrupt flag bit (this function is hardware fixed line, not register configurable)*/

    FLD_MAILBOX_D25F_TO_N22_IRQ = BIT(2), /*when d25f write two words into d25f_to_n22 register, n22 will generate an interrupt and this interrupt flag bit will be set.
                                            After n22 read two words, the hardware will automatically clear the interrupt flag bit (this function is hardware fixed line, not register configurable)*/

    FLD_MAILBOX_N22_TO_D25F_IRQ = BIT(3), /*when n22 write two words into n22_to_d25f register, d25f will generate an interrupt and this interrupt flag bit will be set.
                                            After d25f read two words, the hardware will automatically clear the interrupt flag bit (this function is hardware fixed line, not register configurable)*/

    FLD_MAILBOX_N22_TO_DSP_IRQ = BIT(4),  /*when n22 write two words into n22_to_dsp register, dsp will generate an interrupt and this interrupt flag bit will be set.
                                            After dsp read two words, the hardware will automatically clear the interrupt flag bit (this function is hardware fixed line, not register configurable)*/

    FLD_MAILBOX_DSP_TO_N22_IRQ = BIT(5),  /*when dsp write two words into dsp_to_n22 register, n22 will generate an interrupt and this interrupt flag bit will be set.
                                            After n22 read two words, the hardware will automatically clear the interrupt flag bit (this function is hardware fixed line, not register configurable)*/
} mailbox_irq_status_e;

/**
 * @brief     This function servers to set mailbox irq mask.
 * @param[in] mask - mailbox irq mask.
 * @return    none
 */
static inline void mailbox_set_irq_mask(mailbox_irq_status_e mask)
{
    if(mask == FLD_MAILBOX_N22_TO_D25F_IRQ){
        mailbox_set_irq_mask_d25f();
    }
    else if(mask == FLD_MAILBOX_D25F_TO_N22_IRQ){
        mailbox_set_irq_mask_n22();
    }

}

/**
 * @brief     This function servers to get the mailbox interrupt status.
 * @param[in] status    - variable of enum to select the mailbox interrupt source.
 * @retval    non-zero      - the interrupt occurred.
 * @retval    zero  - the interrupt did not occur.
 */
__INLINE unsigned char mailbox_get_irq_status(void)
{
#ifdef MCU_CORE_N22_ENABLE
    if(mailbox_get_irq_status_n22()){
        return FLD_MAILBOX_D25F_TO_N22_IRQ;
    }
#else
    if(mailbox_get_irq_status_d25f()){
        return  FLD_MAILBOX_N22_TO_D25F_IRQ;
    }
#endif
    return 0;
}

__INLINE void mailbox_clr_irq_status(void)
{
#ifdef MCU_CORE_N22_ENABLE
    mailbox_clr_irq_status_n22();
#else
    mailbox_clr_irq_status_d25f();
#endif
}

__INLINE void mailbox_get_msg(void *msg)
{
#ifdef MCU_CORE_N22_ENABLE
    mailbox_n22_get_d25f_msg((unsigned int *)msg);
#else
    mailbox_d25f_get_n22_msg((unsigned int *)msg);
#endif
}

#endif /* DRIVERS_TL322X_EXT_DRIVER_EXT_MAILBOX_H_ */
