#pragma once
#include <stdint.h>

void sntp_service_init(void);
bool sntp_wait_for_sync(uint32_t timeout_ms);
bool sntp_is_synced(void);