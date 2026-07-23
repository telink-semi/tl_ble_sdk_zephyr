/********************************************************************************************************
 * @file    cs.h
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
#ifndef STACK_BLE_CONTROLLER_LL_CHN_SOUND_CHANNEL_SOUND_H_
#define STACK_BLE_CONTROLLER_LL_CHN_SOUND_CHANNEL_SOUND_H_


#include "stack/ble/hci/hci_cmd.h"

/* Maximum number of CS configuration IDs */
#define CS_CONFIG_ID_MAX 3

/* Maximum number of subevents per CS procedure
 * User can change smaller to save ram size or
 * larger to support more subevents in a procedure 
 */
#define CS_SUBEVENT_PER_PROCEDURE_MAX 8

/* Maximum number of supported antenna paths */
#ifndef MAX_ANT_PATHS_SUPPORT
    #define MAX_ANT_PATHS_SUPPORT 0X02
#endif

/* Maximum number of steps in sub-mode for a procedure.
 * User can change this value.
 * To make sure RTT accuracy, at least need 8 sub-mode steps.
 */
#define CS_SUB_MODE_MAX_STEP_NUM            12
/* RTT ToD/ToA threshold value */
#define CS_RTT_TOD_TOA_THRESHOLD            0x8000

/* Maximum number of steps in Mode 0 */
#define CS_MODE0_MAX_STEP_NUM               3
/* Maximum number of steps in Main Mode */
#define CS_MAIN_MODE_MAX_STEP_NUM           72
/* Total number of steps per subevent (Mode0 + MainMode + SubMode) */
#define STEP_NUM_PER_SUBEVENT               (CS_MODE0_MAX_STEP_NUM + CS_MAIN_MODE_MAX_STEP_NUM + CS_SUB_MODE_MAX_STEP_NUM)

/* Length of phase measurement item in bytes */
#define PHASE_ITEM_LEN             8
/* Length of amplitude measurement item in bytes */
#define AMP_ITEM_LEN               1
/* Maximum step length for Mode 0 */
#define CS_MODE0_STEP_LEN_MAX      (5 + 3)
/* Maximum step length for Mode 1 */
#define CS_MODE1_STEP_LEN_MAX      (14 + 3)
/* Maximum step length for Mode 2 */
#define CS_MODE2_STEP_LEN_MAX      ((MAX_ANT_PATHS_SUPPORT + 1) * 4 + 1 + 3)
/* Maximum step length for Mode 3 (PBR+RTT) */
#define CS_MODE3_STEP_LEN_MAX      (CS_MODE1_STEP_LEN_MAX + CS_MODE2_STEP_LEN_MAX - 3)
/* Maximum subevent step length, the larger of Mode 1 and Mode 2 */
#define CS_SUBEVENT_STEP_LEN_MAX   ((CS_MODE2_STEP_LEN_MAX > CS_MODE1_STEP_LEN_MAX) ? CS_MODE2_STEP_LEN_MAX : CS_MODE1_STEP_LEN_MAX)

/* Maximum number of steps per CS procedure */
#define CS_STEPS_PER_PROCEDURE_MAX (72 + 10 + (3 * CS_SUBEVENT_PER_PROCEDURE_MAX))

/* Maximum subevent buffer length */
#define CS_SUBEVENT_BUFF_LEN_MAX   (16 + CS_SUBEVENT_STEP_LEN_MAX * STEP_NUM_PER_SUBEVENT)

/* Size of DRBG-generated information per step (channel, access address, sequence, etc.) 
 * user can't change this value!!!
 */
#define CS_STEP_DRBG_INFO_SIZE        52

/*
 * Length of struct to control all cs status. user can't change this value!!!
 */
#define CS_PARAM_LENGTH               1484//(1472 + (52 * STEP_NUM_PER_SUBEVENT)) //user can't modify this value !!!


typedef enum
{
    CS_LOAD_CAL_TABLE_SUCCESS = 0,
    CS_LOAD_CAL_TABLE_FAIL = 1,

} cs_load_cal_table_status_t;

