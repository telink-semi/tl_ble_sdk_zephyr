
zephyr_include_directories(
	${TL_BLE_SRC_DIR}/drivers/${SOC}
	${TL_BLE_SRC_DIR}/drivers/${SOC}/compatibility_pack
	${TL_BLE_SRC_DIR}/drivers/${SOC}/reg_include
	${TL_BLE_SRC_DIR}/drivers/${SOC}/ext_driver
	${TL_BLE_SRC_DIR}
	${TL_BLE_SRC_DIR}/common
	${TL_BLE_SRC_DIR}/vendor/common
)

if(NOT CONFIG_SOC_RISCV_TELINK_B91)
	zephyr_include_directories(
		${TL_BLE_SRC_DIR}/drivers/${SOC}/flash
		${TL_BLE_SRC_DIR}/drivers/${SOC}/lib/include
		${TL_BLE_SRC_DIR}/drivers/${SOC}/lib/include/pke
	)
endif()

if(CONFIG_SOC_RISCV_TELINK_B92)
	zephyr_include_directories(
		${TL_BLE_SRC_DIR}/drivers/${SOC}/ext_peripherals/codec_0581
	)
endif()

if(CONFIG_SOC_FAMILY_TELINK_TLX)
	zephyr_include_directories(
		${TL_BLE_SRC_DIR}/drivers/${SOC}/lib/include/rf
		${TL_BLE_SRC_DIR}/drivers/${SOC}/lib/include/ske
		${TL_BLE_SRC_DIR}/drivers/${SOC}/lib/include/trng
		${TL_BLE_SRC_DIR}/drivers/${SOC}/lib/include/pm
	)
endif()

if(CONFIG_SOC_FAMILY_TELINK_TLX)
	zephyr_include_directories(${TL_BLE_SRC_DIR}/drivers/${SOC}/ext_driver/driver_internal)
elseif(CONFIG_SOC_SERIES_RISCV_TELINK_B9X)
	zephyr_include_directories(${TL_BLE_SRC_DIR}/drivers/${SOC}/ext_driver/driver_lib)
endif()

file(GLOB DRIVER_SOURCES
    ${TL_BLE_SRC_DIR}/drivers/${SOC}/error_handler/*.c
	${TL_BLE_SRC_DIR}/drivers/${SOC}/ext_driver/*.c
	${TL_BLE_SRC_DIR}/drivers/${SOC}/flash/*.c
	${TL_BLE_SRC_DIR}/drivers/${SOC}/*.c

	${TL_BLE_SRC_DIR}/common/utility.c
	${TL_BLE_SRC_DIR}/vendor/common/flash_prot.c
	${TL_BLE_SRC_DIR}/vendor/common/ble_flash.c
)
zephyr_sources(${DRIVER_SOURCES})

