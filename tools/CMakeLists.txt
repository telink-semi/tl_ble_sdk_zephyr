set(TL_BLE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../tl_ble_sdk)

if(CONFIG_TL_BLE_CTRL_PERIPHERAL)
	include(${CMAKE_CURRENT_LIST_DIR}/cmake/peripheral_compile_options.cmake)
elseif(CONFIG_TL_BLE_CTRL_CENTRAL)
	include(${CMAKE_CURRENT_LIST_DIR}/cmake/central_compile_options.cmake)
elseif(CONFIG_TL_BLE_CTRL_MULTIROLE)
	include(${CMAKE_CURRENT_LIST_DIR}/cmake/multirole_compile_options.cmake)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/cmake/drivers.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/stack.cmake)
