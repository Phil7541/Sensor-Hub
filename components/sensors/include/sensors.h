#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    SENSOR_TYPE_SHT31,
    // future:
    // SENSOR_TYPE_BH1750,
} SensorType;

typedef struct {
    SensorType type;

    union {
        struct {
            float temperature;
            float humidity;
        } sht31;
    } data;

    bool valid;
} SensorMessage;

// Init system
void sensors_init(void);
void sensors_start(void);

// Push data into system (called by sensor drivers)
bool sensors_publish(SensorMessage* msg);

// Get latest snapshot (for API)
SensorMessage sensors_get_sht31(void);