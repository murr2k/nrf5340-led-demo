# nRF5340 LED Demo - Device Flashing Guide

## 🎯 Device Status
- **Device ID**: 1366:1055 (SEGGER J-Link)
- **Connection**: USB Bus 001 Device 002
- **Serial Ports**: /dev/ttyACM0, /dev/ttyACM1, /dev/ttyACM2
- **Status**: ✅ **DETECTED AND READY**

## 📋 Prerequisites Setup

### 1. Install Zephyr SDK
```bash
# Download and install Zephyr SDK (Linux)
cd ~/
wget https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v0.16.3/zephyr-sdk-0.16.3_linux-x86_64.tar.xz
tar xf zephyr-sdk-0.16.3_linux-x86_64.tar.xz
cd zephyr-sdk-0.16.3
./setup.sh
```

### 2. Install West and Python Dependencies
```bash
# Install west meta-tool
pip3 install west

# Install Python dependencies
pip3 install PyYAML
pip3 install pykwalify
pip3 install colorama
pip3 install packaging
```

### 3. Setup Zephyr Workspace
```bash
# Initialize Zephyr workspace
west init ~/zephyrproject
cd ~/zephyrproject
west update
west zephyr-export
source zephyr/zephyr-env.sh
```

### 4. Install nRF Command Line Tools (Optional)
```bash
# Download from Nordic Semiconductor
# https://www.nordicsemi.com/Products/Development-tools/nrf-command-line-tools
# Extract and add to PATH
```

## 🔧 Build and Flash Instructions

### Method 1: Using Build Script (Recommended)
```bash
# Navigate to project directory
cd /home/murr2k/projects/nrf5340-led-demo

# Set up Zephyr environment
source ~/zephyrproject/zephyr/zephyr-env.sh

# Build and flash in one command
./build.sh all

# Or step by step:
./build.sh build    # Build the firmware
./build.sh flash    # Flash to device
./build.sh monitor  # Start serial monitor
```

### Method 2: Using West Directly
```bash
# Build the project
west build -b nrf5340dk_nrf5340_cpuapp

# Flash the device
west flash

# Start serial monitor
west espressif monitor
```

### Method 3: Using Make
```bash
# Build the project
make build

# Flash the device
make flash

# Start serial monitor
make monitor
```

## 📱 Expected Flashing Process

### 1. Build Output
```
Building nRF5340 LED Demo for nrf5340dk_nrf5340_cpuapp...
-- west build: generating a build system
-- Zephyr version: 3.4.0
-- Found Python3: /usr/bin/python3.8
-- Found DTC: /usr/bin/dtc
-- Board: nrf5340dk_nrf5340_cpuapp
-- Cache files will be written to: /home/user/.cache/zephyr
-- Found toolchain: zephyr 0.16.3 (/opt/zephyr-sdk-0.16.3)
-- Found BOARD.dts: /home/user/zephyrproject/zephyr/boards/arm/nrf5340dk_nrf5340_cpuapp/nrf5340dk_nrf5340_cpuapp.dts
-- Generated zephyr.dts: /home/user/nrf5340-led-demo/build/zephyr/zephyr.dts
-- Generated devicetree_unfixed.h: /home/user/nrf5340-led-demo/build/zephyr/include/generated/devicetree_unfixed.h
-- Generated device_extern.h: /home/user/nrf5340-led-demo/build/zephyr/include/generated/device_extern.h
-- Including boilerplate (Zephyr base): /home/user/zephyrproject/zephyr/cmake/app/boilerplate.cmake
-- Application: /home/user/nrf5340-led-demo
-- Zephyr version: 3.4.0 (/home/user/zephyrproject/zephyr)
-- Found west (found suitable version \"0.14.0\", minimum required is \"0.7.1\")
-- Board: nrf5340dk_nrf5340_cpuapp
-- BOARD_DIR: /home/user/zephyrproject/zephyr/boards/arm/nrf5340dk_nrf5340_cpuapp
-- Found dtc: /usr/bin/dtc (found suitable version \"1.4.7\", minimum required is \"1.4.6\")
-- Found ZEPHYR_TOOLCHAIN_VARIANT: zephyr
-- Found host-tools: zephyr 0.16.3 (/opt/zephyr-sdk-0.16.3)
-- Found toolchain: zephyr 0.16.3 (/opt/zephyr-sdk-0.16.3)
-- Found Dtc: /usr/bin/dtc (found suitable version \"1.4.7\", minimum required is \"1.4.6\") 
-- Found BOARD.dts: /home/user/zephyrproject/zephyr/boards/arm/nrf5340dk_nrf5340_cpuapp/nrf5340dk_nrf5340_cpuapp.dts
-- Found devicetree overlay: /home/user/nrf5340-led-demo/boards/nrf5340dk_nrf5340_cpuapp.overlay
-- Generated zephyr.dts: /home/user/nrf5340-led-demo/build/zephyr/zephyr.dts
-- Generated devicetree_unfixed.h: /home/user/nrf5340-led-demo/build/zephyr/include/generated/devicetree_unfixed.h
-- Generated device_extern.h: /home/user/nrf5340-led-demo/build/zephyr/include/generated/device_extern.h
-- Including boilerplate (Zephyr base): /home/user/zephyrproject/zephyr/cmake/app/boilerplate.cmake
-- Application: /home/user/nrf5340-led-demo
-- Zephyr version: 3.4.0 (/home/user/zephyrproject/zephyr)
-- Found west (found suitable version \"0.14.0\", minimum required is \"0.7.1\")
-- Board: nrf5340dk_nrf5340_cpuapp
-- BOARD_DIR: /home/user/zephyrproject/zephyr/boards/arm/nrf5340dk_nrf5340_cpuapp
-- Found dtc: /usr/bin/dtc (found suitable version \"1.4.7\", minimum required is \"1.4.6\")
-- Found ZEPHYR_TOOLCHAIN_VARIANT: zephyr
-- Found host-tools: zephyr 0.16.3 (/opt/zephyr-sdk-0.16.3)
-- Found toolchain: zephyr 0.16.3 (/opt/zephyr-sdk-0.16.3)
-- Found Dtc: /usr/bin/dtc (found suitable version \"1.4.7\", minimum required is \"1.4.6\") 
-- Check for working C compiler: /opt/zephyr-sdk-0.16.3/arm-zephyr-eabi/bin/arm-zephyr-eabi-gcc
-- Check for working C compiler: /opt/zephyr-sdk-0.16.3/arm-zephyr-eabi/bin/arm-zephyr-eabi-gcc -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working CXX compiler: /opt/zephyr-sdk-0.16.3/arm-zephyr-eabi/bin/arm-zephyr-eabi-g++
-- Check for working CXX compiler: /opt/zephyr-sdk-0.16.3/arm-zephyr-eabi/bin/arm-zephyr-eabi-g++ -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Configuring done
-- Generating done
-- Build files have been written to: /home/user/nrf5340-led-demo/build
[1/171] Preparing syscall dependency handling

[2/171] Generating include/generated/version.h
-- Zephyr version: 3.4.0 (/home/user/zephyrproject/zephyr), build: zephyr-v3.4.0

[3/171] Generating misc/generated/syscalls.json
Parsing /home/user/zephyrproject/zephyr/include/zephyr/drivers/uart.h
Parsing /home/user/zephyrproject/zephyr/include/zephyr/drivers/gpio.h
... (continues)

[171/171] Linking C executable zephyr/zephyr.elf
Memory region         Used Size  Region Size  %age Used
           FLASH:       39452 B      1020 KB      3.78%
            SRAM:       15384 B       448 KB      3.35%
        IDT_LIST:          0 GB         2 KB      0.00%
```

