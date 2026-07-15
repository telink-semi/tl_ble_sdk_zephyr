/********************************************************************************************************
 * @file    app.c
 *
 * @brief   This is the source file for BLE SDK
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
#include "../../feature_app_parse_char.h"
#include "app.h"
#include "app_buffer.h"
#include "app_ui.h"
#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"

#if (FEATURE_TEST_MODE == TEST_AUTO)
#if (AUTO_TEST_MODE == AUTO_TEST_EXT_ADV_DEMO)

#define APP_EXT_ADV_DATA_MAX_CHUNK  100
#define APP_EXT_ADV_SETS_NUMBER     1    //user set value
#define APP_EXT_ADV_DATA_LENGTH     1024 //2048//1664//1024   //user set value
#define APP_EXT_SCANRSP_DATA_LENGTH 1024 //2048//1664//1024   //user set value

_attribute_iram_bss_ u8         app_extAdvSetParam_buf[ADV_SET_PARAM_LENGTH * APP_EXT_ADV_SETS_NUMBER];
_attribute_iram_noinit_data_ u8 app_extAdvData_buf[APP_EXT_ADV_DATA_LENGTH * APP_EXT_ADV_SETS_NUMBER];
_attribute_iram_noinit_data_ u8 app_extScanRspData_buf[APP_EXT_SCANRSP_DATA_LENGTH * APP_EXT_ADV_SETS_NUMBER];

_attribute_data_retention_ u8  adv_data_temp_buffer[APP_EXT_ADV_SETS_NUMBER][APP_EXT_ADV_DATA_LENGTH];
_attribute_data_retention_ u8  scan_rsp_temp_buffer[APP_EXT_ADV_SETS_NUMBER][APP_EXT_SCANRSP_DATA_LENGTH];
_attribute_data_retention_ u16 adv_data_length_temp_buffer[APP_EXT_ADV_SETS_NUMBER];
_attribute_data_retention_ u16 scan_rsp_length_temp_buffer[APP_EXT_ADV_SETS_NUMBER];

int central_smp_pending      = 0; // SMP: security & encryption;
int central_connected_led_on = 0;

static void app_le_enhanced_connection_complete_event_handle(u8 *p)
{
    hci_le_enhancedConnCompleteEvt_t *pConnEvt = (hci_le_enhancedConnCompleteEvt_t *)p;

    app_parse_printf("Enhanced connection complete %02X:%02X:%02X:%02X:%02X:%02X addr_type:%02X connHandle:%d\r\n", pConnEvt->PeerAddr[0], pConnEvt->PeerAddr[1], pConnEvt->PeerAddr[2],
                     pConnEvt->PeerAddr[3], pConnEvt->PeerAddr[4], pConnEvt->PeerAddr[5], pConnEvt->PeerAddrType, pConnEvt->connHandle);
}

static void app_disconnect_event_handle(u8 *p)
{
    hci_disconnectionCompleteEvt_t *pDisConn = (hci_disconnectionCompleteEvt_t *)p;

    app_parse_printf("Disconnect connHandle:%d, reason:%d\r\n", pDisConn->connHandle, pDisConn->reason);
}

//////////////////////////////////////////////////////////
// event call back
//////////////////////////////////////////////////////////
/**
 * @brief      BLE controller event handler call-back.
 * @param[in]  h       event type
 * @param[in]  p       Pointer point to event parameter buffer.
 * @param[in]  n       the length of event parameter.
 * @return
 */
int app_controller_event_callback(u32 h, u8 *p, int n)
{
    (void)n;
    if (h & HCI_FLAG_EVENT_BT_STD) {
        u8 evtCode = h & 0xff;

        if (evtCode == HCI_EVT_DISCONNECTION_COMPLETE) {
            app_disconnect_event_handle(p);
        } else if (evtCode == HCI_EVT_LE_META) {
            u8 subEvt_code = p[0];

            if (subEvt_code == HCI_SUB_EVT_LE_ENHANCED_CONNECTION_COMPLETE) {
                app_le_enhanced_connection_complete_event_handle(p);
            }
        }
    }


    return 0;
}

/**
 * @brief      BLE host event handler call-back.
 * @param[in]  h       event type
 * @param[in]  para    Pointer point to event parameter buffer.
 * @param[in]  n       the length of event parameter.
 * @return
 */
