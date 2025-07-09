#!/bin/bash

# Build script for nRF5340 LED Demo
# Copyright (c) 2024 Murray Kopit
# SPDX-License-Identifier: Apache-2.0

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Project configuration
PROJECT_NAME="nRF5340 LED Demo"
BOARD="nrf5340dk/nrf5340/cpuapp"
BUILD_DIR="build"
TARGET_DIR="target"

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to check prerequisites
check_prerequisites() {
    print_status "Checking prerequisites..."
    
    # Check if west is installed
    if ! command_exists west; then
        print_error "West tool not found. Please install Zephyr SDK and west."
        print_status "Installation instructions: https://docs.zephyrproject.org/latest/getting_started/index.html"
        exit 1
    fi
    
    # Check if ZEPHYR_BASE is set
    if [ -z "$ZEPHYR_BASE" ]; then
        print_warning "ZEPHYR_BASE not set. Attempting to source zephyr-env.sh..."
        if [ -f "~/zephyrproject/zephyr/zephyr-env.sh" ]; then
            source ~/zephyrproject/zephyr/zephyr-env.sh
        else
            print_error "Zephyr environment not found. Please set up Zephyr workspace."
            exit 1
        fi
    fi
    
    # Check if nrfjprog is available (for flashing)
    if ! command_exists nrfjprog; then
        print_warning "nrfjprog not found. Flashing may not work."
        print_status "Install nRF Command Line Tools: https://www.nordicsemi.com/Products/Development-tools/nrf-command-line-tools"
    fi
    
    print_success "Prerequisites check completed."
}

# Function to clean build directory
clean_build() {
    print_status "Cleaning build directory..."
    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
        print_success "Build directory cleaned."
    else
        print_status "Build directory already clean."
    fi
}

# Function to build the project
build_project() {
    print_status "Building $PROJECT_NAME for $BOARD..."
    
    # Create build directory
    mkdir -p "$BUILD_DIR"
    
    # Build with west
    if west build -b "$BOARD" -d "$BUILD_DIR" .; then
        print_success "Build completed successfully!"
        
        # Copy output files to target directory
        mkdir -p "$TARGET_DIR"
        if [ -f "$BUILD_DIR/zephyr/zephyr.hex" ]; then
            cp "$BUILD_DIR/zephyr/zephyr.hex" "$TARGET_DIR/"
            print_status "HEX file copied to $TARGET_DIR/"
        fi
        
        if [ -f "$BUILD_DIR/zephyr/zephyr.elf" ]; then
            cp "$BUILD_DIR/zephyr/zephyr.elf" "$TARGET_DIR/"
            print_status "ELF file copied to $TARGET_DIR/"
        fi
        
        # Display build statistics
        print_status "Build statistics:"
        if [ -f "$BUILD_DIR/zephyr/zephyr.elf" ]; then
            size "$BUILD_DIR/zephyr/zephyr.elf"
        fi
        
        return 0
    else
        print_error "Build failed!"
        return 1
    fi
}

# Function to flash the device
flash_device() {
    print_status "Flashing device..."
    
    if [ ! -f "$BUILD_DIR/zephyr/zephyr.hex" ]; then
        print_error "No HEX file found. Please build the project first."
        return 1
    fi
    
    if command_exists west; then
        if west flash -d "$BUILD_DIR"; then
            print_success "Device flashed successfully!"
            return 0
        else
            print_error "Flashing failed!"
            return 1
        fi
    else
        print_error "West tool not available for flashing."
        return 1
    fi
}

# Function to monitor serial output
monitor_serial() {
    print_status "Starting serial monitor..."
    print_status "Press Ctrl+C to exit monitor."
    
    # Try to find the serial device
    SERIAL_DEVICE="/dev/ttyACM0"
    
    if [ ! -e "$SERIAL_DEVICE" ]; then
        print_warning "$SERIAL_DEVICE not found. Trying /dev/ttyACM1..."
        SERIAL_DEVICE="/dev/ttyACM1"
    fi
    
    if [ ! -e "$SERIAL_DEVICE" ]; then
        print_error "Serial device not found. Please connect the nRF5340DK."
        return 1
    fi
    
    # Use screen if available, otherwise try minicom
    if command_exists screen; then
        screen "$SERIAL_DEVICE" 115200
    elif command_exists minicom; then
        minicom -D "$SERIAL_DEVICE" -b 115200
    else
        print_error "No serial monitor available. Please install screen or minicom."
        return 1
    fi
}

# Function to show help
show_help() {
    echo "Usage: $0 [OPTION]"
    echo ""
    echo "Options:"
    echo "  build     Build the project (default)"
    echo "  clean     Clean build directory"
    echo "  flash     Flash the device"
    echo "  monitor   Start serial monitor"
    echo "  all       Clean, build, and flash"
    echo "  help      Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0              # Build the project"
    echo "  $0 build        # Build the project"
    echo "  $0 clean        # Clean build directory"
    echo "  $0 flash        # Flash the device"
    echo "  $0 monitor      # Start serial monitor"
    echo "  $0 all          # Clean, build, and flash"
    echo ""
    echo "Environment variables:"
    echo "  ZEPHYR_BASE     Path to Zephyr installation"
    echo "  BOARD           Target board (default: $BOARD)"
    echo ""
}

# Function to display project info
show_info() {
    echo "$PROJECT_NAME Build Script"
    echo "Board: $BOARD"
    echo "Build Directory: $BUILD_DIR"
    echo "Target Directory: $TARGET_DIR"
    echo ""
}

# Main function
main() {
    show_info
    
    # Parse command line arguments
    case "${1:-build}" in
        "build")
            check_prerequisites
            build_project
            ;;
        "clean")
            clean_build
            ;;
        "flash")
            check_prerequisites
            flash_device
            ;;
        "monitor")
            monitor_serial
            ;;
        "all")
            check_prerequisites
            clean_build
            build_project && flash_device
            ;;
        "help")
            show_help
            ;;
        *)
            print_error "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
}

# Run main function
main "$@"
