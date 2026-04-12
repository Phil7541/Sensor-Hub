#include "sensors.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "status.h"

#define TAG "SENSORS"
#define QUEUE_SIZE 10

static QueueHandle_t sensor_queue;

// latest values (cached)
static SensorMessage sht31_latest;

void sensors_init(void)
{
    sensor_queue = xQueueCreate(QUEUE_SIZE, sizeof(SensorMessage));

    if (!sensor_queue) {
        ESP_LOGE(TAG, "Failed to create sensor queue");
    }

    sht31_latest.valid = false;
}

// This task processes ALL incoming sensor data
void sensors_task(void* arg)
{
    SensorMessage msg;

    while (1) {
        if (xQueueReceive(sensor_queue, &msg, portMAX_DELAY)) {

            if (!msg.valid) {
                status_set(STATUS_SENSOR_ERROR);
            }

            switch (msg.type) {

                case SENSOR_TYPE_SHT31:
                    sht31_latest = msg;
                    break;

                default:
                    ESP_LOGW(TAG, "Unknown sensor type");
                    break;
            }
        }
    }
}

void sensors_start(void)
{
    xTaskCreate(sensors_task, "sensors_task", 4096, NULL, 5, NULL);
}

bool sensors_publish(SensorMessage* msg)
{
    if (!sensor_queue) return false;

    return xQueueSend(sensor_queue, msg, 0) == pdTRUE;
}

SensorMessage sensors_get_sht31(void)
{
    return sht31_latest;
}