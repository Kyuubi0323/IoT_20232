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

#include "web_hadilao.h"
#include "output_hadilao.h"
#include "input_hadilao.h"
#include "wifi_sta_hadilao.h"
#include "wifi_ap_hadilao.h"
#include "uart_hadilao.h"


#define LED 2
static const char *TAG = "MAIN";
char ssid[50] = {0};
char pwd[50] = {0};

//RingbufHandle_t webserver_ring_buf;
 

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
    //chip_stats();
    /* init GPIO */
    output_io_create(LED);
    /* init wifi configuration*/
    wifi_init();
    sprintf(ssid, "SonHa");
    sprintf(pwd, "07052003");
    wifi_config_t wifi_config;
    bzero(&wifi_config, sizeof(wifi_config_t));
    memcpy(wifi_config.sta.ssid, ssid, strlen(ssid));
    memcpy(wifi_config.sta.password, pwd, strlen(pwd));
    
    //wifi_sta_start(wifi_config, WIFI_MODE_STA);
   wifi_init_softap();
   
    start_webserver();
    
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    
    while(1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        output_io_toggle(LED);
        //ESP_LOGI(TAG, "TOGGLE");
    }
    
}