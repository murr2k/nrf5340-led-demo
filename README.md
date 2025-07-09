# nRF5340 LED Demo - Zephyr RTOS

A comprehensive LED demonstration project for the nRF5340 Development Kit using Zephyr RTOS. This project showcases various LED blinking patterns and demonstrates basic GPIO control, logging, and real-time operating system concepts.

## Features

- **Multiple LED Patterns**: Four different LED animation patterns
- **Real-time Logging**: Comprehensive logging with configurable levels
- **Robust Error Handling**: Proper initialization and error checking
- **Pattern Cycling**: Automatic pattern switching every 10 seconds
- **Startup Sequence**: Visual confirmation of system initialization

## LED Patterns

1. **All Blink**: All 4 LEDs blink together (500ms intervals)
2. **Sequential**: LEDs light up one after another in sequence
3. **Chase**: A single LED "chases" back and forth
4. **Bounce**: Progressive LED fill with bouncing effect

## Hardware Requirements

- nRF5340 Development Kit (PCA10095)
- USB cable for programming and power
- Terminal emulator for viewing logs

## LED Configuration

The nRF5340DK has 4 user LEDs:
- **LED0**: P0.28 (Green)
- **LED1**: P0.29 (Green)
- **LED2**: P0.30 (Green)
- **LED3**: P0.31 (Green)

## Software Requirements

- **Zephyr RTOS**: Version 3.4.0 or later
- **West Tool**: Zephyr's meta-tool for building and flashing
- **ARM GCC Toolchain**: For cross-compilation
- **nRF Command Line Tools**: For programming the device

## Installation

### 1. Install Zephyr SDK

```bash
# Download and install Zephyr SDK
wget https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v0.16.3/zephyr-sdk-0.16.3_linux-x86_64.tar.xz
tar xf zephyr-sdk-0.16.3_linux-x86_64.tar.xz
cd zephyr-sdk-0.16.3
./setup.sh
```

### 2. Install West and Dependencies

```bash
pip3 install west
pip3 install -r ~/zephyrproject/zephyr/scripts/requirements.txt
```

### 3. Initialize Zephyr Workspace

```bash
west init ~/zephyrproject
cd ~/zephyrproject
west update
west zephyr-export
```

## Building the Project

### Method 1: Using West (Recommended)

```bash
# Navigate to the project directory
cd /path/to/nrf5340-led-demo

# Build for nRF5340DK
west build -b nrf5340dk_nrf5340_cpuapp

# Clean build (if needed)
west build -b nrf5340dk_nrf5340_cpuapp -t clean
```

### Method 2: Using CMake Directly

```bash
# Set up environment
export ZEPHYR_BASE=~/zephyrproject/zephyr
source $ZEPHYR_BASE/zephyr-env.sh

# Create build directory
mkdir build && cd build

# Configure and build
cmake -DBOARD=nrf5340dk_nrf5340_cpuapp ..
make -j$(nproc)
```

## Flashing the Device

### Prerequisites
- Connect nRF5340DK to your computer via USB
- Ensure the board is in DFU mode (if required)

### Using West

```bash
# Flash the application
west flash

# Flash with specific runner
west flash --runner nrfjprog
```

### Using nRF Command Line Tools

```bash
# Erase and program
nrfjprog --family NRF53 --eraseall
nrfjprog --family NRF53 --program build/zephyr/zephyr.hex --verify
nrfjprog --family NRF53 --reset
```

## Running and Debugging

### Viewing Logs

1. **Serial Console**: Connect to UART at 115200 baud
   ```bash
   # Using screen
   screen /dev/ttyACM0 115200
   
   # Using minicom
   minicom -D /dev/ttyACM0 -b 115200
   ```

2. **RTT (Real-Time Transfer)**: For faster logging
   ```bash
   JLinkRTTClient
   ```

### Expected Output

