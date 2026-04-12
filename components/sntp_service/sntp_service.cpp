#include "sntp_service.h"

#include "esp_log.h"
#include "esp_sntp.h"
#include "esp_netif_sntp.h"
#include <time.h>

static const char *TAG = "SNTP";

static bool time_synced = false;

static void time_sync_cb(struct timeval *tv)
{
    ESP_LOGI(TAG, "TIME SYNC CALLBACK FIRED");
    time_synced = true;
}

void sntp_service_init(void)
{
    ESP_LOGI(TAG, "Starting SNTP...");

    setenv("TZ", "GMT0BST,M3.5.0/1,M10.5.0", 1);
    tzset();

    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    ip_addr_t server_addr;
    ip4_addr_set_u32(&server_addr.u_addr.ip4, ipaddr_addr("129.6.15.28"));
    server_addr.type = IPADDR_TYPE_V4;

    esp_sntp_setserver(0, &server_addr);

    // REGISTER CALLBACK (THIS IS THE KEY LINE)
    esp_sntp_set_time_sync_notification_cb(time_sync_cb);

    esp_sntp_init();

    ESP_LOGI(TAG, "SNTP initialized");
}

bool sntp_wait_for_sync(uint32_t timeout_ms)
{
    uint32_t waited = 0;

    while (waited < timeout_ms)
    {
        if (time_synced)
        {
            ESP_LOGI(TAG, "Time synced!");
            return true;
        }

        ESP_LOGI(TAG, "Waiting for SNTP...");
        vTaskDelay(pdMS_TO_TICKS(1000));
        waited += 1000;
    }

    ESP_LOGW(TAG, "SNTP sync timeout");
    return false;
}

bool sntp_is_synced(void)
{
    return time_synced;
}