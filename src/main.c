/*
 * Copyright (c) 2024 Murray Kopit
 * nRF5340 LED Demo with Serial Commands
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>
#include <string.h>
#include <stdlib.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

/* LED definitions for nRF5340 DK */
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

/* LED array for easier management */
static const struct gpio_dt_spec leds[] = {
    GPIO_DT_SPEC_GET(LED0_NODE, gpios),
    GPIO_DT_SPEC_GET(LED1_NODE, gpios),
    GPIO_DT_SPEC_GET(LED2_NODE, gpios),
    GPIO_DT_SPEC_GET(LED3_NODE, gpios)
};

#define NUM_LEDS ARRAY_SIZE(leds)

/* Timing configuration */
#define BLINK_DELAY_MS 500
#define SEQUENCE_DELAY_MS 200

/* LED pattern states */
typedef enum {
    PATTERN_ALL_BLINK,
    PATTERN_SEQUENCE,
    PATTERN_CHASE,
    PATTERN_BOUNCE,
    PATTERN_CUSTOM,
    PATTERN_MAX
} led_pattern_t;

static led_pattern_t current_pattern = PATTERN_ALL_BLINK;
static uint32_t pattern_counter = 0;
static bool auto_mode = true;
static uint8_t custom_led_state = 0x0F; // All LEDs on by default

/* UART console device */
static const struct device *const uart_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));

/* Command buffer */
#define CMD_BUFFER_SIZE 32
static char cmd_buffer[CMD_BUFFER_SIZE];
static uint8_t cmd_idx = 0;

/**
 * @brief Initialize all LEDs
 */
static int init_leds(void)
{
    int ret;
    
    for (int i = 0; i < NUM_LEDS; i++) {
        if (!gpio_is_ready_dt(&leds[i])) {
            LOG_ERR("LED %d GPIO device not ready", i);
            return -ENODEV;
        }
        
        ret = gpio_pin_configure_dt(&leds[i], GPIO_OUTPUT_INACTIVE);
        if (ret < 0) {
            LOG_ERR("Cannot configure LED %d GPIO (err %d)", i, ret);
            return ret;
        }
    }
    
    LOG_INF("All LEDs initialized successfully");
    return 0;
}

/**
 * @brief Set individual LED state
 */
static void set_led(int led_num, bool state)
{
    if (led_num >= 0 && led_num < NUM_LEDS) {
        gpio_pin_set_dt(&leds[led_num], state);
    }
}

/**
 * @brief Set all LEDs based on bitmask
 */
static void set_leds_mask(uint8_t mask)
{
    for (int i = 0; i < NUM_LEDS; i++) {
        set_led(i, (mask >> i) & 1);
    }
}

/**
 * @brief Turn all LEDs on or off
 */
static void set_all_leds(bool state)
{
    set_leds_mask(state ? 0x0F : 0x00);
}

/**
 * @brief LED Pattern Functions
 */
static void pattern_all_blink(void)
{
    static bool led_state = false;
    led_state = !led_state;
    set_all_leds(led_state);
}

static void pattern_sequence(void)
{
    static int current_led = 0;
    set_all_leds(false);
    set_led(current_led, true);
    current_led = (current_led + 1) % NUM_LEDS;
}

static void pattern_chase(void)
{
    static int current_led = 0;
    static int direction = 1;
    
    set_all_leds(false);
    set_led(current_led, true);
    
    current_led += direction;
    if (current_led >= NUM_LEDS) {
        current_led = NUM_LEDS - 2;
        direction = -1;
    } else if (current_led < 0) {
        current_led = 1;
        direction = 1;
    }
}

static void pattern_bounce(void)
{
    static int current_led = 0;
    static int direction = 1;
    static bool led_state = false;
    
    set_all_leds(false);
    for (int i = 0; i <= current_led; i++) {
        set_led(i, led_state);
    }
    
    led_state = !led_state;
    if (!led_state) {
        current_led += direction;
        if (current_led >= NUM_LEDS) {
            current_led = NUM_LEDS - 2;
            direction = -1;
        } else if (current_led < 0) {
            current_led = 1;
            direction = 1;
        }
    }
}

static void pattern_custom(void)
{
    set_leds_mask(custom_led_state);
}

/**
 * @brief Execute current LED pattern
 */
