#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_timer.h"

#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "lwip/ip4_addr.h"

#define WIFI_SSID "BT-K5A3MN_PHILS_EXT"
#define WIFI_PASS "H7DFmNGrhJ4kKN"

#include "neopixel.h"
#include "status.h"
#include "sensors.h"
#include "sht31.h"
#include "api.h"
#include "sntp_service.h"

#define NEOPIXEL_PIN GPIO_NUM_8
#define NEOPIXEL_COUNT 1
#define NEOPIXEL_BRIGHTNESS 0.05f
#define I2C_SDA_PIN GPIO_NUM_10
#define I2C_SCL_PIN GPIO_NUM_11

NeoPixel np;

static void system_post_wifi_task(void *arg)
{
    vTaskDelay(pdMS_TO_TICKS(1000));   // <--- ADD THIS

    sntp_service_init();

    sntp_wait_for_sync(60000); // 60 seconds (safe)

    api_start();

    vTaskDelete(NULL);
}

static void wifi_event_handler(void* arg,
                               esp_event_base_t event_base,
                               int32_t event_id,
                               void* event_data)
{
    if (event_base == WIFI_EVENT) {
        ESP_LOGI("WIFI", "WIFI_EVENT: %ld", event_id);
    }
    else if (event_base == IP_EVENT) {
        ESP_LOGI("WIFI", "IP_EVENT: %ld", event_id);
    }

    if (event_base == WIFI_EVENT) {

        if (event_id == WIFI_EVENT_STA_START) {
            ESP_LOGI("WIFI", "WiFi started, connecting...");
            esp_wifi_connect();
            status_set(STATUS_WIFI_CONNECTING);
        }

        else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
            ESP_LOGW("WIFI", "Disconnected! Retrying...");
            esp_wifi_connect();
            status_set(STATUS_WIFI_ERROR);
        }

    } else if (event_base == IP_EVENT) {

        if (event_id == IP_EVENT_STA_GOT_IP) {
            ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
            ESP_LOGI("WIFI", "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
            status_set(STATUS_OK);
            xTaskCreate(system_post_wifi_task, "post_wifi", 4096, NULL, 5, NULL);
            time_t now;
            time(&now);
            ESP_LOGI("TIME", "epoch = %lld", (long long)now);
        }
    }
}

void wifi_init()
{
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    esp_event_handler_instance_register(WIFI_EVENT,
                                        ESP_EVENT_ANY_ID,
                                        &wifi_event_handler,
                                        NULL,
                                        NULL);

    esp_event_handler_instance_register(IP_EVENT,
                                        IP_EVENT_STA_GOT_IP,
                                        &wifi_event_handler,
                                        NULL,
                                        NULL);
    
    
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    wifi_config_t wifi_config = {};
    strcpy((char*)wifi_config.sta.ssid, WIFI_SSID);
    strcpy((char*)wifi_config.sta.password, WIFI_PASS);

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);


    esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");

    if (netif == NULL) {
        ESP_LOGE("WIFI", "Failed to get netif!");
        return;
    }

    esp_netif_dhcpc_stop(netif);

    esp_netif_ip_info_t ip_info;
    esp_netif_str_to_ip4("192.168.1.30", &ip_info.ip);
    esp_netif_str_to_ip4("192.168.1.254", &ip_info.gw);
    esp_netif_str_to_ip4("255.255.255.0", &ip_info.netmask);

    esp_netif_set_ip_info(netif, &ip_info);

    esp_wifi_start();
    ESP_LOGI("WIFI", "esp_wifi_start() called");
}

extern "C" void app_main(void)
{
    ESP_LOGI("MAIN", "Starting...");

    // Required for WiFi
    esp_err_t ret = nvs_flash_init();
    if (ret != ESP_OK) {
        ESP_LOGE("MAIN", "NVS init failed");
        return;
    }
    
    neopixel_init(&np, NEOPIXEL_PIN, NEOPIXEL_COUNT);
    neopixel_set_brightness(&np, NEOPIXEL_BRIGHTNESS);

    sht31_init(I2C_SDA_PIN, I2C_SCL_PIN);
    sht31_start();

    sensors_init();
    sensors_start();
    
    status_init(&np);
    status_start();

    wifi_init();

    ESP_LOGI("MAIN", "Running...");

    while (1) {

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}