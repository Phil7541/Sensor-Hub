#pragma once

#include "neopixel.h"

typedef enum {
    STATUS_OK,
    STATUS_WIFI_CONNECTING,
    STATUS_WIFI_ERROR,
    STATUS_SENSOR_ERROR
} StatusState;

// Init with neopixel instance
void status_init(NeoPixel* np);

// Start background task
void status_start(void);

void status_set(StatusState state);