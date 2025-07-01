set(TL_BLE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../tl_ble_sdk)

zephyr_compile_definitions(
    -DSTD_GCC=1
	-DTLKAPI_USE_INTERNAL_SPECIAL_UART_TOOL=0
	-DSDK_RELEASE_CHECK_EN=1
	-D__PROJECT_ACL_CONN_DEMO__=1
	-DFIX_AUX_CONN_SLOT_IDX_CAL=1
	-DVCD_EN=0
	-DCHIP_TYPE=CHIP_TYPE_${SOC}
	-DBLC_ZEPHYR_BLE_INTEGRATION=1
	-DLL_FEATURE_SUPPORT_LE_DATA_LENGTH_EXTENSION=1
	-DBLUETOOTH_VER=BLUETOOTH_VER_5_0
	-DLL_FEATURE_SUPPORT_PRIVACY=1
	-DCONTROLLER_GEN_P256KEY_ENABLE=0
	-DCONN_MAX_NUM_CONFIG=CONN_MAX_NUM_C0_P1
	-DOPTIMIZE_RAM_FOR_MATTER=1
	-DLL_FEATURE_SUPPORT_LOCAL_RPA=0
	-DLL_FEATURE_SUPPORT_PHY_TEST_MODE=0
	-DLL_FEATURE_SUPPORT_LE_ENCRYPTION=0
	-DLL_FEATURE_SUPPORT_LE_LEGACY_SCANNING=0
	-DLL_FEATURE_SUPPORT_EXTENDED_SCANNER_FILTER_POLICIES=0
	-DLL_FEATURE_SUPPORT_LE_2M_PHY=1
	-DLL_FEATURE_SUPPORT_LE_CODED_PHY=0
	-DLL_FEATURE_SUPPORT_LE_EXTENDED_ADVERTISING=0
	-DLL_FEATURE_SUPPORT_LE_PERIODIC_ADVERTISING=0
	-DLL_FEATURE_SUPPORT_CHANNEL_SELECTION_ALGORITHM2=1
	-DLL_FEATURE_SUPPORT_MIN_USED_OF_USED_CHANNELS=0
	-DLL_FEATURE_SUPPORT_LE_EXTENDED_SCANNING=0
	-DLL_FEATURE_SUPPORT_LE_EXTENDED_INITIATE=0
	-DLL_FEATURE_SUPPORT_LE_PERIODIC_ADVERTISING_SYNC=0
	-DLL_FEATURE_SUPPORT_SYNCHRONIZED_RECEIVER=0
	-DTIFS_VARIATION_WORKAROUND_MLP_CODE_IN_RAM=0
	-DLL_FEATURE_SUPPORT_LE_PING=0
	-DLE_AUTHENTICATED_PAYLOAD_TIMEOUT_SUPPORT_EN=0
	-DDEBUG_GPIO_ENABLE=0
	-DSCH_TASK_PRIORITY_IN_CB_EN=0
	-DSTACK_IRQ_CODE_IN_SRAM_DUE_TO_FLASH_OPERATION=0
	-DSTACK_IRQ_CODE_IN_SRAM_DUE_TO_FLASH_OPERATION_V2=0
	-DBLC_PM_RETENTION_DRIVERS_ONLY=1
	-DBLC_PM_DEEP_RETENTION_MODE_EN=0
	-DPM_DEEPSLEEP_RETENTION_ENABLE=0
	-DBLMS_PM_ENABLE=0
	-DBLC_PM_EN=0    
)