int app_host_event_callback(u32 h, u8 *para, int n)
{
    (void)n;
    u8 event = h & 0xFF;

    switch (event) {
    case GAP_EVT_SMP_PAIRING_BEGIN:
    {
    } break;

    case GAP_EVT_SMP_PAIRING_SUCCESS:
    {
    } break;

    case GAP_EVT_SMP_PAIRING_FAIL:
    {
#if (ACL_CENTRAL_SMP_ENABLE)
        gap_smp_pairingFailEvt_t *p = (gap_smp_pairingFailEvt_t *)para;

        if (dev_char_get_conn_role_by_connhandle(p->connHandle) == ACL_ROLE_CENTRAL) {
            if (central_smp_pending == p->connHandle) {
                central_smp_pending = 0;
            }
        }
#endif
    } break;

    case GAP_EVT_SMP_CONN_ENCRYPTION_DONE:
    {
    } break;

    case GAP_EVT_SMP_SECURITY_PROCESS_DONE:
    {
        gap_smp_connEncDoneEvt_t *p = (gap_smp_connEncDoneEvt_t *)para;

#if (ACL_CENTRAL_SMP_ENABLE)
        if (dev_char_get_conn_role_by_connhandle(p->connHandle) == ACL_ROLE_CENTRAL) {
            if (central_smp_pending == p->connHandle) {
                central_smp_pending = 0;
            }
        }
#endif

#if (ACL_CENTRAL_SIMPLE_SDP_ENABLE)                       //SMP finish
        if (central_sdp_pending == p->connHandle) {       //SDP is pending
            app_register_service(&app_service_discovery); //start SDP now
        }
#endif
    } break;

    case GAP_EVT_SMP_TK_DISPLAY:
    {
    } break;

    case GAP_EVT_SMP_TK_REQUEST_PASSKEY:
    {
    } break;

    case GAP_EVT_SMP_TK_REQUEST_OOB:
    {
    } break;

    case GAP_EVT_SMP_TK_NUMERIC_COMPARE:
    {
    } break;

    case GAP_EVT_ATT_EXCHANGE_MTU:
    {
    } break;

    case GAP_EVT_GATT_HANDLE_VALUE_CONFIRM:
    {
    } break;

    default:
        break;
    }

    return 0;
}

#define HID_HANDLE_CONSUME_REPORT  25
#define HID_HANDLE_KEYBOARD_REPORT 29
#define AUDIO_HANDLE_MIC           52
#define OTA_HANDLE_DATA            48

/**
 * @brief      BLE GATT data handler call-back.
 * @param[in]  connHandle     connection handle.
 * @param[in]  pkt             Pointer point to data packet buffer.
 * @return
 */
int app_gatt_data_handler(u16 connHandle, u8 *pkt)
{
    if (dev_char_get_conn_role_by_connhandle(connHandle) == ACL_ROLE_CENTRAL) //GATT data for Central
    {
#if (ACL_CENTRAL_SIMPLE_SDP_ENABLE)
        if (central_sdp_pending == connHandle) { //ATT service discovery is ongoing on this conn_handle
            //when service discovery function is running, all the ATT data from peripheral
            //will be processed by it,  user can only send your own att cmd after  service discovery is over
            host_att_client_handler(connHandle, pkt); //handle this ATT data by service discovery process
        }
#endif

        rf_packet_att_t *pAtt = (rf_packet_att_t *)pkt;

        //so any ATT data before service discovery will be dropped
        dev_char_info_t *dev_info = dev_char_info_search_by_connhandle(connHandle);
        if (dev_info) {
            //-------    user process ------------------------------------------------
            u16 attHandle = pAtt->handle;

            if (pAtt->opcode == ATT_OP_HANDLE_VALUE_NOTI) {
                //---------------    consumer key --------------------------
#if (ACL_CENTRAL_SIMPLE_SDP_ENABLE)
                if (attHandle == dev_info->char_handle[3]) // Consume Report In (Media Key)
#else
                if (attHandle == HID_HANDLE_CONSUME_REPORT) //Demo device(825x_ble_sample) Consume Report AttHandle value is 25
#endif
                {
                    att_keyboard_media(connHandle, pAtt->dat);
                }
                //---------------    keyboard key --------------------------
#if (ACL_CENTRAL_SIMPLE_SDP_ENABLE)
                else if (attHandle == dev_info->char_handle[4]) // Key Report In
#else
                else if (attHandle == HID_HANDLE_KEYBOARD_REPORT) // Demo device(825x_ble_sample) Key Report AttHandle value is 29
#endif
                {
                    att_keyboard(connHandle, pAtt->dat);
                }
#if (ACL_CENTRAL_SIMPLE_SDP_ENABLE)
                else if (attHandle == dev_info->char_handle[0]) // AUDIO Notify
#else
                else if (attHandle == AUDIO_HANDLE_MIC) // Demo device(825x_ble_remote) Key Report AttHandle value is 52
#endif
                {

                } else {
                }
            } else if (pAtt->opcode == ATT_OP_HANDLE_VALUE_IND) {
            }
        }

        /* The Central does not support GATT Server by default */
        if (!(pAtt->opcode & 0x01)) {
            switch (pAtt->opcode) {
            case ATT_OP_FIND_INFO_REQ:
            case ATT_OP_FIND_BY_TYPE_VALUE_REQ:
            case ATT_OP_READ_BY_TYPE_REQ:
            case ATT_OP_READ_BY_GROUP_TYPE_REQ:
                blc_gatt_pushErrResponse(connHandle, pAtt->opcode, pAtt->handle, ATT_ERR_ATTR_NOT_FOUND);
                break;
            case ATT_OP_READ_REQ:
            case ATT_OP_READ_BLOB_REQ:
            case ATT_OP_READ_MULTI_REQ:
            case ATT_OP_WRITE_REQ:
            case ATT_OP_PREPARE_WRITE_REQ:
                blc_gatt_pushErrResponse(connHandle, pAtt->opcode, pAtt->handle, ATT_ERR_INVALID_HANDLE);
                break;
            case ATT_OP_EXECUTE_WRITE_REQ:
            case ATT_OP_HANDLE_VALUE_CFM:
            case ATT_OP_WRITE_CMD:
            case ATT_OP_SIGNED_WRITE_CMD:
                //ignore
                break;
            default: //no action
                break;
            }
        }
    } else { //GATT data for Peripheral
    }


    return 0;
}


