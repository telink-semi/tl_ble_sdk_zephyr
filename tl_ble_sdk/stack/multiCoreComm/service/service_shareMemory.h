
#include"../comm.h"
#include"../drv/shareMemory.h"

/**
 *  @brief HCI Communication Buffers
 */
#ifndef TLK_SM_HCI_TX_BUFFER_SIZE
#define TLK_SM_HCI_TX_BUFFER_SIZE       268  /**< HCI TX buffer byte size (max HCI packet length) */
#endif

#ifndef TLK_SM_HCI_TX_BUFFER_NUM
#define TLK_SM_HCI_TX_BUFFER_NUM        16   /**< Number of HCI TX buffers for queueing */
#endif

#ifndef TLK_SM_HCI_RX_BUFFER_SIZE
#define TLK_SM_HCI_RX_BUFFER_SIZE       268  /**< HCI RX buffer byte size (fixed to match TX) */
#endif

#ifndef TLK_SM_HCI_RX_BUFFER_NUM
#define TLK_SM_HCI_RX_BUFFER_NUM        16   /**< Number of HCI RX buffers */
#endif

/**
 *  @brief Log Buffers
 */
#ifndef TLK_SM_LOG_BUFFER_SIZE
#define TLK_SM_LOG_BUFFER_SIZE          64   /**< Single log entry size (bytes) */
#endif

#ifndef TLK_SM_LOG_BUFFER_NUM
#define TLK_SM_LOG_BUFFER_NUM           32   /**< Total log buffer entries */
#endif

/**
 *  @brief Synchronization Buffers
 */
#ifndef TLK_SM_SYNC_BUFFER_SIZE
#define TLK_SM_SYNC_BUFFER_SIZE         16   /**< Sync buffer size (bytes) */
#endif

#ifndef TLK_SM_SYNC_BUFFER_NUM
#define TLK_SM_SYNC_BUFFER_NUM          4    /**< Number of sync buffers */
#endif

/**
 *  @brief Connection Security Buffers
 */
#ifndef TLK_SM_CS_RAW_PCT_BUFFER_SIZE
#define TLK_SM_CS_RAW_PCT_BUFFER_SIZE   16   /**< Raw PCT buffer size for connection security */
#endif

#ifndef TLK_SM_CS_RAW_PCT_BUFFER_NUM
#define TLK_SM_CS_RAW_PCT_BUFFER_NUM    8    /**< Number of raw PCT buffers */
#endif

/**
 *  @brief Flash Operation Buffers
 */
#ifndef TLK_SM_FLASH_WR_BUFFER_SIZE
#define TLK_SM_FLASH_WR_BUFFER_SIZE     32   /**< Flash write buffer size (bytes) */
#endif

#ifndef TLK_SM_FLASH_WR_BUFFER_NUM
#define TLK_SM_FLASH_WR_BUFFER_NUM      4    /**< Number of flash write buffers */
#endif

u32 tlk_sm_get_cs_raw_buff_addr(void);

int tlk_sm_cs_raw_buff_is_ready(void);

void tlk_share_memory_service_init(void);

void tlk_share_memory_service_loop(void);

void tlk_share_memory_service_hci_handler(void);

void tlk_share_memory_service_log_handler(void);

#if(TLK_MAILBOX_MESSAGE_HOST_EN == 1)
void tlk_d25f_register_hci_receive_cb(tlk_sm_message_type_e type,tlk_sm_rx_cb_f cb);

void tlk_d25f_register_sync_receive_cb(tlk_sm_message_type_e type,tlk_sm_rx_cb_f cb);

void tlk_d25f_register_log_receive_cb(tlk_sm_message_type_e type,tlk_sm_rx_cb_f cb);

tlk_sm_ret_e tlk_d25f_hci_send_message(tlk_sm_message_type_e type,u8 *data, u32 dataLen);

#elif(TLK_MAILBOX_MESSAGE_CONTROLLER_EN == 1)
tlk_sm_ret_e tlk_n22_hci_send_message(tlk_sm_message_type_e type,u8 *data, u32 dataLen);

tlk_sm_ret_e tlk_n22_sync_send_message(tlk_sm_message_type_e type,u8 *data, u32 dataLen);

tlk_sm_ret_e tlk_n22_log_send_message(tlk_sm_message_type_e type,u8 *data, u32 dataLen);

void tlk_n22_register_hci_receive_cb(tlk_sm_message_type_e type,tlk_sm_rx_cb_f cb);
#endif

