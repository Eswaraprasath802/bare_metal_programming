cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()

set(sdkVersion 2.3.1)
set(toolchainVersion 15_2_Rel1)
set(picotoolVersion 2.3.1)

set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)

if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()

# NORMAL RASPBERRY PI PICO
set(PICO_BOARD pico CACHE STRING "Board type")

include(pico_sdk_import.cmake)

project(uart-controller C CXX ASM)

pico_sdk_init()

add_executable(uart-controller
    main.c
)

pico_set_program_name(uart-controller "uart-controller")
pico_set_program_version(uart-controller "0.1")

# We are doing UART manually using registers.
pico_enable_stdio_uart(uart-controller 0)
pico_enable_stdio_usb(uart-controller 0)

target_link_libraries(uart-controller
    pico_stdlib
)

target_include_directories(uart-controller PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(uart-controller)