#if (BATT_CHECK_ENABLE) //battery check must do before OTA relative operation

_attribute_data_retention_ u32 lowBattDet_tick = 0;

/**
 * @brief        this function is used to process battery power.
 *                 The low voltage protection threshold 2.0V is an example and reference value. Customers should
 *                 evaluate and modify these thresholds according to the actual situation. If users have unreasonable designs
 *                 in the hardware circuit, which leads to a decrease in the stability of the power supply network, the
 *                 safety thresholds must be increased as appropriate.
 * @param[in]    none
 * @return      none
 */
_attribute_ram_code_ void user_battery_power_check(u16 alarm_vol_mv)
{
    /*For battery-powered products, as the battery power will gradually drop, when the voltage is low to a certain
      value, it will cause many problems.
        a) When the voltage is lower than operating voltage range of chip, chip can no longer guarantee stable operation.
        b) When the battery voltage is low, due to the unstable power supply, the write and erase operations
            of Flash may have the risk of error, causing the program firmware and user data to be modified abnormally,
            and eventually causing the product to fail. */
    u8 battery_check_returnValue = 0;
    if (analog_read(USED_DEEP_ANA_REG) & LOW_BATT_FLG) {
        battery_check_returnValue = app_battery_power_check(alarm_vol_mv + 200);
    } else {
        battery_check_returnValue = app_battery_power_check(alarm_vol_mv);
    }
    if (battery_check_returnValue) {
        analog_write_reg8(USED_DEEP_ANA_REG, analog_read_reg8(USED_DEEP_ANA_REG) & (~LOW_BATT_FLG)); //clr
    } else {
#if (UI_LED_ENABLE) //led indicate
        for (int k = 0; k < 3; k++) {
            gpio_write(GPIO_LED_BLUE, LED_ON_LEVEL);
            sleep_us(200000);
            gpio_write(GPIO_LED_BLUE, !LED_ON_LEVEL);
            sleep_us(200000);
        }
#endif
        analog_write_reg8(USED_DEEP_ANA_REG, analog_read_reg8(USED_DEEP_ANA_REG) | LOW_BATT_FLG); //mark

#if (UI_KEYBOARD_ENABLE)
        u32 pin[] = KB_DRIVE_PINS;
        for (unsigned int i = 0; i < (sizeof(pin) / sizeof(*pin)); i++) {
            cpu_set_gpio_wakeup(pin[i], 1, 1); //drive pin pad high wakeup deepsleep
        }

        cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0); //deepsleep
#endif
    }
}

#endif


#if (APP_FLASH_PROTECTION_ENABLE)

/**
 * @brief      flash protection operation, including all locking & unlocking for application
 *                handle all flash write & erase action for this demo code. use should add more more if they have more flash operation.
 * @param[in]  flash_op_evt - flash operation event, including application layer action and stack layer action event(OTA write & erase)
 *                attention 1: if you have more flash write or erase action, you should should add more type and process them
 *                attention 2: for "end" event, no need to pay attention on op_addr_begin & op_addr_end, we set them to 0 for
 *                                stack event, such as stack OTA write new firmware end event
 * @param[in]  op_addr_begin - operating flash address range begin value
 * @param[in]  op_addr_end - operating flash address range end value
 *                attention that, we use: [op_addr_begin, op_addr_end)
 *                e.g. if we write flash sector from 0x10000 to 0x20000, actual operating flash address is 0x10000 ~ 0x1FFFF
 *                        but we use [0x10000, 0x20000):  op_addr_begin = 0x10000, op_addr_end = 0x20000
 * @return     none
 */
_attribute_data_retention_ u16 flash_lockBlock_cmd;

