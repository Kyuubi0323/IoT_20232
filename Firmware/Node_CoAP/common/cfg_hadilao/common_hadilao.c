/**
 * @file common.c
 * @author Kyuubi0323 (khoi.nv202647@sis.hust.edu.vn)
 * @brief 
 * @version 0.1
 * @date 2023-12-23
 * 
 * @copyright Copyright (c) 2023

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
#include "esp_spiffs.h"
#include "esp_http_client.h"
#include "esp_attr.h"
#include "esp_http_server.h"

#include "driver/gpio.h"
#include "driver/uart.h"
#include "driver/spi_master.h"

#include "common_hadilao.h"
#include "cJSON.h"

static const char *TAG = "COMMON";

esp_err_t mqtt_parse_data(char *mqtt_data, mqtt_obj_t *mqtt_obj)
{
    cJSON *root = cJSON_Parse(mqtt_data);
    if (root == NULL)
        return ESP_FAIL;
    cJSON *cur_elem = NULL;
    cJSON_ArrayForEach(cur_elem, root)
    {
        if (cur_elem->string)
        {
            const char *cur_str = cur_elem->string;
            if (strcmp(cur_str, "id") == 0)
                mqtt_obj->id = cur_elem->valueint;
            else if (strcmp(cur_str, "version") == 0)
                memcpy(mqtt_obj->version, cur_elem->valuestring, strlen(cur_elem->valuestring) + 1);
            else if (strcmp(cur_str, "temp") == 0)
                mqtt_obj->temp = cur_elem->valueint;
            else if (strcmp(cur_str, "humid") == 0)
                mqtt_obj->humid = cur_elem->valueint;
            else if (strcmp(cur_str, "light") == 0)
                mqtt_obj->LDR_value = cur_elem->valueint;
        }
    }
    cJSON_Delete(root);
    return ESP_OK;
}

void json_crt_obj(void)
{ 
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "id", 1);
    cJSON_AddStringToObject(root, "version", "1.0");
    cJSON_AddNumberToObject(root, "temp", 30);
    cJSON_AddNumberToObject(root, "humid", 50);
    cJSON_AddNumberToObject(root, "light", 100);
    char *json_str = cJSON_Print(root);
    cJSON_Delete(root);
    free(json_str);
}