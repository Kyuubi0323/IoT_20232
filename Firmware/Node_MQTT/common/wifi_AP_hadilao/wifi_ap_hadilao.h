#ifndef _WIFI_AP_HADILAO_H_
#define _WIFI_AP_HADILAO_H_

/**
 * @file wifi_ap_hadilao.h
 * @Kyuubi {Kyuubi} ({khoi.nv0323.work@gmail.com})
 * @brief 
 * @version 0.1
 * @date 2024-04-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/ringbuf.h"
#include "freertos/semphr.h"

#include "nvs_flash.h"

#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_smartconfig.h"
#include "esp_spiffs.h"
#include "esp_http_client.h"
#include "esp_attr.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "esp_http_server.h"
#include "esp_mac.h"
#include "esp_chip_info.h"

#include "driver/gpio.h"
#include "driver/uart.h"
#include "driver/spi_master.h"

#include <math.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define WIFI_AP_SSID "MANDevices"
#define WIFI_AP_CHANNEL 1
#define WIFI_AP_MAX_CONN 2

void wifi_init_softap(void);




#endif
