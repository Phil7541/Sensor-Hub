idf_component_register(
    SRCS "api.cpp"
    INCLUDE_DIRS "include"
    REQUIRES esp_http_server esp_timer sensors sntp_service
)