typedef enum
{
    CS_ROLE_DISABLE   = 0,
    CS_INITIATOR_ROLE = BIT(0),
    CS_REFLECTOR_ROLE = BIT(1),
    CS_INIT_REFL_ROLE = BIT(0) | BIT(1),
} cs_role_t;

typedef enum
{
    CS_PARAM_INITIATOR_ROLE = 0,
    CS_PARAM_REFLECTOR_ROLE = 1,
} cs_param_role_t;

typedef enum
{
    CS_CONFIG_INITIATOR_ROLE = 0,
    CS_CONFIG_REFLECTOR_ROLE = 1,
} cs_config_role_t;

typedef enum
{
    CS_DIST_SUCCESS                                 = 0,
    CS_DIST_ERR_STEPS_NUMS_ZEROS                    = 1,
    CS_DIST_ERR_STEPS_NUM_NOT_ENOUGH                = 2,
    CS_DIST_ERR_ALGO_TYPE_NOT_SUPPORT               = 3,
    CS_DIST_ERR_ALGO_MASK_NOT_SET                   = 4,
    CS_RAS_PARSE_ERROR                              = 5,
    CS_RAS_ERR_NULL_PTR                             = 6,
    CS_RAS_ERR_COUNTER_MISMATCH                     = 7,
    CS_RAS_ERR_LOCAL_PROC_CTRL_NULL                 = 8,
    CS_RAS_ERR_PROC_DATA_NULL_PTR                   = 9,
    CS_RAS_ERR_REMOTE_PROC_DATE_LEN_MISMATCH        = 10,
    CS_RAS_ERR_LOCAL_PROC_DATE_LEN_MISMATCH         = 11,
    CS_RAS_ERR_UNEXPECTED_DONE_FLAG                 = 12,
    CS_PROCEDURE_PARAMETER_ERROR                    = 13,
    CS_DIST_ERR_RANGING_DATA_DECODE_ERROR           = 14,
    CS_DIST_ERR_ALLOC_IQ_MEM_FAILED                 = 15,
    CS_DIST_ERR_VALID_CHM_NUM_NOT_ENOUGH            = 16,
    CS_RANGING_DATA_ERR_ANT_NUM_INVALID             = 17,
    CS_DIST_ERR_INVALID_PARAMETER                   = 18,
    CS_DIST_ERR_OVER_MAX_DISTANCE                   = 19,
} cs_distance_error_code_t;


enum
{
    STEP_MODE_0          = 0,
    STEP_MODE_1          = 1,
    SUBMODE_TYPE_MODE_1  = 1,
    MAINMODE_TYPE_MODE_1 = 1,
    STEP_MODE_2          = 2,
    SUBMODE_TYPE_MODE_2  = 2,
    MAINMODE_TYPE_MODE_2 = 2,
    STEP_MODE_3          = 3,
    SUBMODE_TYPE_MODE_3  = 3,
    MAINMODE_TYPE_MODE_3 = 3,

    SUBMODE_TYPE_MODE_UNUSED = 0xFF,
};

enum
{
    RTT_Type_AA_Only   = 0,
    RTT_Type_32bit_ss  = 1,
    RTT_Type_96bit_ss  = 2,
    RTT_Type_32bit_rs  = 3,
    RTT_Type_64bit_rs  = 4,
    RTT_Type_96bit_rs  = 5,
    RTT_Type_128bit_rs = 6,
};

enum
{
    CS_COMPANION_SIGNAL_SUPPORT      = BIT(0),
    CS_No_FAE_SUPPORT                = BIT(1),
    CS_CSA_3C_SUPPORT                = BIT(2),
    CS_SOUNDING_PCT_ESTIMATE_SUPPORT = BIT(3),
};

typedef enum
{
    CS_T_IP1_10US   = BIT(0),
    CS_T_IP1_20US   = BIT(1),
    CS_T_IP1_30US   = BIT(2),
    CS_T_IP1_40US   = BIT(3),
    CS_T_IP1_50US   = BIT(4),
    CS_T_IP1_60US   = BIT(5),
    CS_T_IP1_80US   = BIT(6),
    CS_T_IP1_145US  = 0,
}CS_T_IP1_BIT_MAP_ENUM;

