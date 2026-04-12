#include "sht31.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sensors.h"

#define TAG "SHT31"

#define I2C_PORT I2C_NUM_0
#define SHT31_ADDR 0x44

static i2c_master_dev_handle_t sht31_dev;
static SensorData data;

void sht31_init(gpio_num_t sda_pin, gpio_num_t scl_pin)
{
    i2c_master_bus_config_t bus_config = {};
    bus_config.i2c_port = I2C_PORT;
    bus_config.sda_io_num = sda_pin;
    bus_config.scl_io_num = scl_pin;
    bus_config.clk_source = I2C_CLK_SRC_DEFAULT;

    i2c_master_bus_handle_t bus;
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &bus));

    i2c_device_config_t dev_config = {};
    dev_config.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    dev_config.device_address = SHT31_ADDR;
    dev_config.scl_speed_hz = 100000;

    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus, &dev_config, &sht31_dev));

    data.valid = false;

    ESP_LOGI(TAG, "Init OK (SDA=%d, SCL=%d)", sda_pin, scl_pin);
}

void sht31_task(void* arg)
{
    while (1) {
        sht31_update();

        SensorData d = sht31_get_data();

        SensorMessage msg = {};
        msg.type = SENSOR_TYPE_SHT31;
        msg.valid = d.valid;

        if (d.valid) {
            msg.data.sht31.temperature = d.temperature;
            msg.data.sht31.humidity = d.humidity;
        }

        sensors_publish(&msg);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void sht31_start(void)
{
    xTaskCreate(sht31_task, "sht31_task", 2048, NULL, 5, NULL);
}

void sht31_update(void)
{
    const int max_retries = 3;
    uint8_t cmd[2] = {0x24, 0x00};
    uint8_t buffer[6];

    esp_err_t err = ESP_FAIL;

    for (int i = 0; i < max_retries; i++) {

        err = i2c_master_transmit_receive(
            sht31_dev,
            cmd, sizeof(cmd),
            buffer, sizeof(buffer),
            100
        );

        if (err == ESP_OK) break;

        ESP_LOGW(TAG, "Read failed (attempt %d)", i + 1);
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    if (err != ESP_OK) {
        data.valid = false;
        return;
    }

    uint16_t raw_temp = (buffer[0] << 8) | buffer[1];
    uint16_t raw_hum  = (buffer[3] << 8) | buffer[4];

    data.temperature = -45.0f + 175.0f * ((float)raw_temp / 65535.0f);
    data.humidity    = 100.0f * ((float)raw_hum / 65535.0f);

    data.valid = true;
}

SensorData sht31_get_data(void)
{
    return data;
}