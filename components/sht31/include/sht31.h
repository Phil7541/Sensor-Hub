#pragma once

#include "driver/gpio.h"
#include <stdbool.h>

typedef struct {
    float temperature;
    float humidity;
    bool valid;
} SensorData;

void sht31_init(gpio_num_t sda_pin, gpio_num_t scl_pin);
void sht31_start(void);
void sht31_update(void);
SensorData sht31_get_data(void);