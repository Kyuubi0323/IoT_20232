/* CoAP client Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/*
 * WARNING
 * libcoap is not multi-thread safe, so only this thread must make any coap_*()
 * calls.  Any external (to this thread) data transmitted in/out via libcoap
 * therefore has to be passed in/out by xQueue*() via this thread.
 */

#include <string.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <netdb.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_event.h"

#include "wifi_sta_hadilao.h"
#include "led_hadilao.h"
#include "common_hadilao.h"

#include "coap3/coap.h"
#include "coap3/coap_internal.h"


#define ESP_WIFI_SSID      		"Kyuubi"		// Please insert your SSID
#define ESP_WIFI_PASS      		"laclac123"		// Please insert your password
#define ESP_WIFI_AUTH_MODE		WIFI_AUTH_WPA2_PSK // See esp_wifi_types.h
#define ESP_WIFI_MAX_RETRY 		5U

#define THETHINGSIO_TOKEN_ID 	"5Bl--lhSEfIiWI-chEouKEAaOsIWLrD5An5KMegGo7I"
//#define THETHINGSIO_TOKEN_ID    "VGfuIegbEt6gwjO82oY2iitab_59g8wHgqP_OD8CcDU"
#define THETHINGSIO_COAP_HOST	"coap://coap.thethings.io"
#define THETHINGSIO_COAP_PATH 	"v2/things/" THETHINGSIO_TOKEN_ID

#define TEMP_SENSOR_TASK_DELAY	1000U	// In milliseconds
#define COAP_POST_DELAY			300000U	// In milliseconds
#define COAP_DEFAULT_TIME 		5000U	// In milliseconds

#define COAP_BUFFER_SIZE 		((uint8_t)64)

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static ip_event_got_ip_t* event = NULL;
static uint8_t u8RetryCounter = 0U;

const static char *pcTAG = "COAP_CLIENT";

static float fTemperature = 21;

static bool resp_wait = true;
static coap_optlist_t *optlist = NULL;
static int wait_ms = 0U;



static void CoAP_event_handler(coap_context_t *psCtx, coap_session_t *psSession, coap_pdu_t *psPduSent, coap_pdu_t *psPduReceived,
                            const coap_mid_t id)
{

    unsigned char *pcBuffer = NULL, acBuffer[4];
    size_t BufferLen;
    coap_pdu_t *psPdu = NULL;
    coap_opt_t *sBlockOpt;
    coap_opt_iterator_t sOptIter;
    coap_optlist_t *sOption;
    coap_mid_t sMid;
    if (COAP_RESPONSE_CLASS(psPduReceived->code) == 2)
    {
        /* Need to see if blocked response */
        sBlockOpt = coap_check_option(psPduReceived, COAP_OPTION_BLOCK2, &sOptIter);

        if (sBlockOpt)
        {
            uint16_t blktype = sOptIter.number;

            if (coap_opt_block_num(sBlockOpt) == 0)
            {
                printf("Received:\n");
            }

            if (coap_get_data(psPduReceived, &BufferLen, &pcBuffer))
            {
                ESP_LOGI(pcTAG, "COAPPPPPP DATA NEEEEEE\n\r");
                printf("%.*s", (int)BufferLen, pcBuffer);
            }

            if (COAP_OPT_BLOCK_MORE(sBlockOpt))
            {
                /* more bit is set */

                /* create pdu with request for next block */
                psPdu = coap_new_pdu(COAP_MESSAGE_CON, COAP_REQUEST_GET, psSession);

                if (!psPdu)
                {
                    ESP_LOGE(pcTAG, "coap_new_pdu() failed");
                    goto clean_up;
                }
                psPdu->mid = coap_new_message_id(psSession);
                /* add URI components from optlist */
                for (sOption = optlist; sOption; sOption = sOption->next )
                {
                    switch (sOption->number)
                    {
						case COAP_OPTION_URI_HOST :
						case COAP_OPTION_URI_PORT :
						case COAP_OPTION_URI_PATH :
						case COAP_OPTION_URI_QUERY :
							coap_add_option(psPdu, sOption->number, sOption->length,
											sOption->data);
							break;
						default:
							;     /* skip other options */
                    }
                }

                /* finally add updated block option from response, clear M bit */
                /* blocknr = (blocknr & 0xfffffff7) + 0x10; */
                coap_add_option(psPdu, blktype, coap_encode_var_safe(acBuffer, sizeof(acBuffer),
                                                     	 	 	 	 ((coap_opt_block_num(sBlockOpt) + 1) << 4) |
																	 COAP_OPT_BLOCK_SZX(sBlockOpt)), acBuffer);

                sMid = coap_send(psSession, psPdu);

                if (sMid != COAP_INVALID_MID)
                {
                    resp_wait = true;
                    wait_ms = COAP_DEFAULT_TIME;
                    return;
                }
            }
            printf("\n");
        }
        else
        {
            if (coap_get_data(psPduReceived, &BufferLen, &pcBuffer))
            {
                printf("Received: %.*s\n", (int)BufferLen, pcBuffer);
            }
        }
    }

clean_up:

    resp_wait = false;

}

