/**
 * @file led.c
 * @author Kyuubi0323 (khoi.nv202647@sis.hust.edu.vn)
 * @brief 
 * @version 0.1
 * @date 2023-12-23
 * 
 * @copyright Copyright (c) 2023
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
#include "mqtt_client.h"
#include "driver/gpio.h"
#include "driver/gpio.h"

#include "output_hadilao.h"
#include "common_hadilao.h"
#include "input_hadilao.h"
#include "led_hadilao.h"

static const char *TAG = "LED";
//extern status_red_t status_red;
extern status_blue_t status_blue;


// void led_red_task(void *param)
// {
//     led_init(LED_STATUS_RED);
//     while (1)
//     {
//         switch (status_red)
//         {
//         case LOCAL_MODE:
//             gpio_set_level(LED_STATUS_RED, LED_ON);
//             vTaskDelay(100 / portTICK_PERIOD_MS);
//             gpio_set_level(LED_STATUS_RED, LED_OFF);
//             vTaskDelay(100 / portTICK_PERIOD_MS);
//             gpio_set_level(LED_STATUS_RED, LED_ON);
//             vTaskDelay(100 / portTICK_PERIOD_MS);
//             gpio_set_level(LED_STATUS_RED, LED_OFF);
//             vTaskDelay(1000 / portTICK_PERIOD_MS);
//             break;
//         case NORMAL_MODE:
//             gpio_set_level(LED_STATUS_RED, LED_ON);
//             vTaskDelay(100 / portTICK_PERIOD_MS);
//             break;
//         case CONFIG_MODE:
//             gpio_set_level(LED_STATUS_RED, LED_OFF);
//             vTaskDelay(100 / portTICK_PERIOD_MS);
//             break;
//         default:
//             gpio_set_level(LED_STATUS_RED, LED_OFF);
//             vTaskDelay(100 / portTICK_PERIOD_MS);
//             break;
//         }
//     }
// }

void led_blue_task(void *param)
{
    output_io_create(LED_STATUS_BLUE);
    while (1)
    {
        switch (status_blue)
        {
        case NORMAL_MODE:
            gpio_set_level(LED_STATUS_BLUE, LED_ON);
            vTaskDelay(500 / portTICK_PERIOD_MS);
            gpio_set_level(LED_STATUS_BLUE, LED_OFF);
            vTaskDelay(500 / portTICK_PERIOD_MS);
            break;
        case FOTA:
            gpio_set_level(LED_STATUS_BLUE, LED_ON);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            break;
        case WIFI_SOFTAP:
            gpio_set_level(LED_STATUS_BLUE, LED_ON);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            gpio_set_level(LED_STATUS_BLUE, LED_OFF);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            gpio_set_level(LED_STATUS_BLUE, LED_ON);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            gpio_set_level(LED_STATUS_BLUE, LED_OFF);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            gpio_set_level(LED_STATUS_BLUE, LED_ON);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            gpio_set_level(LED_STATUS_BLUE, LED_OFF);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            gpio_set_level(LED_STATUS_BLUE, LED_ON);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            gpio_set_level(LED_STATUS_BLUE, LED_OFF);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            gpio_set_level(LED_STATUS_BLUE, LED_ON);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            gpio_set_level(LED_STATUS_BLUE, LED_OFF);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            break;
        case NOT_STATE:
            gpio_set_level(LED_STATUS_BLUE, LED_OFF);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            break;
        default:
            gpio_set_level(LED_STATUS_BLUE, LED_OFF);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            break;
        }
    }
}