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
#include "mqtt_client.h"

#include "driver/gpio.h"
#include "driver/uart.h"
#include "driver/spi_master.h"

//#include "web_hadilao.h"
#include "output_hadilao.h"
#include "input_hadilao.h"
#include "wifi_sta_hadilao.h"
#include "wifi_ap_hadilao.h"
#include "uart_hadilao.h"
#include "common_hadilao.h"
#include "mqtt_hadilao.h"
#include "led_hadilao.h"


#define LED 19
#define RELAY1 18
#define RELAY2 17
static const char *TAG = "MAIN";
char ssid[50] = {0};
char pwd[50] = {0};
// Pass Ezcast 0915423686



//RingbufHandle_t webserver_ring_buf;
extern esp_mqtt_client_handle_t client;
status_blue_t status_blue;
void app_main()
{   
    esp_err_t err;
    ESP_LOGI(TAG, "Initializing...");
    err = nvs_flash_init();

    if (err == ESP_ERR_NVS_NO_FREE_PAGES)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    ESP_ERROR_CHECK(err);
    /* chip's information*/
    chip_stats();
    /* init GPIO */
    output_io_create(LED);
    output_io_create(RELAY1);
    output_io_create(RELAY2);

    /* init wifi configuration*/
    //wifi_init();
    sprintf(ssid, "Kyuubi");
    sprintf(pwd, "laclac123");
    wifi_config_t wifi_config;
    bzero(&wifi_config, sizeof(wifi_config_t));
    
    memcpy(wifi_config.sta.ssid, ssid, strlen(ssid));
    memcpy(wifi_config.sta.password, pwd, strlen(pwd));
    
    //wifi_sta_start(wifi_config, WIFI_MODE_STA);
    //mqtt_client_start();
    output_io_toggle(RELAY1);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    output_io_toggle(RELAY1);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    while(1)
    {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        output_io_toggle(LED);
        
        ESP_LOGI(TAG, "LED TOGGLE");
    }
}