### 2. Flash Output
```
-- west flash: rebuilding
ninja: no work to do.
-- west flash: using runner nrfjprog
-- runners.nrfjprog: reset after flashing requested
-- runners.nrfjprog: Flashing file: /home/user/nrf5340-led-demo/build/zephyr/zephyr.hex
[ #################### ]  100%
-- runners.nrfjprog: Board with serial number 123456789 flashed successfully.
```

## 🔍 Verification Steps

### 1. Check Device Status
```bash
# Check if device is detected
lsusb | grep SEGGER
# Should show: Bus 001 Device 002: ID 1366:1055 SEGGER J-Link

# Check serial ports
ls /dev/ttyACM*
# Should show: /dev/ttyACM0 /dev/ttyACM1 /dev/ttyACM2
```

### 2. Monitor Serial Output
```bash
# Start serial monitor
./build.sh monitor

# Or using screen directly
screen /dev/ttyACM0 115200

# Expected output:
# [00:00:00.000,000] <inf> main: nRF5340 LED Demo Application Starting...
# [00:00:00.000,000] <inf> main: Board: nrf5340dk_nrf5340_cpuapp
# [00:00:00.000,000] <inf> main: Zephyr version: 3.4.0
# [00:00:00.001,000] <inf> main: All LEDs initialized successfully
# [00:00:00.001,000] <inf> main: Running startup sequence...
# [00:00:00.602,000] <inf> main: Starting LED patterns...
# [00:00:00.602,000] <inf> main: All LEDs: ON
# [00:00:01.102,000] <inf> main: All LEDs: OFF
# [00:00:01.602,000] <inf> main: All LEDs: ON
```

### 3. Visual Verification
You should see the following LED behavior on the nRF5340DK:

1. **Startup Sequence**: All 4 LEDs flash 3 times rapidly
2. **Pattern 1 (10 seconds)**: All LEDs blink together every 500ms
3. **Pattern 2 (10 seconds)**: LEDs light up sequentially (LED0→LED1→LED2→LED3→repeat)
4. **Pattern 3 (10 seconds)**: Single LED chases back and forth
5. **Pattern 4 (10 seconds)**: Progressive LED fill with bouncing effect
6. **Loop**: Returns to Pattern 1 and repeats

## 🚨 Troubleshooting

### Device Not Detected
```bash
# Check USB connection
lsusb | grep 1366:1055

# Check udev rules (if needed)
sudo usermod -a -G dialout $USER
```

### Build Errors
```bash
# Clean and rebuild
./build.sh clean
./build.sh build

# Check environment
source ~/zephyrproject/zephyr/zephyr-env.sh
west --version
```

### Flash Errors
```bash
# Reset device
nrfjprog --family NRF53 --reset

# Erase and reflash
nrfjprog --family NRF53 --eraseall
./build.sh flash
```

### Serial Monitor Issues
```bash
# Check port permissions
ls -la /dev/ttyACM*
sudo chmod 666 /dev/ttyACM0

# Try different port
screen /dev/ttyACM1 115200
```

## 📊 Project Statistics

**Memory Usage (Expected):**
- **Flash**: ~39 KB / 1020 KB (3.78%)
- **SRAM**: ~15 KB / 448 KB (3.35%)
- **Build Time**: ~30 seconds
- **Flash Time**: ~10 seconds

## 🎯 Next Steps

Once flashed successfully:
1. Monitor serial output for log messages
2. Observe LED patterns on the development board
3. Modify patterns in `src/main.c` if desired
4. Rebuild and reflash with changes

---

**🔗 Repository**: https://github.com/murr2k/nrf5340-led-demo  
**📧 Support**: Murray Kopit - murr2k@gmail.com