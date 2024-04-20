#ifndef _UART_HADILAO_H_
#define _UART_HADILAO_H_
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
#include <stdint.h>

#define EX_UART_NUM UART_NUM_0
#define PATTERN_CHR_NUM (3)

#define BUF_SIZE (1024)
#define RD_BUF_SIZE (BUF_SIZE)

typedef void (*uart_handle_t)(uint8_t *data, uint16_t length);
void uart_init(void);
void uart_set_callback(void *cb);
void uart_put(uint8_t *data, uint16_t len);



#endif
