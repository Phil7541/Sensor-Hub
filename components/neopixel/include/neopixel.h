#pragma once

#include <stdint.h>
#include "driver/gpio.h"
#include "driver/rmt_tx.h"

typedef struct {
    gpio_num_t pin;
    int led_count;

    float brightness;

    uint8_t* buffer;
    uint8_t* raw_buffer;

    rmt_symbol_word_t* symbols;   // 👈 ADD THIS

    rmt_channel_handle_t channel;
    rmt_encoder_handle_t encoder;
} NeoPixel;

void neopixel_init(NeoPixel* np, gpio_num_t pin, int led_count);

void neopixel_set_color(NeoPixel* np, int index, uint8_t r, uint8_t g, uint8_t b);
void neopixel_fill(NeoPixel* np, uint8_t r, uint8_t g, uint8_t b);

void neopixel_set_brightness(NeoPixel* np, float brightness);

void neopixel_show(NeoPixel* np);