void app_flash_protection_operation(u8 flash_op_evt, u32 op_addr_begin, u32 op_addr_end)
{
    (void)op_addr_begin;
    (void)op_addr_end;
    if (flash_op_evt == FLASH_OP_EVT_APP_INITIALIZATION) {
        /* ignore "op addr_begin" and "op addr_end" for initialization event
         * must call "flash protection_init" first, will choose correct flash protection relative API according to current internal flash type in MCU */
        flash_protection_init();

        /* just sample code here, protect all flash area for old firmware and OTA new firmware.
         * user can change this design if have other consideration */
        u32 app_lockBlock = FLASH_LOCK_FW_LOW_512K; //just demo value, user can change this value according to application

        flash_lockBlock_cmd = flash_change_app_lock_block_to_flash_lock_block(app_lockBlock);

        if (blc_flashProt.init_err) {
            tlkapi_printf(APP_FLASH_PROT_LOG_EN, "[FLASH][PROT] flash protection initialization error!!!\n");
        }

        tlkapi_printf(APP_FLASH_PROT_LOG_EN, "[FLASH][PROT] initialization, lock flash\n");
        flash_lock(flash_lockBlock_cmd);
    }
    /* add more flash protection operation for your application if needed */
}

#endif

static void read_bdaddr(char *argv[], int argc, void *user_data)
{
    u8 addr[6];

    (void)user_data;
    (void)argv;
    (void)argc;

    if (blc_ll_readBDAddr(addr) == BLE_SUCCESS) {
        app_parse_printf("read_bdaddr: %02X:%02X:%02X:%02X:%02X:%02X\r\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
    } else {
        app_parse_printf("read_bdaddr: failed\r\n");
    }
}

static bool app_parse_bdaddr(const char *str, u8 *addr);

static void ext_adv_data_param_set(char *argv[], int argc, void *user_data)
{
    ble_sts_t status;
    u8        adv_handle, chn_map, own_addr_type, peer_addr_type, fp, tx_power, pri_phy, sec_phy, sec_adv_max_skip, adv_sid, scan_req_notify_en;
    u16       props;
    u32       interval_min, interval_max;
    u8        addr[6];
    int       i = 0;

    (void)user_data;

    if (argc < 15) {
        app_parse_printf("ext_adv_p_set <adv_handle> <props> <int_min> <int_max> <chn_map> <own_addr_type>"
                         "<peer_addr_type> <addr> <fp> <tx_pow> <pri_phy> <sec_adv_max_skip> <sec_phy> <adv_sid> <scan_req_notify_en>\r\n");
        return;
    }

    adv_handle     = app_parse_str2n(argv[i++]);
    props          = app_parse_str2n(argv[i++]);
    interval_min   = app_parse_str2n(argv[i++]);
    interval_max   = app_parse_str2n(argv[i++]);
    chn_map        = app_parse_str2n(argv[i++]);
    own_addr_type  = app_parse_str2n(argv[i++]);
    peer_addr_type = app_parse_str2n(argv[i++]);
    app_parse_bdaddr(argv[i++], addr);
    fp                 = app_parse_str2n(argv[i++]);
    tx_power           = app_parse_str2n(argv[i++]);
    pri_phy            = app_parse_str2n(argv[i++]);
    sec_adv_max_skip   = app_parse_str2n(argv[i++]);
    sec_phy            = app_parse_str2n(argv[i++]);
    adv_sid            = app_parse_str2n(argv[i++]);
    scan_req_notify_en = app_parse_str2n(argv[i++]);

    status = blc_ll_setExtAdvParam(adv_handle, props, interval_min, interval_max, chn_map, own_addr_type, peer_addr_type, addr, fp, tx_power, pri_phy, sec_adv_max_skip, sec_phy, adv_sid,
                                   scan_req_notify_en);

    app_parse_printf("ext_adv_p_set status:%d\r\n", status);
}

static void ext_adv_data_append(char *argv[], int argc, void *user_data)
{
    uint16_t adv_id, offset, adv_data_len = 0;

    (void)user_data;

    if (argc < 3) {
        app_parse_printf("adv_data_set <adv_id> <offset> <data>\r\n");
        return;
    }

    adv_id = app_parse_str2n(argv[0]);
    offset = app_parse_str2n(argv[1]);

    if (adv_id > APP_EXT_ADV_SETS_NUMBER) {
        app_parse_printf("adv_data_set: Invalid adv id\r\n");
        return;
    }

    if (strlen(argv[2]) % 2) {
        app_parse_printf("adv_data_set: Invalid data\r\n");
        return;
    }

    if ((strlen(argv[2]) % 2) + offset > APP_EXT_ADV_DATA_LENGTH) {
        app_parse_printf("adv_data size:%d exceeds max adv data size:%d\r\n", (strlen(argv[1]) % 2) + offset, APP_EXT_ADV_DATA_LENGTH);
        return;
    }

    adv_data_len                        = app_parse_str2hex(argv[2], &adv_data_temp_buffer[adv_id][offset], APP_EXT_ADV_DATA_LENGTH - offset);
    adv_data_length_temp_buffer[adv_id] = offset + adv_data_len;

    app_parse_printf("adv_data_append length:%d\r\n", adv_data_len);
}

static void ext_scan_rsp_data_append(char *argv[], int argc, void *user_data)
{
    uint16_t adv_id, offset, adv_data_len = 0;

    (void)user_data;

    if (argc < 3) {
        app_parse_printf("ext_scn_r_d_app <adv_id> <offset> <data>\r\n");
        return;
    }

    adv_id = app_parse_str2n(argv[0]);
    offset = app_parse_str2n(argv[1]);

    if (adv_id > APP_EXT_ADV_SETS_NUMBER) {
        app_parse_printf("ext_scn_r_d_app: Invalid adv id\r\n");
        return;
    }

    if (strlen(argv[2]) % 2) {
        app_parse_printf("ext_scn_r_d_app: Invalid data\r\n");
        return;
    }

    if ((strlen(argv[2]) % 2) + offset > APP_EXT_SCANRSP_DATA_LENGTH) {
        app_parse_printf("ext_scn_r_d_app size:%d exceeds max adv data size:%d\r\n", (strlen(argv[1]) % 2) + offset, APP_EXT_SCANRSP_DATA_LENGTH);
        return;
    }

    adv_data_len                        = app_parse_str2hex(argv[2], &scan_rsp_temp_buffer[adv_id][offset], APP_EXT_SCANRSP_DATA_LENGTH - offset);
    scan_rsp_length_temp_buffer[adv_id] = offset + adv_data_len;

    app_parse_printf("ext_scn_r_d_app length:%d\r\n", adv_data_len);
}

static void ext_adv_data_set(char *argv[], int argc, void *user_data)
{
    uint16_t  adv_id;
    ble_sts_t status;

    (void)user_data;

    if (argc < 1) {
        app_parse_printf("Missing adv_id\r\n");
        return;
    }

    adv_id = app_parse_str2n(argv[0]);
    if (adv_id >= APP_EXT_ADV_SETS_NUMBER) {
        app_parse_printf("adv_data_set: Invalid adv id\r\n");
        return;
    }

    status = blc_ll_setExtAdvData(adv_id, adv_data_length_temp_buffer[adv_id], adv_data_temp_buffer[adv_id]);
    app_parse_printf("adv_data_set status:%d\r\n", status);
}

static void ext_scan_rsp_data_set(char *argv[], int argc, void *user_data)
{
    uint16_t  adv_id;
    ble_sts_t status;

    (void)user_data;

    if (argc < 1) {
        app_parse_printf("Missing adv_id\r\n");
        return;
    }

    adv_id = app_parse_str2n(argv[0]);
    if (adv_id >= APP_EXT_ADV_SETS_NUMBER) {
        app_parse_printf("ext_scan_rsp_data_set: Invalid adv id\r\n");
        return;
    }

    status = blc_ll_setExtScanRspData(adv_id, scan_rsp_length_temp_buffer[adv_id], scan_rsp_temp_buffer[adv_id]);
    app_parse_printf("ext_scan_rsp_data_set status:%d\r\n", status);
}

static void ext_adv_data_seg_set(char *argv[], int argc, void *user_data)
{
    uint16_t  adv_id, written = 0;
    ble_sts_t status = BLE_SUCCESS;
    uint16_t  length;
    uint8_t  *data;

    (void)user_data;

    if (argc < 1) {
        app_parse_printf("Missing adv_id\r\n");
        return;
    }

    adv_id = app_parse_str2n(argv[0]);
    if (adv_id >= APP_EXT_ADV_SETS_NUMBER) {
        app_parse_printf("adv_data_set: Invalid adv id\r\n");
        return;
    }

    length = adv_data_length_temp_buffer[adv_id];
    data   = adv_data_temp_buffer[adv_id];

    while ((length - written)) {
        data_oper_t oper = DATA_OPER_INTER;
        uint16_t    cur_length;

        if (!written) {
            if ((length - written) <= APP_EXT_ADV_DATA_MAX_CHUNK) {
                oper = DATA_OPER_COMPLETE;
            } else {
                oper = DATA_OPER_FIRST;
            }
        } else if ((length - written) <= APP_EXT_ADV_DATA_MAX_CHUNK) {
            oper = DATA_OPER_LAST;
        }

        cur_length = (length - written) <= APP_EXT_ADV_DATA_MAX_CHUNK ? (length - written) : APP_EXT_ADV_DATA_MAX_CHUNK;
        app_parse_printf("aa %d length:%d\r\n", oper, cur_length);
        status = blt_ll_setSegmentExtendedAdvData(adv_id, oper, DATA_FRAGMENT_ALLOWED, cur_length, &data[written]);
        if (status != BLE_SUCCESS) {
            break;
        }
        written += cur_length;
    }

    app_parse_printf("adv_data_set status:%d\r\n", status);
}

static void ext_adv_enable(char *argv[], int argc, void *user_data)
{
    adv_en_t  en;
    ble_sts_t status;
    u8        adv_handle, max_adv_events;
    u16       duration;

    (void)user_data;

    if (argc < 4) {
        goto help;
    }

    if (!strcasecmp(argv[0], "on")) {
        en = BLC_ADV_ENABLE;
    } else if (!strcasecmp(argv[0], "off")) {
        en = BLC_ADV_DISABLE;
    } else {
        goto help;
    }

    adv_handle     = app_parse_str2n(argv[1]);
    duration       = app_parse_str2n(argv[2]);
    max_adv_events = app_parse_str2n(argv[3]);

    status = blc_ll_setExtAdvEnable(en, adv_handle, duration, max_adv_events);
    app_parse_printf("ext_adv_en status:0x%02X\r\n", status);
    return;

help:
    app_parse_printf("ext_adv_en <on|off> <adv_handle> <duration> <max_adv_events>\r\n");
}

static bool app_parse_bdaddr(const char *str, u8 *addr)
{
    u8 pos = 0;

    if (strlen(str) != 17) {
        return false;
    }

    for (u8 i = 0; i < 6; i++) {
        u8 temp[3] = {str[pos], str[pos + 1], 0};

        app_parse_str2hex((char *)temp, &addr[i], 1);
        pos += 2;
        if (i < 5 && str[pos++] != ':') {
            return false;
        }
    }

    return true;
}

static void disc(char *argv[], int argc, void *user_data)
{
    ble_sts_t status;
    u16       connHandle;

    (void)user_data;

    if (argc < 1) {
        app_parse_printf("disc <connHandle>\r\n");
        return;
    }

    connHandle = app_parse_str2n(argv[0]);

    status = blc_ll_disconnect(connHandle, HCI_ERR_REMOTE_USER_TERM_CONN);
    app_parse_printf("Disconnect %d status:%02X\r\n", connHandle, status);
}

static void help_fun(char *argv[], int argc, void *user_data);

static parse_fun_list_t app_funcs[] = {
    {"help", help_fun, NULL},
    {"ext_adv_p_set", ext_adv_data_param_set, NULL},
    {"ext_adv_d_app", ext_adv_data_append, NULL},
    {"ext_adv_d_set", ext_adv_data_set, NULL},
    {"ext_adv_seg_set", ext_adv_data_seg_set, NULL},
    {"ext_adv_en", ext_adv_enable, NULL},
    {"ext_scn_r_d_set", ext_scan_rsp_data_set, NULL},
    {"ext_scn_r_d_app", ext_scan_rsp_data_append, NULL},
    {"disc", disc, NULL},
    {"read_bdaddr", read_bdaddr, NULL},
    {"help", help_fun, NULL},
};

static void help_fun(char *argv[], int argc, void *user_data)
{
    app_parse_printf("help:\r\n");

    foreach_arr(i, app_funcs)
    {
        app_parse_printf("\t%s\r\n", app_funcs[i].fun_name);
    }
}

///////////////////////////////////////////

/**
 * @brief        user initialization when MCU power on or wake_up from deepSleep mode
 * @param[in]    none
 * @return      none
 */
_attribute_no_inline_ void user_init_normal(void)
{
    //////////////////////////// basic hardware Initialization  Begin //////////////////////////////////
    /* random number generator must be initiated here( in the beginning of user_init_normal).
     * When deepSleep retention wakeUp, no need initialize again */
    random_generator_init();

#if (TLKAPI_DEBUG_ENABLE)
    tlkapi_debug_init();
    blc_debug_enableStackLog(STK_LOG_NONE);
#endif

    blc_readFlashSize_autoConfigCustomFlashSector();

    /* attention that this function must be called after "blc readFlashSize_autoConfigCustomFlashSector" !!!*/
    blc_app_loadCustomizedParameters_normal();
    //////////////////////////// basic hardware Initialization  End /////////////////////////////////


    //////////////////////////// BLE stack Initialization  Begin //////////////////////////////////

    u8 mac_public[6];
    u8 mac_random_static[6];
    /* Note: If change IC type, need to confirm the FLASH_SIZE_CONFIG */
    blc_initMacAddress(flash_sector_mac_address, mac_public, mac_random_static);


    //////////// LinkLayer Initialization  Begin /////////////////////////
    blc_ll_initBasicMCU();

    blc_ll_initStandby_module(mac_public);

    blc_ll_initExtendedScanning_module();
    blc_ll_initExtendedInitiating_module();

    blc_ll_initAclConnection_module();
    blc_ll_initAclCentralRole_module();
    blc_ll_initAclPeriphrRole_module();
    blc_ll_initLegacyAdvertising_module();

    blc_ll_setMaxConnectionNumber(ACL_CENTRAL_MAX_NUM, ACL_PERIPHR_MAX_NUM);

    blc_ll_setAclConnMaxOctetsNumber(ACL_CONN_MAX_RX_OCTETS, ACL_CENTRAL_MAX_TX_OCTETS, ACL_PERIPHR_MAX_TX_OCTETS);

    /* all ACL connection share same RX FIFO */
    blc_ll_initAclConnRxFifo(app_acl_rx_fifo, ACL_RX_FIFO_SIZE, ACL_RX_FIFO_NUM);
    /* ACL Central TX FIFO */
    blc_ll_initAclCentralTxFifo(app_acl_cen_tx_fifo, ACL_CENTRAL_TX_FIFO_SIZE, ACL_CENTRAL_TX_FIFO_NUM, ACL_CENTRAL_MAX_NUM);
    /* ACL Peripheral TX FIFO */
    blc_ll_initAclPeriphrTxFifo(app_acl_per_tx_fifo, ACL_PERIPHR_TX_FIFO_SIZE, ACL_PERIPHR_TX_FIFO_NUM, ACL_PERIPHR_MAX_NUM);

    blc_ll_setAclCentralBaseConnectionInterval(CONN_INTERVAL_100MS); //CONN_INTERVAL_31P25MS);

    //////////// LinkLayer Initialization  End /////////////////////////


    //////////// HCI Initialization  Begin /////////////////////////
    blc_hci_registerControllerDataHandler(blc_l2cap_pktHandler_5_3);

    blc_hci_registerControllerEventHandler(app_controller_event_callback); //controller hci event to host all processed in this func

    //bluetooth event
    blc_hci_setEventMask_cmd(HCI_EVT_MASK_DISCONNECTION_COMPLETE);

    //bluetooth low energy(LE) event
    blc_hci_le_setEventMask_cmd(HCI_LE_EVT_MASK_CONNECTION_COMPLETE | HCI_LE_EVT_MASK_ADVERTISING_REPORT | HCI_LE_EVT_MASK_ENHANCED_CONNECTION_COMPLETE | HCI_LE_EVT_MASK_CONNECTION_UPDATE_COMPLETE |
                                HCI_LE_EVT_MASK_EXTENDED_ADVERTISING_REPORT);

    blc_hci_le_setEventMask_2_cmd(HCI_LE_EVT_MASK_2_PERIODIC_ADVERTISING_SUBEVENT_DATA_REQUEST | HCI_LE_EVT_MASK_2_PERIODIC_ADVERTISING_RESPONSE_REPORT);

    u8 error_code = blc_contr_checkControllerInitialization();
    if (error_code != INIT_SUCCESS) {
        /* It's recommended that user set some UI alarm to know the exact error, e.g. LED shine, print log */
        write_log32(0x88880000 | error_code);
#if (TLKAPI_DEBUG_ENABLE)
        tlkapi_send_string_data(APP_LOG_EN, "[APP][INI] Controller INIT ERROR", &error_code, 1);
        while (1) {
            tlkapi_debug_handler();
        }
#else
        while (1);
#endif
    }
    //////////// HCI Initialization  End /////////////////////////


    //////////// Host Initialization  Begin /////////////////////////
    /* Host Initialization */
    /* GAP initialization must be done before any other host feature initialization !!! */
    blc_gap_init();

    /* L2CAP data buffer Initialization */
    blc_l2cap_initAclCentralBuffer(app_cen_l2cap_rx_buf, CENTRAL_L2CAP_BUFF_SIZE, app_cen_l2cap_tx_buf, CENTRAL_L2CAP_BUFF_SIZE);
    blc_l2cap_initAclPeripheralBuffer(app_per_l2cap_rx_buf, PERIPHR_L2CAP_BUFF_SIZE, app_per_l2cap_tx_buf, PERIPHR_L2CAP_BUFF_SIZE);

    blc_att_setCentralRxMtuSize(CENTRAL_ATT_RX_MTU);    ///must be placed after "blc_gap_init"
    blc_att_setPeripheralRxMtuSize(PERIPHR_ATT_RX_MTU); ///must be placed after "blc_gap_init"

/* GATT Initialization */
#if (ACL_CENTRAL_SIMPLE_SDP_ENABLE)
    host_att_register_idle_func(main_idle_loop);
#endif
    blc_gatt_register_data_handler(app_gatt_data_handler);

/* SMP Initialization */
#if (ACL_PERIPHR_SMP_ENABLE || ACL_CENTRAL_SMP_ENABLE)
    /* Note: If change IC type, need to confirm the FLASH_SIZE_CONFIG */
    blc_smp_configPairingSecurityInfoStorageAddressAndSize(flash_sector_smp_storage, FLASH_SMP_PAIRING_MAX_SIZE);
#endif

#if (ACL_PERIPHR_SMP_ENABLE)                                                   //Peripheral SMP Enable
    blc_smp_setSecurityLevel_periphr(Unauthenticated_Pairing_with_Encryption); //LE_Security_Mode_1_Level_2
#else
    blc_smp_setSecurityLevel_periphr(No_Security);
#endif

#if (ACL_CENTRAL_SMP_ENABLE)
    blc_smp_setSecurityLevel_central(Unauthenticated_Pairing_with_Encryption); //LE_Security_Mode_1_Level_2
#else
    blc_smp_setSecurityLevel_central(No_Security);
#endif

    blc_smp_smpParamInit();


    //host(GAP/SMP/GATT/ATT) event process: register host event callback and set event mask
    blc_gap_registerHostEventHandler(app_host_event_callback);
    blc_gap_setEventMask(GAP_EVT_MASK_SMP_PAIRING_BEGIN | GAP_EVT_MASK_SMP_PAIRING_SUCCESS | GAP_EVT_MASK_SMP_PAIRING_FAIL | GAP_EVT_MASK_SMP_CONN_ENCRYPTION_DONE |
                         GAP_EVT_MASK_SMP_SECURITY_PROCESS_DONE);
    //////////// Host Initialization  End /////////////////////////

    //////////////////////////// BLE stack Initialization  End //////////////////////////////////


    //////////////////////////// User Configuration for BLE application ////////////////////////////

    rf_set_power_level_index(RF_POWER_P3dBm);

#if (BLE_APP_PM_ENABLE)
    blc_ll_initPowerManagement_module();
    blc_pm_setSleepMask(PM_SLEEP_LEG_ADV | PM_SLEEP_LEG_SCAN | PM_SLEEP_ACL_PERIPHR | PM_SLEEP_ACL_CENTRAL);
#endif

    blc_ll_initExtendedAdvModule_initExtendedAdvSetParamBuffer(app_extAdvSetParam_buf, APP_EXT_ADV_SETS_NUMBER);
    blc_ll_initExtendedAdvDataBuffer(app_extAdvData_buf, APP_EXT_ADV_DATA_LENGTH);
    blc_ll_initExtendedScanRspDataBuffer(app_extScanRspData_buf, APP_EXT_SCANRSP_DATA_LENGTH);

    app_parse_init(app_funcs, ARRAY_SIZE(app_funcs));
    app_parse_printf("ext_adv init\r\n");
    ////////////////////////////////////////////////////////////////////////////////////////////////
}

/**
 * @brief        user initialization when MCU wake_up from deepSleep_retention mode
 * @param[in]    none
 * @return      none
 */
void user_init_deepRetn(void) {}

/////////////////////////////////////////////////////////////////////
// main loop flow
/////////////////////////////////////////////////////////////////////

/**
 * @brief     BLE main idle loop
 * @param[in]  none.
 * @return     none.
 */
int main_idle_loop(void)
{
    ////////////////////////////////////// BLE entry /////////////////////////////////
    blc_sdk_main_loop();


////////////////////////////////////// Debug entry /////////////////////////////////
#if (TLKAPI_DEBUG_ENABLE)
    tlkapi_debug_handler();
#endif

////////////////////////////////////// UI entry /////////////////////////////////
#if (BATT_CHECK_ENABLE)
    /*The frequency of low battery detect is controlled by the variable lowBattDet_tick, which is executed every
         500ms in the demo. Users can modify this time according to their needs.*/
    if (battery_get_detect_enable() && clock_time_exceed(lowBattDet_tick, 500000)) {
        lowBattDet_tick = clock_time();
        user_battery_power_check(BAT_DEEP_THRESHOLD_MV);
    }
#endif

#if (UI_BUTTON_ENABLE)
    static u8 button_detect_en = 0;
    if (!button_detect_en && clock_time_exceed(0, 1000000)) { // process button 1 second later after power on
        button_detect_en = 1;
    }
    if (button_detect_en) {
        proc_button(); //button triggers pair & unpair  and OTA
    }
#elif (UI_KEYBOARD_ENABLE)
    proc_keyboard(0, 0, 0);
#endif


    proc_central_role_unpair();

#if (APPLICATION_DONGLE)
    usb_handle_irq();
#endif


    app_parse_loop();

    return 0; //must return 0 due to SDP flow
}

/**
 * @brief     BLE main loop
 * @param[in]  none.
 * @return     none.
 */
_attribute_no_inline_ void main_loop(void)
{
    main_idle_loop();

#if (ACL_CENTRAL_SIMPLE_SDP_ENABLE)
    simple_sdp_loop();
#endif
}

#endif
#endif