static void run_pattern(void)
{
    switch (current_pattern) {
        case PATTERN_ALL_BLINK:
            pattern_all_blink();
            break;
        case PATTERN_SEQUENCE:
            pattern_sequence();
            break;
        case PATTERN_CHASE:
            pattern_chase();
            break;
        case PATTERN_BOUNCE:
            pattern_bounce();
            break;
        case PATTERN_CUSTOM:
            pattern_custom();
            break;
        default:
            pattern_all_blink();
            break;
    }
}

/**
 * @brief Process received command
 */
static void process_command(const char *cmd)
{
    LOG_INF("Command received: %s", cmd);
    
    if (strcmp(cmd, "help") == 0) {
        printk("\n=== nRF5340 LED Demo Commands ===\n");
        printk("help          - Show this help\n");
        printk("auto          - Enable auto pattern cycling\n");
        printk("manual        - Disable auto pattern cycling\n");
        printk("pattern <0-4> - Set pattern (0=blink, 1=sequence, 2=chase, 3=bounce, 4=custom)\n");
        printk("led <0-3> <0-1> - Set individual LED (led 2 1 = turn on LED 2)\n");
        printk("leds <mask>   - Set LEDs by bitmask (leds 5 = LEDs 0,2 on)\n");
        printk("all <0-1>     - Turn all LEDs on/off\n");
        printk("status        - Show current status\n");
        printk("===============================\n");
        
    } else if (strcmp(cmd, "auto") == 0) {
        auto_mode = true;
        pattern_counter = 0;
        LOG_INF("Auto mode enabled");
        
    } else if (strcmp(cmd, "manual") == 0) {
        auto_mode = false;
        LOG_INF("Manual mode enabled");
        
    } else if (strncmp(cmd, "pattern ", 8) == 0) {
        int pattern = atoi(&cmd[8]);
        if (pattern >= 0 && pattern < PATTERN_MAX) {
            current_pattern = pattern;
            pattern_counter = 0;
            auto_mode = false;
            
            const char *pattern_names[] = {"BLINK", "SEQUENCE", "CHASE", "BOUNCE", "CUSTOM"};
            LOG_INF("Pattern set to: %s", pattern_names[pattern]);
        } else {
            LOG_ERR("Invalid pattern number: %d (0-%d)", pattern, PATTERN_MAX-1);
        }
        
    } else if (strncmp(cmd, "led ", 4) == 0) {
        // Simple parsing: "led X Y" where X is LED number, Y is state
        const char *args = &cmd[4];
        int led_num = atoi(args);
        // Find space after first number
        while (*args && *args != ' ') args++;
        if (*args == ' ') {
            args++;
            int state = atoi(args);
            if (led_num >= 0 && led_num < NUM_LEDS && (state == 0 || state == 1)) {
                set_led(led_num, state);
                current_pattern = PATTERN_CUSTOM;
                auto_mode = false;
                LOG_INF("LED %d set to %s", led_num, state ? "ON" : "OFF");
            } else {
                LOG_ERR("Invalid LED parameters (led: 0-%d, state: 0-1)", NUM_LEDS-1);
            }
        } else {
            LOG_ERR("Invalid LED command format (use: led <0-3> <0-1>)");
        }
        
    } else if (strncmp(cmd, "leds ", 5) == 0) {
        int mask = atoi(&cmd[5]);
        if (mask >= 0 && mask <= 15) {
            custom_led_state = mask;
            set_leds_mask(mask);
            current_pattern = PATTERN_CUSTOM;
            auto_mode = false;
            LOG_INF("LEDs set to mask: 0x%02X", mask);
        } else {
            LOG_ERR("Invalid LED mask: %d (0-15)", mask);
        }
        
    } else if (strncmp(cmd, "all ", 4) == 0) {
        int state = atoi(&cmd[4]);
        if (state == 0 || state == 1) {
            set_all_leds(state);
            current_pattern = PATTERN_CUSTOM;
            custom_led_state = state ? 0x0F : 0x00;
            auto_mode = false;
            LOG_INF("All LEDs set to %s", state ? "ON" : "OFF");
        } else {
            LOG_ERR("Invalid state: %d (0 or 1)", state);
        }
        
    } else if (strcmp(cmd, "status") == 0) {
        const char *pattern_names[] = {"BLINK", "SEQUENCE", "CHASE", "BOUNCE", "CUSTOM"};
        printk("\n=== Status ===\n");
        printk("Mode: %s\n", auto_mode ? "Auto" : "Manual");
        printk("Pattern: %s (%d)\n", pattern_names[current_pattern], current_pattern);
        printk("LED States: ");
        for (int i = 0; i < NUM_LEDS; i++) {
            printk("LED%d=%d ", i, (custom_led_state >> i) & 1);
        }
        printk("\n==============\n");
        
    } else {
        LOG_ERR("Unknown command: %s (type 'help' for commands)", cmd);
    }
}

