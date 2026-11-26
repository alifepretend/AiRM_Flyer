# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
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
# ====================================================================================
set(PICO_BOARD pico2_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(AiRM_Flyer C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(AiRM_Flyer 
    main.c
    System.c
    Display.c
    Core1.c
)

pico_set_program_name(AiRM_Flyer "AiRM_Flyer")
pico_set_program_version(AiRM_Flyer "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(AiRM_Flyer 0)
pico_enable_stdio_usb(AiRM_Flyer 1)

# # Add the standard library to the build
# target_link_libraries(AiRM_Flyer PRIVATE
#         pico_stdlib)

# Add the standard include files to the build
target_include_directories(AiRM_Flyer PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(AiRM_Flyer PRIVATE
        pico_stdlib
        hardware_i2c
        pico_multicore 
        )


target_compile_definitions(AiRM_Flyer PRIVATE
    LCD_USE_I2C=1 # Comentar para usar display por barramento
)
pico_add_extra_outputs(AiRM_Flyer)

