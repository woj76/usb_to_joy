cmake_minimum_required(VERSION 3.13)

if(NOT PICO_BOARD)
	set(PICO_BOARD pico)
endif()

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

project(usb_to_joy C CXX ASM)

pico_sdk_init()

add_executable(usb_to_joy)

target_sources(usb_to_joy PUBLIC ${CMAKE_CURRENT_LIST_DIR}/main.c)

target_include_directories(usb_to_joy PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(usb_to_joy PUBLIC pico_stdlib tinyusb_host tinyusb_board)

pico_add_extra_outputs(usb_to_joy)