typedef enum
{
    CS_T_IP2_10US   = BIT(0),
    CS_T_IP2_20US   = BIT(1),
    CS_T_IP2_30US   = BIT(2),
    CS_T_IP2_40US   = BIT(3),
    CS_T_IP2_50US   = BIT(4),
    CS_T_IP2_60US   = BIT(5),
    CS_T_IP2_80US   = BIT(6),
    CS_T_IP2_145US  = 0,
}CS_T_IP2_BIT_MAP_ENUM;

typedef enum
{
    CS_T_FCS_15US   = BIT(0),
    CS_T_FCS_20US   = BIT(1),
    CS_T_FCS_30US   = BIT(2),
    CS_T_FCS_40US   = BIT(3),
    CS_T_FCS_50US   = BIT(4),
    CS_T_FCS_60US   = BIT(5),
    CS_T_FCS_80US   = BIT(6),
    CS_T_FCS_100US  = BIT(7),
    CS_T_FCS_120US  = BIT(8),
    CS_T_FCS_150US  = 0,
}CS_T_FCS_BIT_MAP_ENUM;




typedef enum
{
    CS_T_PM_10US = BIT(0),
    CS_T_PM_20US = BIT(1),
    CS_T_PM_40US = 0,
}CS_T_PM_BIT_MAP_ENUM;


typedef enum
{
    T_SW_0US = 0,
    T_SW_1US = 1,
    T_SW_2US = 2,
    T_SW_4US = 4,
    T_SW_10US = 10,
}CS_T_SW_TIME_VALUE_ENUM;

typedef struct __attribute__((packed))
{
    u8  Num_Config_Supported;                 //range 1-4
    u16 max_consecutive_procedures_supported; // range 0- 0xffff
    u8  Num_Antennas_Supported;               // range 1--4

    u8 Max_Antenna_Paths_Supported;           //range 1--4
    u8 Roles_Supported;                       // bit map,  BIT(0) initiator, BIT(1) reflector
    u8 Mode_Types;                            // bit map, BIT(0) mode3
    u8 RTT_Capability;

    u8  RTT_AA_Only_N;
    u8  RTT_Sounding_N;
    u8  RTT_Random_Payload_N;
    u16 Optional_NADM_Sounding_Capability;
    u16 Optional_NADM_Random_Capability;
    u8  Optional_CS_SYNC_PHYs_Supported;

    u16 Optional_Subfeatures_Supported;
    u16 Optional_T_IP1_Times_Supported;

    u16 Optional_T_IP2_Times_Supported;
    u16 Optional_T_FCS_Times_Supported; //

    u16 Optional_T_PM_Times_Supported;  //bit map, BIT(0): 10us, BIT(1):20us
    u8  T_SW_Time_Supported;            //0x01, 0x02, 0x04, or 0x0A
    u8  Optional_TX_SNR_Capability;
} chn_sound_capabilities_t;

/**
 * @brief   the structure of antenna switch config.
 */
typedef struct{
    u32 ant_default_seq_value;
    u32 ant_ctrl_seq_base_value;

    u8 Num_Antennas_Supported;
    u8 Max_Antenna_Paths_Supported;
}cs_ant_switch_config_t;

/**
 * @brief   for user to set fixed cs req parameters.
 */
typedef struct __attribute__((packed))
{
    u8  enable;
    u8  Subevents_Per_Event;
    u16 subEvtIntvl_625us;
    u16 Event_Interval;
    u16 Max_Procedure_Len;
    u32 offset_min;
    u32 offset_max;
    u32 Subevent_Len;
    u16 Procedure_Interval;
    u8  Preferred_Peer_Ant;
}ll_cs_fixed_cs_req_t;

/**
 * @brief   for user to set fixed cs rsp parameters.
 */
typedef struct __attribute__((packed))
{
    u8  enable;
    u32 offset_min;
    u32 offset_max;
    u16 Event_Interval;
    u8  Subevents_Per_Event;
    u16 subEvtIntvl_625us;
    u32 Subevent_Len;
}ll_cs_fixed_cs_rsp_t;

