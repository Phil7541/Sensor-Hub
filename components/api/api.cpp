#include "api.h"
#include "esp_http_server.h"
#include "sensors.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "sntp_service.h"
#include <time.h>

#define TAG "API"

static uint64_t get_uptime_ms()
{
    return esp_timer_get_time() / 1000ULL;
}

static uint64_t get_timestamp()
{
    if (!sntp_is_synced()) {
        return 0;
    }

    return (uint64_t)time(NULL);
}

static esp_err_t health_get_handler(httpd_req_t *req)
{
    SensorMessage msg = sensors_get_sht31();

    char response[128];

    snprintf(response, sizeof(response),
        "{"
        "\"status\":\"ok\","
        "\"sensor_ok\":%s,"
        "\"uptime_ms\":%llu"
        "}",
        msg.valid ? "true" : "false",
        get_uptime_ms()
    );

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, response, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

static esp_err_t time_get_handler(httpd_req_t *req)
{
    time_t now;
    time(&now);

    char response[128];

    snprintf(response, sizeof(response),
        "{\"epoch\":%lld,\"synced\":%s}",
        (long long)now,
        sntp_is_synced() ? "true" : "false"
    );

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, response, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

static esp_err_t all_get_handler(httpd_req_t *req)
{
    SensorMessage s = sensors_get_sht31();

    char response[256];

    // Add time synced to all

    snprintf(response, sizeof(response),
        "{"
        "\"sht31\":{"
            "\"valid\":%s,"
            "\"temperature\":%.2f,"
            "\"humidity\":%.2f"
        "},"
        "\"uptime_ms\":%llu,"
        "\"timestamp\":%llu,"
        "\"synced\":%s"
        "}",
        s.valid ? "true" : "false",
        s.data.sht31.temperature,
        s.data.sht31.humidity,
        get_uptime_ms(),
        get_timestamp(),
        sntp_is_synced() ? "true" : "false"
    );

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, response, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

static esp_err_t sht31_get_handler(httpd_req_t *req)
{
    SensorMessage msg = sensors_get_sht31();

    char response[128];

    if (!msg.valid) {
        snprintf(response, sizeof(response),
                 "{\"status\":\"error\"}");
    } else {
        snprintf(response, sizeof(response),
                 "{\"temperature\":%.2f,\"humidity\":%.2f, \"uptime_ms\":%llu,\"timestamp\":%llu}",
                 msg.data.sht31.temperature,
                 msg.data.sht31.humidity,
                 get_uptime_ms(),
                 get_timestamp()
        );
    }

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, response, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

static esp_err_t temperature_get_handler(httpd_req_t *req)
{
    SensorMessage msg = sensors_get_sht31();

    char response[64];

    if (!msg.valid) {
        snprintf(response, sizeof(response),
                 "{\"status\":\"error\"}");
    } else {
        snprintf(response, sizeof(response),
                 "{\"temperature\":%.2f, \"uptime_ms\":%llu, \"timestamp\":%llu}",
                 msg.data.sht31.temperature,
                 get_uptime_ms(),
                 get_timestamp()
        );
    }

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, response, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

static esp_err_t humidity_get_handler(httpd_req_t *req)
{
    SensorMessage msg = sensors_get_sht31();

    char response[64];

    if (!msg.valid) {
        snprintf(response, sizeof(response),
                 "{\"status\":\"error\"}");
    } else {
        snprintf(response, sizeof(response),
                 "{\"humidity\":%.2f, \"uptime_ms\":%llu, \"timestamp\":%llu}",
                 msg.data.sht31.humidity,
                 get_uptime_ms(),
                 get_timestamp()
        );
    }

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, response, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

void api_start(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK) {

        httpd_uri_t health_uri = {
            .uri = "/health",
            .method = HTTP_GET,
            .handler = health_get_handler,
            .user_ctx = NULL
        };

        httpd_uri_t time_uri = {
            .uri = "/time",
            .method = HTTP_GET,
            .handler = time_get_handler,
            .user_ctx = NULL
        };

        httpd_uri_t all_uri = {
            .uri = "/all",
            .method = HTTP_GET,
            .handler = all_get_handler,
            .user_ctx = NULL
        };

        httpd_uri_t sht31_uri = {
            .uri = "/sht31",
            .method = HTTP_GET,
            .handler = sht31_get_handler,
            .user_ctx = NULL
        };

        httpd_uri_t temperature_uri = {
            .uri = "/temperature",
            .method = HTTP_GET,
            .handler = temperature_get_handler,
            .user_ctx = NULL
        };

        httpd_uri_t humidity_uri = {
            .uri = "/humidity",
            .method = HTTP_GET,
            .handler = humidity_get_handler,
            .user_ctx = NULL
        };
        
        httpd_register_uri_handler(server, &health_uri);
        httpd_register_uri_handler(server, &time_uri);
        httpd_register_uri_handler(server, &all_uri);
        httpd_register_uri_handler(server, &sht31_uri);
        httpd_register_uri_handler(server, &temperature_uri);
        httpd_register_uri_handler(server, &humidity_uri);
    }
}