idf_component_register(
    SRCS "sntp_service.cpp"
    INCLUDE_DIRS "include"
    REQUIRES esp_netif esp_event lwip
)