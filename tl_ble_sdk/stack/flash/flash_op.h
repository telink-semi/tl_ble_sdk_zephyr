/********************************************************************************************************
 * @file    flash_op.h
 *
 * @brief   This is the source file for Bluetooth SDK
 *
 * @author  BLE GROUP
 * @date    04,2026
 *
 * @par     Copyright (c) 2026, Telink Semiconductor (Shanghai) Co., Ltd.
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

#ifndef FLASH_OP_H
#define FLASH_OP_H

#include "tl_common.h"

/********************************************************************************************************
 *                                           FLASH CONFIGURATION                                          *
 ********************************************************************************************************/

// Flash operation parameters
#define FLASH_SECTOR_SIZE          4096    // 4KB sector size
#define FLASH_PAGE_SIZE            256     // 256B page size
#define FLASH_SAFE_VOLTAGE         3000    // 3.0V minimum safe voltage for flash operations
#define FLASH_ERASE_TIME_US        20000   // Sector erase time in microseconds (20ms)
#define FLASH_OP_PAGE_BASE         2500    // Maximum time for a single byte operation (2.5ms)
#define FLASH_OP_MIN_GAP_TICK      1000    // Minimum gap between flash operations in system ticks
#define FLASH_BATCH_MAX_SIZE       FLASH_SECTOR_SIZE  // Maximum batch write size (one sector)
#define FLASH_BATCH_MIN_SIZE       512     // Minimum batch write size

// Flash operation queue parameters
#define FLASH_OP_QUEUE_SIZE        4       // Maximum number of pending flash operations

// Flash operation types
#define FLASH_OP_TYPE_WRITE        0       // Write operation
#define FLASH_OP_TYPE_READ         1       // Read operation

/********************************************************************************************************
 *                                             DATA STRUCTURES                                            *
 ********************************************************************************************************/

typedef enum {
    FLASH_OP_ok = 0,    // Flash operation successful
    FLASH_OP_FAILED,         // Flash operation failed
    FLASH_OP_VOLTAGE_ERROR,  // Voltage too low for flash operation
    FLASH_OP_TIMEOUT,        // Flash operation timed out
    FLASH_OP_BUSY,           // Flash is busy
    FLASH_OP_INVALID_PARAM,  // Invalid parameters
    FLASH_OP_QUEUE_FULL,     // Flash operation queue is full
} flash_op_result_t;

/**
 * @brief Flash operation completion callback function type
 * @param result     Operation result
 * @param op_id      Operation ID for tracking
 * @param op_type    Operation type: FLASH_OP_TYPE_WRITE or FLASH_OP_TYPE_READ
 * @param start_addr Start address of the operation
 * @param len        Length of data processed
 * @param data       Pointer to data buffer (for read: contains read data; for write: contains written data)
 */
typedef void (*flash_op_complete_cb_t)(flash_op_result_t result, u16 op_id, u8 op_type, 
                                       u32 start_addr, u32 len, u8 *data);

/**
 * @brief Flash operation request structure
 */
typedef struct {
    u32 start_addr;                 // Start address of flash operation
    u32 total_len;                  // Total length of data to write/read
    u8 data_buf[256];               // Data buffer (max 256 bytes per operation)
    flash_op_complete_cb_t complete_cb; // Completion callback function
    u32 operation_time_us;          // Time required for flash operation (in microseconds)
    u32 bytes_written;              // Number of bytes already written
    u8 need_erase;                  // Whether sector erase is needed
    u8 op_type;                    // Operation type: FLASH_OP_TYPE_WRITE or FLASH_OP_TYPE_READ
    u16 op_id;                     // Operation ID for tracking specific operations
} flash_op_req_t;

/********************************************************************************************************
 *                                             API FUNCTIONS                                             *
 ********************************************************************************************************/

/**
 * @brief   Initialize the flash operation module
 * @return  None
 */
void blc_flash_op_init(void);

/**
 * @brief   Add a flash write operation to the queue
 * @param   start_addr   Start address for flash write
 * @param   len          Length of data to write
 * @param   buf          Pointer to data buffer
 * @param   need_erase   Whether sector erase is needed before write
 * @param   complete_cb  Completion callback function (can be NULL)
 * @return  FLASH_OP_SUCCESS if successful, otherwise error code
 */
flash_op_result_t blc_flash_add_write_operation(u32 start_addr, u32 len, u8 *buf, u8 need_erase,
                                               flash_op_complete_cb_t complete_cb);

/**
 * @brief   Check if flash operation queue is empty
 * @return  1 if empty, 0 otherwise
 */
int blc_flash_op_queue_is_empty(void);

/**
 * @brief   Check if flash operation queue is full
 * @return  1 if full, 0 otherwise
 */
int blc_flash_op_queue_is_full(void);

/**
 * @brief   Get current flash operation queue length
 * @return  Number of pending flash operations
 */
u8 blc_flash_op_queue_length(void);

/**
 * @brief   Check if flash operation is busy 
 * @return  1 if busy (operations pending), 0 if idle
 */
int blc_flash_op_is_busy(void);

/**
 * @brief   Add a flash operation (read/write) with operation ID tracking 
 * @param   op_type      Operation type: FLASH_OP_TYPE_WRITE or FLASH_OP_TYPE_READ
 * @param   start_addr   Start address for flash operation
 * @param   len          Length of data to write/read
 * @param   buf          Pointer to data buffer (write: input, read: output)
 * @param   need_erase   Whether sector erase is needed (only for write)
 * @param   complete_cb  Completion callback function (can be NULL)
 * @return  Operation ID (>0) if successful, 0 if failed
 */
u16 blc_flash_add_operation_with_id(u8 op_type, u32 start_addr, u32 len, u8 *buf, u8 need_erase,
                                    flash_op_complete_cb_t complete_cb);

/**
 * @brief   Add a flash write operation with operation ID tracking (backward compatibility)
 * @note    This function is kept for backward compatibility with existing code
 * @param   start_addr   Start address for flash write
 * @param   len          Length of data to write
 * @param   buf          Pointer to data buffer
 * @param   need_erase   Whether sector erase is needed before write
 * @param   complete_cb  Completion callback function (can be NULL)
 * @return  Operation ID (>0) if successful, 0 if failed
 */
u16 blc_flash_add_write_operation_with_id(u32 start_addr, u32 len, u8 *buf, u8 need_erase,
                                          flash_op_complete_cb_t complete_cb);

/**
 * @brief   Query flash operation status by ID 
 * @param   op_id        Operation ID to query
 * @return  FLASH_OP_ok if operation completed, FLASH_OP_BUSY if still pending
 */
flash_op_result_t blc_flash_op_query_by_id(u16 op_id);

#endif /* FLASH_OP_H */