static void CoAP_Task(void *p)
{

	const char acThingUri[] = THETHINGSIO_COAP_HOST "/v2/things/" THETHINGSIO_TOKEN_ID;
	const uint8_t acThingData[50] = {0};

    struct hostent *psHostent;
    coap_address_t  dst_addr;
    static coap_uri_t sUri;
    char *pcHostName = NULL, acTemporalBuffer[INET6_ADDRSTRLEN];
    unsigned char *pcBuffer, acBuffer[COAP_BUFFER_SIZE];
	size_t BufferLen;
	int res = 0U;
	coap_context_t *psCtx = NULL;
	coap_session_t *psSession = NULL;
	coap_pdu_t *psRequest = NULL;

	coap_set_log_level(LOG_DEBUG);

    vTaskDelay(1100U / portTICK_PERIOD_MS);

    while (1)
    {
        optlist = NULL;

        if (coap_split_uri((const uint8_t *)acThingUri, strlen(acThingUri), &sUri) == -1)
        {
            ESP_LOGE(pcTAG, "CoAP server uri error");
            break;
        }

        pcHostName = (char *)calloc(1, sUri.host.length + 1);


        if (pcHostName == NULL)
        {
            ESP_LOGE(pcTAG, "calloc failed");
            break;
        }

        memcpy(pcHostName, sUri.host.s, sUri.host.length);
        psHostent = gethostbyname(pcHostName);
        

        if (psHostent == NULL)
        {
            ESP_LOGE(pcTAG, "DNS lookup failed");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            free(pcHostName);
            continue;
        }
        free(pcHostName);
        coap_address_init(&dst_addr);

        switch (psHostent->h_addrtype)
        {
            case AF_INET:
                dst_addr.addr.sin.sin_family      = AF_INET;
                dst_addr.addr.sin.sin_port        = htons(sUri.port);
                memcpy(&dst_addr.addr.sin.sin_addr, psHostent->h_addr, sizeof(dst_addr.addr.sin.sin_addr));
                inet_ntop(AF_INET, &dst_addr.addr.sin.sin_addr, acTemporalBuffer, sizeof(acTemporalBuffer));
                ESP_LOGI(pcTAG, "DNS lookup succeeded. IP=%s", acTemporalBuffer);
                break;
            case AF_INET6:
                dst_addr.addr.sin6.sin6_family      = AF_INET6;
                dst_addr.addr.sin6.sin6_port        = htons(sUri.port);
                memcpy(&dst_addr.addr.sin6.sin6_addr, psHostent->h_addr, sizeof(dst_addr.addr.sin6.sin6_addr));
                inet_ntop(AF_INET6, &dst_addr.addr.sin6.sin6_addr, acTemporalBuffer, sizeof(acTemporalBuffer));
                ESP_LOGI(pcTAG, "DNS lookup succeeded. IP=%s", acTemporalBuffer);
                break;
            default:
                ESP_LOGE(pcTAG, "DNS lookup response failed");
                goto clean_up;
        }

        if (sUri.path.length)
        {
            BufferLen = COAP_BUFFER_SIZE;
            pcBuffer = acBuffer;

            res = coap_split_path(sUri.path.s, sUri.path.length, pcBuffer, &BufferLen);

            while (res--)
            {
                coap_insert_optlist(&optlist, coap_new_optlist(COAP_OPTION_URI_PATH, coap_opt_length(pcBuffer), coap_opt_value(pcBuffer)));

                pcBuffer += coap_opt_size(pcBuffer);
            }
        }

        if (sUri.query.length)
        {
            BufferLen = COAP_BUFFER_SIZE;
            pcBuffer = acBuffer;

            res = coap_split_query(sUri.query.s, sUri.query.length, pcBuffer, &BufferLen);

            while (res--)
            {
                coap_insert_optlist(&optlist, coap_new_optlist(COAP_OPTION_URI_QUERY, coap_opt_length(pcBuffer), coap_opt_value(pcBuffer)));

                pcBuffer += coap_opt_size(pcBuffer);
            }
        }

        psCtx = coap_new_context(NULL);

        if (!psCtx)
        {
            ESP_LOGE(pcTAG, "coap_new_context() failed");
            goto clean_up;
        }

        psSession = coap_new_client_session(psCtx, NULL, &dst_addr, COAP_PROTO_UDP);

        if (!psSession)
        {
            ESP_LOGE(pcTAG, "coap_new_client_session() failed");
            goto clean_up;
        }

        coap_register_response_handler(psCtx, (coap_response_handler_t)CoAP_event_handler);

        psRequest = coap_new_pdu(COAP_MESSAGE_CON, COAP_REQUEST_POST, psSession);

        if (!psRequest)
        {
            ESP_LOGE(pcTAG, "coap_new_pdu() failed");
            goto clean_up;
        }
        psRequest->mid = coap_new_message_id(psSession);
        coap_add_optlist_pdu(psRequest, &optlist);

        sprintf((char *)acThingData, "{\"values\":[{\"key\":\"Temperature\",\"value\":\"%2.2f\"}]}", fTemperature);
        ESP_LOGI(pcTAG, "Message sent: %s", acThingData);
        coap_add_data(psRequest, sizeof(acThingData) - 1, acThingData);

        coap_send(psSession, psRequest);

        resp_wait = false;
        wait_ms = COAP_DEFAULT_TIME;

        while (resp_wait)
        {
            int result = coap_run_once(psCtx, wait_ms > 1000U ? 1000U : wait_ms);

            if (result >= 0)
            {
                if (result >= wait_ms)
                {
                    ESP_LOGE(pcTAG, "Select timeout");
                    break;
                }
                else
                {
                    wait_ms -= result;
                }
            }
    	}

clean_up:

        if (optlist)
        {
            coap_delete_optlist(optlist);
            optlist = NULL;
        }

        if (psSession)
        {
            coap_session_release(psSession);
        }

        if (psCtx)
        {
            coap_free_context(psCtx);
        }

        coap_cleanup();

        /*
         * change the following line to something like sleep(2)
         * if you want the request to continually be sent
         */
        vTaskDelay(COAP_POST_DELAY / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}
char ssid[50] = {0};
char pwd[50] = {0};
status_blue_t status_blue;

void app_main(void)
{

	// Initialize NVS
	esp_err_t ret = nvs_flash_init();

	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
	  ESP_ERROR_CHECK(nvs_flash_erase());
	  ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);
    status_blue = NORMAL_MODE;  

    chip_stats();
    
    wifi_init();
    sprintf(ssid, "Kyuubi");
    sprintf(pwd, "laclac123");
    wifi_config_t wifi_config;
    bzero(&wifi_config, sizeof(wifi_config_t));
    memcpy(wifi_config.sta.ssid, ssid, strlen(ssid));
    memcpy(wifi_config.sta.password, pwd, strlen(pwd));
    
    // Initialize station mode
    wifi_sta_start(wifi_config, WIFI_MODE_STA);
	vTaskDelay(2000 / portTICK_PERIOD_MS);
    xTaskCreate(led_blue_task, "led_blue_task", 2048, NULL, 10, NULL);
    xTaskCreate(CoAP_Task, "coap_task", 8192U, NULL, 5, NULL);

}