/**
 * @brief   for user to set fixed cs ind parameters.
 */
typedef struct __attribute__((packed))
{
    u8  enable;
    u32 offset;
    u16 Event_Interval;
    u8  Subevents_Per_Event;
    u16 subEvtIntvl_625us;
    u32 Subevent_Len;
}ll_cs_fixed_cs_ind_t;

/**
 * @brief      for user to initialize channel sounding module and allocate channel sounding configuration buffer.
 * @param[in]  pParamBuf - start address of channel sounding configuration parameters buffer
 * @param[in]  cs_config_num - channel sounding configuration number that application layer may use
 * @return     status, 0x00:  succeed
 *                     other: failed
 */
ble_sts_t blc_ll_initCsConfigParam(u8 *pParamBuf, int cs_config_num);

/**
 * @brief      for user to allocate channel sounding step DRBG information buffer.
 * @param[in]  pStepInfoBuffer - start address of channel sounding step DRBG information buffer
 * @param[in]  step_num_per_subevent - maximum steps number supported of each subevent.
 * @param[in]  cs_config_num - channel sounding configuration number that application layer may use
 * @return     status, 0x00:  succeed
 *                     other: failed
 */
ble_sts_t blc_cs_initStepDRBGInfo(u8 *pStepInfoBuffer, u8 step_num_per_subevent, u8 cs_config_num);

/**
 * @brief       for user to load channel sounding calibration value (from Flash) when MCU power on or wake_up from deepSleep mode
 * @param[in]   flash_addr - flash address for channel sounding calibration value.
 * @return      none
 */
cs_load_cal_table_status_t blc_loadCsCali_table(u32 flash_addr);

/**
 * @brief      for user to initialize channel sounding Phy Layer RX FIFO.
 * @param[in]  pRxBuf - RX FIFO buffer address.
 * @param[in]  fifo_size - RX FIFO size, size must be 4*n
 * @param[in]  fifo_number - RX FIFO number, can only be 4, 8, 16 or 32
 * @return     status, 0x00:  succeed
 *                     other: failed
 */
ble_sts_t blc_ll_initCsPhyRxFifo(u8 *pRxBuf, int fifo_size, int fifo_num);

/**
 * @brief      for user to initialize channel sounding transport layer RX FIFO.
 * @param[in]  pRxBuf - RX FIFO buffer address.
 * @param[in]  fifo_size - RX FIFO size, size must be 4*n
 * @param[in]  fifo_number - RX FIFO number, can only be 4, 8, 16 or 32
 * @return     status, 0x00:  succeed
 *                     other: failed
 */
ble_sts_t blc_ll_initCsTransportRxFifo(u8 *pRxBuf, int fifo_size, int fifo_num);


/**
 * @brief      for user to read local CS capabilities.
 * @param[out] pRetParam - refer to hci command of "LE CS Read Local Supported Capabilities command"
 * @return     status, 0x00:  succeed
 *                     other: failed
 */
ble_sts_t blc_hci_le_cs_readLocalSupportedCap(hci_le_cs_readLocalSupportedCap_retParam_t *pRetParam);

/**
 * @brief      for user to read remote device CS capabilities.
 * @param[in]  handle - ACL handle
 * @return     status, 0x00:  succeed
 *                     other: failed
 */
ble_sts_t blc_hci_le_cs_readRemoteSupportedCap(u16 handle);

/**
 * @brief      for user to  write the cached copy of the CS capabilities that are supported by the remote Controller.
 * @param[in]  pCS_param - refer to HCI_LE_CS_Write_Cached_Remote_Supported_Capabilities command in core spec
 * @param[out] pRetParam - include status and Connection_Handle.
 * @return     status, 0x00:  succeed
 *                     other: failed
 */
ble_sts_t blc_hci_le_cs_writeCachedRemoteSupportedCap(
    hci_le_cs_writeCachedRemoteSupportedCap_cmdParam_t *pCS_param,
    hci_le_cs_writeCachedRemoteSupportedCap_retParam_t *pRetParam);

/**
 * @brief      for user to start or restart the Channel Sounding Security Start procedure.
 * @param[in]  connHandle - ACL connection handle
 * @return     status, 0x00:  succeed
 *                     other: failed
 */
