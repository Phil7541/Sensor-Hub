#include "freertos/FreeRTOS.h"
#include "neopixel.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include <string.h>
#include <stdlib.h>

#define TAG "NEOPIXEL"

// WS2812 timing (in nanoseconds)
#define T0H  350
#define T0L  800
#define T1H  700
#define T1L  600

// RMT resolution (10MHz = 0.1us per tick)
#define RMT_RESOLUTION_HZ 10000000

static rmt_symbol_word_t make_bit(bool bit)
{
    rmt_symbol_word_t symbol;

    if (bit) {
        symbol.level0 = 1;
        symbol.duration0 = T1H / 100;
        symbol.level1 = 0;
        symbol.duration1 = T1L / 100;
    } else {
        symbol.level0 = 1;
        symbol.duration0 = T0H / 100;
        symbol.level1 = 0;
        symbol.duration1 = T0L / 100;
    }

    return symbol;
}

void neopixel_init(NeoPixel* np, gpio_num_t pin, int led_count)
{
    np->pin = pin;
    np->led_count = led_count;

    np->brightness = 1.0f; // default full brightness

    // Allocate buffer (GRB per LED)
    np->buffer = (uint8_t*) malloc(led_count * 3);
    np->raw_buffer = (uint8_t*) malloc(led_count * 3);

    memset(np->buffer, 0, led_count * 3);
    memset(np->raw_buffer, 0, led_count * 3);

    // Configure RMT TX channel
    rmt_tx_channel_config_t config = {};
    config.gpio_num = pin;
    config.clk_src = RMT_CLK_SRC_DEFAULT;
    config.resolution_hz = RMT_RESOLUTION_HZ;
    config.mem_block_symbols = 64;
    config.trans_queue_depth = 4;
    config.intr_priority = 0;
    config.flags = {};

    ESP_ERROR_CHECK(rmt_new_tx_channel(&config, &np->channel));
    ESP_ERROR_CHECK(rmt_enable(np->channel));

    rmt_copy_encoder_config_t encoder_config = {};
    ESP_ERROR_CHECK(rmt_new_copy_encoder(&encoder_config, &np->encoder));

    int total_bits = led_count * 24;
    np->symbols = (rmt_symbol_word_t*) malloc(sizeof(rmt_symbol_word_t) * total_bits);
}

void neopixel_set_color(NeoPixel* np, int index, uint8_t r, uint8_t g, uint8_t b)
{
    if (index >= np->led_count) return;

    np->raw_buffer[index * 3 + 0] = g;
    np->raw_buffer[index * 3 + 1] = r;
    np->raw_buffer[index * 3 + 2] = b;
}

void neopixel_fill(NeoPixel* np, uint8_t r, uint8_t g, uint8_t b)
{
    for (int i = 0; i < np->led_count; i++) {
        neopixel_set_color(np, i, r, g, b);
    }
}

void neopixel_show(NeoPixel* np)
{
    int total_bits = np->led_count * 24;

    for (int i = 0; i < np->led_count * 3; i++) {
        np->buffer[i] = (uint8_t)(np->raw_buffer[i] * np->brightness);
    }

    int idx = 0;

    for (int i = 0; i < np->led_count * 3; i++) {
        uint8_t byte = np->buffer[i];

        for (int bit = 7; bit >= 0; bit--) {
            bool bit_val = (byte >> bit) & 0x01;
            np->symbols[idx++] = make_bit(bit_val);
        }
    }

    rmt_transmit_config_t tx_config = {};
    tx_config.loop_count = 0;

    ESP_ERROR_CHECK(rmt_transmit(
        np->channel,
        np->encoder,
        np->symbols,
        sizeof(rmt_symbol_word_t) * total_bits,
        &tx_config
    ));

    ESP_ERROR_CHECK(rmt_tx_wait_all_done(np->channel, portMAX_DELAY));

    esp_rom_delay_us(60);
}

void neopixel_set_brightness(NeoPixel* np, float brightness)
{
    if (brightness < 0.0f) brightness = 0.0f;
    if (brightness > 1.0f) brightness = 1.0f;

    np->brightness = brightness;
}