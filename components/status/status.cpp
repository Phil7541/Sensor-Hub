#include "status.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sensors.h"
#include "esp_log.h"

#define TAG "STATUS"

static NeoPixel* np_ptr = NULL;
static StatusState current_state = STATUS_WIFI_CONNECTING;

static void status_task(void* arg)
{
    while (1) {

        switch (current_state) {

            // Neopixel colors are in GRB order

            case STATUS_OK:
                neopixel_set_color(np_ptr, 0, 255, 0, 0); // green
                break;

            case STATUS_WIFI_CONNECTING:
                neopixel_set_color(np_ptr, 0, 255, 255, 0); // yellow
                break;

            case STATUS_WIFI_ERROR:
                neopixel_set_color(np_ptr, 0, 0, 255, 0); // red
                break;

            case STATUS_SENSOR_ERROR:
                neopixel_set_color(np_ptr, 0, 165, 255, 0); // orange
                break;
        }

        neopixel_show(np_ptr);

        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void status_init(NeoPixel* np)
{
    np_ptr = np;
}

void status_start(void)
{
    xTaskCreate(status_task, "status_task", 2048, NULL, 4, NULL);
}

void status_set(StatusState state)
{
    current_state = state;
}