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
char topic_commands_data[25] = "/mandevices/response";
char topic_commands_gateway[25] = "/mandevices/gateway";
//node_t _sensor;
mqtt_obj_t mqtt_obj;

extern const uint8_t client_cert_pem_start[] asm("_binary_client_crt_start");
extern const uint8_t client_cert_pem_end[] asm("_binary_client_crt_end");
extern const uint8_t client_key_pem_start[] asm("_binary_client_key_start");
extern const uint8_t client_key_pem_end[] asm("_binary_client_key_end");
extern const uint8_t server_cert_pem_start[] asm("_binary_mosquitto_org_crt_start");
extern const uint8_t server_cert_pem_end[] asm("_binary_mosquitto_org_crt_end");

void cJSON_mqtt_handler(void *event_data, mqtt_obj_t *mqtt_obj)
{   esp_mqtt_event_handle_t event = event_data;
    client = event->client;
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "id", mqtt_obj->id);
    cJSON_AddStringToObject(root, "version", mqtt_obj->version);
    cJSON_AddNumberToObject(root, "temp", mqtt_obj->temp); 
    cJSON_AddNumberToObject(root, "humid", mqtt_obj->humid);
    cJSON_AddNumberToObject(root, "light", mqtt_obj->LDR_value);
    char *payload = cJSON_Print(root);
    esp_mqtt_client_publish(client, topic_commands_gateway, payload, 0, 1, 1);
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
        //esp_mqtt_client_subscribe(client, topic_commands_gateway, 0);
        esp_mqtt_client_subscribe(client, topic_commands_data, 0);
        //cJSON_mqtt_handler(event, &mqtt_obj);
        break;
    }
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT event disconnected");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT event subcribed, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT event unsubcribed, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT event published, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
    {   
        //ESP_LOGI(TAG, "Event ID:%d\n Event Data: %.*s\n", event->msg_id, event->data_len, event->data);
        UBaseType_t res = xRingbufferSend(mqtt_ring_buf, event->data, event->data_len, portMAX_DELAY);
        if(res != pdTRUE)
            ESP_LOGE(TAG, "Failed to send item\n");
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
        mess_recv = (char*)xRingbufferReceive(mqtt_ring_buf, &mess_size, pdMS_TO_TICKS(2000));
        if (mess_recv)
        {
            mess_recv[mess_size] = '\0';
            ESP_LOGI(TAG, "Recv payload: %s", mess_recv);
            memset(&mqtt_obj, 0, sizeof(mqtt_obj));
            mqtt_parse_data(mess_recv, &mqtt_obj);
            vRingbufferReturnItem(mqtt_ring_buf, (void*)mess_recv);
        }
    }
}

void mqtt_client_start(void)
{   
    uint8_t broker[50] = {0};
    sprintf((char*)broker, "mqtts://%s", MQTT_BROKER);  
    ESP_LOGI(TAG, "Broker: %s", broker);
    mqtt_ring_buf = xRingbufferCreate(4096, RINGBUF_TYPE_NOSPLIT);
    if (mqtt_ring_buf == NULL)
        ESP_LOGE(TAG, "Failed to create ring buffer");
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = (char *)broker,
        .broker.address.port = 8884,
        .broker.verification.certificate = (const char *)server_cert_pem_start,
        .credentials = {
            .authentication = {
                .certificate = (const char *)client_cert_pem_start,
                .key = (const char *)client_key_pem_start,
                }
         },
        .session.keepalive = 60
                       
    };
    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
    xTaskCreate(&mqtt_client_task, "mqtt_task", 2048, NULL, 9, NULL);
}