```
[00:00:00.000,000] <inf> main: nRF5340 LED Demo Application Starting...
[00:00:00.000,000] <inf> main: Board: nrf5340dk_nrf5340_cpuapp
[00:00:00.000,000] <inf> main: Zephyr version: 3.4.0
[00:00:00.001,000] <inf> main: All LEDs initialized successfully
[00:00:00.001,000] <inf> main: Running startup sequence...
[00:00:00.602,000] <inf> main: Starting LED patterns...
[00:00:00.602,000] <inf> main: All LEDs: ON
[00:00:01.102,000] <inf> main: All LEDs: OFF
```

## Project Structure

```
nrf5340-led-demo/
├── CMakeLists.txt                    # Main CMake build file
├── prj.conf                          # Project configuration
├── README.md                         # This file
├── boards/
│   └── nrf5340dk_nrf5340_cpuapp.overlay  # Device tree overlay
├── src/
│   └── main.c                        # Main application code
└── build/                            # Build output (generated)
    └── zephyr/
        ├── zephyr.elf                # Compiled ELF file
        └── zephyr.hex                # Intel HEX file for flashing
```

## Configuration Options

### Timing Configuration

Modify these values in `src/main.c`:

```c
#define BLINK_DELAY_MS 500      // All-blink pattern delay
#define SEQUENCE_DELAY_MS 100   // Sequential pattern delay
```

### Pattern Duration

Change pattern switching frequency:

```c
// Change pattern every 20 iterations (adjust as needed)
if (pattern_counter >= 20) {
    next_pattern();
}
```

### Logging Level

Modify in `prj.conf`:

```ini
# Log levels: 0=OFF, 1=ERROR, 2=WARNING, 3=INFO, 4=DEBUG
CONFIG_LOG_DEFAULT_LEVEL=3
```

## Troubleshooting

### Build Issues

1. **Zephyr not found**:
   ```bash
   export ZEPHYR_BASE=~/zephyrproject/zephyr
   source $ZEPHYR_BASE/zephyr-env.sh
   ```

2. **Board not supported**:
   ```bash
   west boards | grep nrf5340
   ```

3. **Dependencies missing**:
   ```bash
   pip3 install -r ~/zephyrproject/zephyr/scripts/requirements.txt
   ```

### Hardware Issues

1. **LEDs not working**:
   - Check power supply
   - Verify board connections
   - Test with simple blink program

2. **No serial output**:
   - Check UART settings (115200 baud)
   - Verify USB connection
   - Try different terminal emulator

3. **Flashing fails**:
   - Ensure board is in DFU mode
   - Check USB cable and connection
   - Try erasing flash first: `nrfjprog --family NRF53 --eraseall`

### Runtime Issues

1. **Application crashes**:
   - Check stack sizes in `prj.conf`
   - Enable debug logging
   - Use debugger: `west debug`

2. **LEDs behave unexpectedly**:
   - Verify device tree overlay
   - Check GPIO configuration
   - Review timing constants

## Advanced Features

### Adding New Patterns

1. Add new pattern to enum in `main.c`:
   ```c
   typedef enum {
       PATTERN_ALL_BLINK,
       PATTERN_SEQUENCE,
       PATTERN_CHASE,
       PATTERN_BOUNCE,
       PATTERN_YOUR_NEW_PATTERN,  // Add here
       PATTERN_MAX
   } led_pattern_t;
   ```

2. Implement pattern function:
   ```c
   static void pattern_your_new_pattern(void) {
       // Your pattern implementation
   }
   ```

3. Add to switch statement in `run_pattern()`

### Power Management

Uncomment in `prj.conf` to enable power management:

```ini
CONFIG_PM=y
CONFIG_PM_DEVICE=y
```

### Interactive Shell

Enable shell for runtime control:

```ini
CONFIG_SHELL=y
CONFIG_SHELL_BACKENDS=y
CONFIG_SHELL_BACKEND_SERIAL=y
```

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Submit a pull request

## License

This project is licensed under the Apache License 2.0. See the source files for license headers.

## Author

Murray Kopit - Initial implementation

## Acknowledgments

- Zephyr RTOS community
- Nordic Semiconductor for nRF5340 support
- ARM for development tools

---

*For more information about Zephyr RTOS, visit: https://zephyrproject.org/*  
*For nRF5340 documentation, visit: https://www.nordicsemi.com/Products/nRF5340*
