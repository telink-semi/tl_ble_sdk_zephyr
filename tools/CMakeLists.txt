set(TL_BLE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../tl_ble_sdk)

# include(${CMAKE_CURRENT_LIST_DIR}/cmake/general_compile_options.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/ram_optimize_compile_options.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/drivers.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/stack.cmake)