ble_sts_t blc_hci_le_cs_security_enable(u16 connHandle);

/**
 * @brief      for user to  to set default CS settings in the local Controller .
 * @param[in]  pSetting - refer to  HCI_LE_CS_Set_Default_Settings command in core spec
 * @param[out] retParam - include status and Connection_Handle.
 * @return     status, 0x00:  succeed
 *                     other: failed
 */
ble_sts_t blc_hci_le_cs_setDefaultSettings(hci_le_cs_setDefaultSetting_cmdParam_t *pSetting,
                                           hci_le_cs_setDefaultSetting_retParam_t *retParam);

/**
 * @brief      for user to read the per-channel mode0 Frequency Actuation Error table of the remote Controller.
 * @param[in]  connHandle - ACL connection handle
 * @return     status, 0x00:  succeed
 *                     other: failed
 */
ble_sts_t blc_hci_le_cs_readRemoteFAE_table(u16 connHandle);

/**
 * @brief      for user to  to  write a cached copy of the per-channel mode 0 Frequency Actuation
 *             Error table of the remote device in the local Controller.
 * @param[in]  table - Per-channel mode 0 Frequency Actuation Error table of the local Controller
 * @param[out] retParam - include status and Connection_Handle.
 * @return     status, 0x00:  succeed
 *                     other: failed
 */
ble_sts_t blc_hci_le_cs_writeCachedRemoteFAE_table(u16 connHandle, u8 *table, hci_le_cs_writeChchedRemoteFAE_retParam_t *retParam);

/**
 * @brief      for user to create a new CS configuration.
 * @param[in]  pConfig - CS configuration refer to  HCI_LE_CS_Create_Config command
 * @return     status, 0x00:  succeed
 *                     other: failed
 */
ble_sts_t blc_hci_le_cs_createConfig(hci_le_cs_creatConfig_cmdParam_t *pConfig);

/**
 * @brief      for user to set the parameters for the scheduling of one or more CS procedures by the local Controller.
 * @param[in]  pParam - CS procedure parameters refer to HCI_LE_CS_Set_Procedure_Parameters command
 * @param[out] retParam - include status and Connection_Handle.
 * @return     status, 0x00:  succeed
 *                     other: failed
 */
ble_sts_t blc_hci_le_cs_setProcedureParam(hci_le_cs_setProcedureParame_cmdParam_t *pParam,
                                          hci_le_cs_setProcedureParam_retParam_t  *ret);

/**
 * @brief      for user to enable or disable the scheduling of CS procedures.
 * @param[in]  pCmd - refer to HCI_LE_CS_Procedure_Enable command
 * @return     status, 0x00:  succeed
 *                     other: failed
 */
ble_sts_t blc_hci_le_cs_procedureEnable(hci_le_cs_enableProcedure_cmdParam_t *pCmd);

/**
 * @brief      for user to remove a CS configuration.
 * @param[in]  connHandle - ACL connection handle
 * @param[in]  config_ID - Config ID
 * @return     status, 0x00:  succeed
 *                     other: failed
 */
ble_sts_t blc_hci_le_cs_removeConfig(u16 connHandle, u8 config_ID);

/**
 * @brief      for user to update the channel classification based on its local information.
 * @param[in]  chn - channel bit fields.
 * @return     status, 0x00:  succeed
 *                     other: failed
 */
ble_sts_t blc_hci_le_cs_setChannelClassification(u8 *chn);

/**
 * @brief      Get CS security exchange status.
 * @param[in]  connHandle - ACL connection handle
 * @return     status: 0x01 finished 0x00 unfinished
 */
int blc_ll_getCsSecExchStatus(u16 connHandle);

/**
 * @brief      CS FAE table exchange control.
 * @param[in]  connHandle - ACL connection handle
 * @return     status: 0x00 needn't exchange FAE table
 *                     0x01 wait peer device send CS FAE exchange
 *                     0x02 local device send CS FAE exchange
 */
int blc_ll_CsFaeExchCtrl(u16 connHandle);

