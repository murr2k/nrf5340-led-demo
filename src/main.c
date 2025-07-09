/*
 * Copyright (c) 2024 Murray Kopit
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

/* LED definitions for nRF5340 DK */
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

/* GPIO device and pin definitions */
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);
static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(LED3_NODE, gpios);

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
#define SEQUENCE_DELAY_MS 100

/* LED pattern states */
typedef enum {
    PATTERN_ALL_BLINK,
    PATTERN_SEQUENCE,
    PATTERN_CHASE,
    PATTERN_BOUNCE,
    PATTERN_MAX
} led_pattern_t;

static led_pattern_t current_pattern = PATTERN_ALL_BLINK;
static uint32_t pattern_counter = 0;

/**
 * @brief Initialize all LEDs
 * @return 0 on success, negative error code on failure
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
 * @brief Turn all LEDs on or off
 * @param state true to turn on, false to turn off
 */
static void set_all_leds(bool state)
{
    for (int i = 0; i < NUM_LEDS; i++) {
        gpio_pin_set_dt(&leds[i], state);
    }
}

/**
 * @brief Set individual LED state
 * @param led_num LED number (0-3)
 * @param state true to turn on, false to turn off
 */
static void set_led(int led_num, bool state)
{
    if (led_num >= 0 && led_num < NUM_LEDS) {
        gpio_pin_set_dt(&leds[led_num], state);
    }
}

/**
 * @brief All LEDs blink together pattern
 */
static void pattern_all_blink(void)
{
    static bool led_state = false;
    
    led_state = !led_state;
    set_all_leds(led_state);
    
    LOG_INF("All LEDs: %s", led_state ? "ON" : "OFF");
}

/**
 * @brief Sequential LED pattern (one after another)
 */
static void pattern_sequence(void)
{
    static int current_led = 0;
    
    set_all_leds(false);
    set_led(current_led, true);
    
    LOG_INF("LED %d ON", current_led);
    
    current_led = (current_led + 1) % NUM_LEDS;
}

/**
 * @brief Chase pattern (moving dot)
 */
static void pattern_chase(void)
{
    static int current_led = 0;
    static int direction = 1;
    
    set_all_leds(false);
    set_led(current_led, true);
    
    LOG_INF("Chase: LED %d ON", current_led);
    
    current_led += direction;
    
    if (current_led >= NUM_LEDS) {
        current_led = NUM_LEDS - 2;
        direction = -1;
    } else if (current_led < 0) {
        current_led = 1;
        direction = 1;
    }
}

/**
 * @brief Bounce pattern (back and forth)
 */
static void pattern_bounce(void)
{
    static int current_led = 0;
    static int direction = 1;
    static bool led_state = false;
    
    set_all_leds(false);
    
    // Light up LEDs from 0 to current_led
    for (int i = 0; i <= current_led; i++) {
        set_led(i, led_state);
    }
    
    LOG_INF("Bounce: LEDs 0-%d %s", current_led, led_state ? "ON" : "OFF");
    
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
        default:
            pattern_all_blink();
            break;
    }
}

/**
 * @brief Change to next LED pattern
 */
static void next_pattern(void)
{
    current_pattern = (current_pattern + 1) % PATTERN_MAX;
    pattern_counter = 0;
    set_all_leds(false); // Clear all LEDs when changing pattern
    
    const char *pattern_names[] = {
        "ALL_BLINK",
        "SEQUENCE", 
        "CHASE",
        "BOUNCE"
    };
    
    LOG_INF("Switched to pattern: %s", pattern_names[current_pattern]);
}

/**
 * @brief Main application entry point
 */
int main(void)
{
    int ret;
    
    LOG_INF("nRF5340 LED Demo Application Starting...");
    LOG_INF("Board: %s", CONFIG_BOARD);
    LOG_INF("Zephyr version: %s", KERNEL_VERSION_STRING);
    
    // Initialize LEDs
    ret = init_leds();
    if (ret < 0) {
        LOG_ERR("LED initialization failed: %d", ret);
        return ret;
    }
    
    // Startup sequence - flash all LEDs 3 times
    LOG_INF("Running startup sequence...");
    for (int i = 0; i < 3; i++) {
        set_all_leds(true);
        k_msleep(200);
        set_all_leds(false);
        k_msleep(200);
    }
    
    LOG_INF("Starting LED patterns...");
    
    // Main loop
    while (1) {
        run_pattern();
        
        // Change pattern every 20 iterations (10 seconds for most patterns)
        pattern_counter++;
        if (pattern_counter >= 20) {
            next_pattern();
        }
        
        // Different delays for different patterns
        switch (current_pattern) {
            case PATTERN_ALL_BLINK:
                k_msleep(BLINK_DELAY_MS);
                break;
            case PATTERN_SEQUENCE:
            case PATTERN_CHASE:
                k_msleep(SEQUENCE_DELAY_MS * 2);
                break;
            case PATTERN_BOUNCE:
                k_msleep(SEQUENCE_DELAY_MS);
                break;
            default:
                k_msleep(BLINK_DELAY_MS);
                break;
        }
    }
    
    return 0;
}
