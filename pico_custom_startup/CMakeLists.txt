# Generated CMake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Pico VS Code extension configuration
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()

set(sdkVersion 2.3.1)
set(toolchainVersion 15_2_Rel1)
set(picotoolVersion 2.3.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)

if(EXISTS ${picoVscode})
    include(${picoVscode})
endif()

set(PICO_BOARD pico CACHE STRING "Board type")

include(pico_sdk_import.cmake)

project(pico_custom_startup C CXX ASM)

pico_sdk_init()

# Keep the SDK linker script, boot stage, and vector table, but replace the
# SDK reset routine with the implementation in startup.S.
add_executable(pico_custom_startup
    startup.S
    main.c
)

pico_set_compile_definition(pico_custom_startup PICO_CRT0_NO_RESET_SECTION 1)

pico_set_program_name(pico_custom_startup "pico_custom_startup")
pico_set_program_version(pico_custom_startup "0.1")

pico_enable_stdio_uart(pico_custom_startup 0)
pico_enable_stdio_usb(pico_custom_startup 0)

target_link_libraries(pico_custom_startup
    pico_stdlib
)

target_include_directories(pico_custom_startup PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(pico_custom_startup)
