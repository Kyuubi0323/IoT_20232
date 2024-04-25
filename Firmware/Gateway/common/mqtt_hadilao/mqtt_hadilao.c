/**
 * @file mqtt.c
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
#include "inttypes.h"
#include "nvs_flash.h"
#include "esp_tls.h"
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
#include "mqtt_hadilao.h"
#include "cJSON.h"


static const char *TAG = "MQTT";
RingbufHandle_t mqtt_ring_buf;
esp_mqtt_client_handle_t client;
char version[10];

extern status_blue_t status_blue;
char topic_commands_data[20] = "mandevices/response";
char topic_commands_upgrade[20] = "mandevices/receive";
node_t _sensor;


void cJSON_mqtt_handler(void *event_data)
{   esp_mqtt_event_handle_t event = event_data;
    client = event->client;
    _sensor.node_id = 1;
    memset(_sensor.ver, 0, 6);
    strcpy(_sensor.ver, "1.0");
    _sensor.temp = 40;
    _sensor.humid = 86;
    _sensor.LDR_value = 120;
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "id", _sensor.node_id);
    cJSON_AddStringToObject(root, "version", _sensor.ver);
    cJSON_AddNumberToObject(root, "temp", _sensor.temp);
    cJSON_AddNumberToObject(root, "humid", _sensor.humid);
    cJSON_AddNumberToObject(root, "light", _sensor.LDR_value);
    char *payload = cJSON_Print(root);
    esp_mqtt_client_publish(client, topic_commands_data, payload, 0, 1, 1);
    cJSON_Delete(root);
    if (payload != NULL)
    free(payload);

}
void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    client = event->client;
    
    switch ((esp_mqtt_event_id_t) event_id)
    {
    case MQTT_EVENT_CONNECTED:
    {
        ESP_LOGI(TAG, "MQTT event connected");
        //esp_mqtt_client_subscribe(client, topic_commands_data, 0);
        esp_mqtt_client_subscribe(client, topic_commands_upgrade, 0);
        cJSON_mqtt_handler(event);
        break;
    }
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT event disconnected");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT event subcribed, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT event unsubcrib ed, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT event published, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
    {   
        UBaseType_t res = xRingbufferSend(mqtt_ring_buf, event->data, event->data_len, portMAX_DELAY);
        if(res != pdTRUE)
            ESP_LOGE(TAG, "Failed to send item\n");
        char *mess_recv;
        size_t mess_size = 0;
        mess_recv = (char*)xRingbufferReceive(mqtt_ring_buf, &mess_size, portMAX_DELAY);
        mess_recv[mess_size] = '\0';
        printf("DATA %s\n", mess_recv);
        break;
    }
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT event error");
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}
void mqtt_client_task(void *param)
{
    char *mess_recv = NULL;
    size_t mess_size = 0;
    mqtt_obj_t mqtt_obj;
    while(1)
    {
        mess_recv = (char*)xRingbufferReceive(mqtt_ring_buf, &mess_size, portMAX_DELAY);
        ESP_LOGI(TAG, "New_data");
        if (mess_recv)
        {
            mess_recv[mess_size] = '\0';
            ESP_LOGI(TAG, "Recv payload: %s", mess_recv);
            memset(&mqtt_obj, 0, sizeof(mqtt_obj));
            mqtt_parse_data(mess_recv, &mqtt_obj);
            if (strcmp(mqtt_obj.action, "set") == 0)
            {   
                mqtt_obj.dev_state = 1 - mqtt_obj.dev_state;
                ESP_LOGI(TAG, "Set state %s", mqtt_obj.dev_state ? "ON":"OFF");
            }
            if (strcmp(mqtt_obj.action, "get") == 0)
            {
               ESP_LOGI(TAG, "Get state");
            }
            else if (strcmp(mqtt_obj.action, "upgrade") == 0)
            {
                ESP_LOGI(TAG, "Upgrade firmware");
                //uint32_t free_heap_size = 0, min_free_heap_size = 0;
                //free_heap_size = esp_get_free_heap_size();
                //min_free_heap_size = esp_get_minimum_free_heap_size();
                //ESP_LOGW(TAG, "Free heap size = %d, Min free heap size = %d", free_heap_size, min_free_heap_size);
                //ble_mesh_deinit();
                //xTaskCreate(&fota_task, "fota_task", 8192, mqtt_obj.url, 8, NULL);
            }
            else if (strcmp(mqtt_obj.action, "open") == 0)
            {
                ESP_LOGI(TAG, "here is open");
                
            }
            else if (strcmp(mqtt_obj.action, "close") == 0)
            {
                ESP_LOGI(TAG, "Close");
                status_blue = NOT_STATE;
            }
            else if (strcmp(mqtt_obj.action, "delete") == 0)
            {
                ESP_LOGI(TAG, "Close");
            }
            else if (strcmp(mqtt_obj.action, "set_timeout") == 0)
            {
                ESP_LOGI(TAG, "Close");
            }
            else if (strcmp(mqtt_obj.action, "cfg") == 0)
            {
                ESP_LOGI(TAG, "Trigger Smartconfig");
                
                esp_restart();
            }
            else if (strcmp(mqtt_obj.action, "AP") == 0)
            {
                ESP_LOGI(TAG, "Trigger SoftAP");
                
                esp_restart();
            }
            else
            {
                ESP_LOGI(TAG, "Unknown action");
            }
            vRingbufferReturnItem(mqtt_ring_buf, (void*)mess_recv);
        }
    }
}

void mqtt_client_start(void)
{   
    uint8_t broker[50] = {0};
    ESP_LOGI(TAG, "MQTT init");
    sprintf((char*)broker, "mqtt://%s", MQTT_BROKER);
    ESP_LOGI(TAG, "Broker: %s", broker);
    mqtt_ring_buf = xRingbufferCreate(4096, RINGBUF_TYPE_NOSPLIT);
    if (mqtt_ring_buf == NULL)
        ESP_LOGE(TAG, "Failed to create ring buffer");
    ESP_LOGI(TAG, "done Ring_buffer");
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = (char *)broker,
        .broker.address.port = 1883,
        // .credentials.username = MQTT_USERNAME,
        // .credentials.client_id = MQTT_PASSWORD,
        .session.keepalive = 60,
    };
    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
    xTaskCreate(&mqtt_client_task, "mqtt_task", 4096, NULL, 9, NULL);
}

