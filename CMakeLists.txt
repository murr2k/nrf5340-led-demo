# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

# Set the board if not specified
if(NOT DEFINED BOARD)
    set(BOARD nrf5340dk_nrf5340_cpuapp)
endif()

# Find Zephyr package
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

# Project definition
project(nrf5340_led_demo VERSION 1.0.0 LANGUAGES C)

# Add source files
target_sources(app PRIVATE src/main.c)

# Add include directories if needed
# target_include_directories(app PRIVATE include)

# Add compiler definitions
target_compile_definitions(app PRIVATE
    PROJECT_NAME="nRF5340 LED Demo"
    PROJECT_VERSION="${PROJECT_VERSION}"
)

# Optional: Add custom build configurations
if(CONFIG_DEBUG)
    target_compile_options(app PRIVATE -g -O0)
else()
    target_compile_options(app PRIVATE -O2)
endif()