/**
 * @brief UART callback for receiving characters
 */
static void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{
    static uint8_t rx_char;
    
    switch (evt->type) {
    case UART_RX_RDY:
        if (evt->data.rx.len == 1) {
            rx_char = evt->data.rx.buf[0];
            
            if (rx_char == '\r' || rx_char == '\n') {
                if (cmd_idx > 0) {
                    cmd_buffer[cmd_idx] = '\0';
                    process_command(cmd_buffer);
                    cmd_idx = 0;
                }
            } else if (rx_char == '\b' || rx_char == 127) { // Backspace
                if (cmd_idx > 0) {
                    cmd_idx--;
                    uart_tx(dev, "\b \b", 3, SYS_FOREVER_MS);
                }
            } else if (cmd_idx < CMD_BUFFER_SIZE - 1) {
                cmd_buffer[cmd_idx++] = rx_char;
                uart_tx(dev, &rx_char, 1, SYS_FOREVER_MS); // Echo
            }
        }
        uart_rx_enable(dev, &rx_char, 1, SYS_FOREVER_MS);
        break;
        
    case UART_RX_DISABLED:
        uart_rx_enable(dev, &rx_char, 1, SYS_FOREVER_MS);
        break;
        
    default:
        break;
    }
}

/**
 * @brief Main application entry point
 */
int main(void)
{
    int ret;
    uint8_t rx_char;
    
    LOG_INF("nRF5340 LED Demo with Commands Starting...");
    LOG_INF("Board: %s", CONFIG_BOARD);
    
    // Initialize LEDs
    ret = init_leds();
    if (ret < 0) {
        LOG_ERR("LED initialization failed: %d", ret);
        return ret;
    }
    
    // Initialize UART for commands
    if (!device_is_ready(uart_dev)) {
        LOG_ERR("UART device not ready");
        return -ENODEV;
    }
    
    uart_callback_set(uart_dev, uart_cb, NULL);
    uart_rx_enable(uart_dev, &rx_char, 1, SYS_FOREVER_MS);
    
    // Startup sequence
    LOG_INF("Running startup sequence...");
    for (int i = 0; i < 3; i++) {
        set_all_leds(true);
        k_msleep(200);
        set_all_leds(false);
        k_msleep(200);
    }
    
    printk("\n=== nRF5340 LED Demo Ready ===\n");
    printk("Type 'help' for available commands\n");
    printk("Auto mode enabled (patterns change every 10s)\n");
    printk("==============================\n");
    
    // Main loop
    while (1) {
        if (auto_mode) {
            run_pattern();
            
            // Change pattern every 50 iterations (~10 seconds)
            pattern_counter++;
            if (pattern_counter >= 50) {
                current_pattern = (current_pattern + 1) % (PATTERN_MAX - 1); // Skip CUSTOM
                pattern_counter = 0;
                set_all_leds(false);
                
                const char *pattern_names[] = {"BLINK", "SEQUENCE", "CHASE", "BOUNCE"};
                LOG_INF("Auto switched to: %s", pattern_names[current_pattern]);
            }
        } else {
            run_pattern();
        }
        
        // Different delays for different patterns
        switch (current_pattern) {
            case PATTERN_ALL_BLINK:
                k_msleep(BLINK_DELAY_MS);
                break;
            case PATTERN_SEQUENCE:
            case PATTERN_CHASE:
                k_msleep(SEQUENCE_DELAY_MS);
                break;
            case PATTERN_BOUNCE:
                k_msleep(SEQUENCE_DELAY_MS / 2);
                break;
            case PATTERN_CUSTOM:
                k_msleep(100); // Fast refresh for manual control
                break;
            default:
                k_msleep(BLINK_DELAY_MS);
                break;
        }
    }
    
    return 0;
}