file(GLOB SOC_SOURCES
	${TL_BLE_SRC_DIR}/drivers/${SOC}/error_handler/*.c
	${TL_BLE_SRC_DIR}/drivers/${SOC}/ext_driver/*.c
	${TL_BLE_SRC_DIR}/drivers/${SOC}/flash/*.c
	${TL_BLE_SRC_DIR}/drivers/${SOC}/watchdog.c
	${TL_BLE_SRC_DIR}/drivers/${SOC}/dma.c
	${TL_BLE_SRC_DIR}/vendor/common/flash_prot.c
	${TL_BLE_SRC_DIR}/vendor/common/ble_flash.c
)
zephyr_sources(${SOC_SOURCES})

zephyr_include_directories(
	${TL_BLE_SRC_DIR}
	${TL_BLE_SRC_DIR}/drivers/${SOC}
	${TL_BLE_SRC_DIR}/drivers/${SOC}/compatibility_pack
	${TL_BLE_SRC_DIR}/drivers/${SOC}/reg_include
	${TL_BLE_SRC_DIR}/drivers/${SOC}/ext_driver
	${TL_BLE_SRC_DIR}/stack/ble/hal/hal_${SOC}
	${TL_BLE_SRC_DIR}/common
	${TL_BLE_SRC_DIR}/vendor/common
)

if(NOT CONFIG_SOC_RISCV_TELINK_B91)
	zephyr_include_directories(
		${TL_BLE_SRC_DIR}/drivers/${SOC}/flash
		${TL_BLE_SRC_DIR}/drivers/${SOC}/lib/include
		${TL_BLE_SRC_DIR}/drivers/${SOC}/lib/include/pke
	)
	if(CONFIG_SOC_RISCV_TELINK_B92)
		zephyr_include_directories(
			${TL_BLE_SRC_DIR}/drivers/${SOC}/ext_peripherals/codec_0581
		)
	endif()
	if(CONFIG_SOC_SERIES_RISCV_TELINK_TLX OR CONFIG_SOC_RISCV_TELINK_TLX)
		zephyr_include_directories(
			${TL_BLE_SRC_DIR}/drivers/${SOC}/lib/include/rf
			${TL_BLE_SRC_DIR}/drivers/${SOC}/lib/include/ske
			${TL_BLE_SRC_DIR}/drivers/${SOC}/lib/include/trng
		)
	endif()
	if(CONFIG_SOC_SERIES_RISCV_TELINK_TLX OR CONFIG_SOC_RISCV_TELINK_TLX)
		zephyr_include_directories(${TL_BLE_SRC_DIR}/drivers/${SOC}/ext_driver/driver_internal)
	elseif(CONFIG_SOC_SERIES_RISCV_TELINK_B9X)
		zephyr_include_directories(${TL_BLE_SRC_DIR}/drivers/${SOC}/ext_driver/driver_lib)
	endif()
endif()

if(NOT (CONFIG_SOC_SERIES_RISCV_TELINK_TLX OR CONFIG_SOC_RISCV_TELINK_TLX))
	zephyr_sources(${TL_BLE_SRC_DIR}/drivers/${SOC}/clock.c)
	zephyr_sources(${TL_BLE_SRC_DIR}/drivers/${SOC}/analog.c)
endif()

if(CONFIG_SOC_FLASH_TELINK_B9X OR CONFIG_USB_TELINK_B9X)
	zephyr_sources(
		${TL_BLE_SRC_DIR}/drivers/${SOC}/flash.c
		${TL_BLE_SRC_DIR}/drivers/${SOC}/stimer.c
	)
endif()

if(CONFIG_SOC_FLASH_TELINK_TLX OR CONFIG_USB_TELINK_TLX)
	zephyr_sources(${TL_BLE_SRC_DIR}/drivers/${SOC}/flash.c)
endif()

if(CONFIG_GPIO_TELINK_B9X OR CONFIG_GPIO_TELINK_TLX)
	zephyr_sources(${TL_BLE_SRC_DIR}/drivers/${SOC}/gpio.c)
endif()

if(CONFIG_PWM_TELINK_B9X OR CONFIG_PWM_TELINK_TLX OR CONFIG_SOC_RISCV_TELINK_TLX)
	zephyr_sources(${TL_BLE_SRC_DIR}/drivers/${SOC}/pwm.c)
endif()

if(CONFIG_SPI_TELINK_B9X OR CONFIG_SPI_TELINK_TLX)
	zephyr_sources(${TL_BLE_SRC_DIR}/drivers/${SOC}/gpio.c)
endif()

if(CONFIG_I2C_TELINK_B9X OR CONFIG_I2C_TELINK_TLX)
	zephyr_sources(${TL_BLE_SRC_DIR}/drivers/${SOC}/i2c.c)
endif()

if(CONFIG_BT OR CONFIG_IEEE802154)
	if(NOT ${SOC} STREQUAL "TL721X")
		zephyr_sources(${TL_BLE_SRC_DIR}/drivers/${SOC}/aes.c)
	endif()
	if(CONFIG_BT_B9X OR CONFIG_IEEE802154_TELINK_B9X)
		zephyr_sources(${TL_BLE_SRC_DIR}/drivers/${SOC}/stimer.c)
	endif()
endif()

if(CONFIG_ADC_TELINK_B9X OR CONFIG_ADC_TELINK_TLX)
	zephyr_sources(${TL_BLE_SRC_DIR}/drivers/${SOC}/gpio.c)
	zephyr_sources(${TL_BLE_SRC_DIR}/drivers/${SOC}/adc.c)
endif()

if(CONFIG_USB_TELINK_B9X OR CONFIG_USB_TELINK_TLX)
	zephyr_sources(${TL_BLE_SRC_DIR}/drivers/${SOC}/gpio.c)
	zephyr_sources(${TL_BLE_SRC_DIR}/drivers/${SOC}/usbhw.c)
endif()

if(CONFIG_PM)
	zephyr_sources(common/tl_sleep.c)
	zephyr_sources_ifdef(NOT (CONFIG_SOC_SERIES_RISCV_TELINK_TLX OR CONFIG_SOC_RISCV_TELINK_TLX)
		${TL_BLE_SRC_DIR}/drivers/${SOC}/stimer.c
	)
endif()

if(CONFIG_BT_B9X OR CONFIG_BT_TLX)
	zephyr_sources(
		${TL_BLE_SRC_DIR}/common/utility.c
	)
endif()