/**
 * @brief      Reset channel sounding relative control block by acl connection handle.
 * @param[in]  connHandle - ACL connection handle
 * @return     status
 */
int blc_cs_resetByHandle(u16 connHandle);


/**
 * @brief      For initializing the CS HCI RX FIFO.
 * @param[in]  rxFifo - For buffering HCI CS subevent/continue event.
 * @param[in]  fifo_size - rx FIFO size.
 * @param[in]  fifo_num - rx FIFO number can only be 2 4, 8, 16 or 32.
 * @return     status, 0x00:  succeed
 *                     other: failed
 */
ble_sts_t blc_cs_initCsHciRxFifo(u8 *rxFifo, u16 fifo_size, u8 fifo_num);

/**
 * @brief      For initializing the CS PCT data buffer.
 * @param[in]  DataBuf - For buffering CS PCT data.
 * @param[in]  buff_size - DataBuf size.
 * @param[in]  ant_path - ant_path.
 * @return     status, 0x00:  succeed
 *                     other: failed
 */
ble_sts_t blc_ll_initCsPctDataBuff(u8 *DataBuf, int buff_size, int ant_path);

/**
 * @brief      for user to initialize channel sounding module of initiator role.
 * @param[in]  cap - Pointer of CS local capability.
 * @return     status, 0x00:  succeed
 *                     other: failed
 */
ble_sts_t blc_ll_initCsInitiatorModule(void);

/**
 * @brief      for user to initialize channel sounding module of reflect role.
 * @param[in]  cap - Pointer of CS local capability.
 * @return     status, 0x00:  succeed
 *                     other: failed
 */
ble_sts_t blc_ll_initCsReflectorModule(void);

/**
 * @brief      this function is used to initialize channel selection algorithm #3c feature in channel sounding module.
 * @param      none
 * @return     none
 */
void blc_ll_initChannelSelectionAlgorithm_3c_feature(void);

/**
 * @brief      Set Channel sounding TX power level.
 * @param[in]  power_level - power level for Channel sounding(rf_power_level_e).
 * @param[out] None.
 * @return     None
 */
void blc_cs_set_tx_power_level(u8 power_level);

/**
 * @brief      disable input and output, set pulldown of PD4 - pd7, only for TL721X
 * @param[in]  None
 * @return     None
 */
void blc_cs_disableGpioPinsFromD4ToD7(void);

/**
 * Check if the CS (Channel Sounding) subevent is busy.
 *
 * This function evaluates whether the CS subevent is currently occupied by checking 
 * the `isCSsubeventBusy` flag in the global CS management structure (`gCsMng`).
 *
 * @return bool Returns TRUE if the CS subevent is busy, otherwise FALSE.
 */
/**
 * @brief      Check if the CS (Channel Sounding) subevent is busy.
 * @param[in]  None
 * @return     TRUE if the CS subevent is busy, otherwise FALSE.
 *
 */
bool blc_cs_isSubeventBusy(void);

/**
 * @brief      Init cs antenna switch configuration
 * @param[in]  params - cs antenna switch configuration parameter
 * @return     pointer to cs lib version string.
 *
 */
void blc_cs_antenna_switch_config_init(cs_ant_switch_config_t *params);

/**
 * @brief      Set the Channel Sounding capability parameters.
 * @param[in]  t_ip1 - Optional T_IP1 times supported.
 * @param[in]  t_ip2 - Optional T_IP2 times supported.
 * @param[in]  t_fcs - Optional T_FCS times supported.
 * @param[in]  t_pm  - Optional T_PM times supported.
 * @param[in]  t_sw  - T_SW time supported.
 * @return     None
 */
void blc_setCsCapabilityParam(CS_T_IP1_BIT_MAP_ENUM t_ip1, CS_T_IP2_BIT_MAP_ENUM t_ip2,
                              CS_T_FCS_BIT_MAP_ENUM t_fcs, CS_T_PM_BIT_MAP_ENUM t_pm,
                              CS_T_SW_TIME_VALUE_ENUM t_sw );


#endif /* STACK_BLE_CONTROLLER_LL_CHN_SOUND_CHANNEL_SOUND_H_ */
