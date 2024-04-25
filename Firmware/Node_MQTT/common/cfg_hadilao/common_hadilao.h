/**
 * @file common.h
 * @author Kyuubi0323 (khoi.nv202647@sis.hust.edu.vn)
 * @brief 
 * @version 0.1
 * @date 2023-12-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _COMMON_HADILAO_H_
#define _COMMON_HADILAO_H_

#include <math.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


#define SMARTCONFIG_MODE 10

#define MAIN_MODE 30

typedef enum
{
    NOT_STATE,
    POWER_ON_PROVISIONING,
    SMARTCONFIG,
    FOTA,
    PROVISIONING,
    WIFI_SOFTAP,
}status_blue_t;

typedef struct 
{   
    char cmd[20];
    char action[15];
    int state;
    char url[50];
    int timeout;
} mqtt_obj_t;

typedef struct {
    uint8_t node_id;
    char ver[4];
    uint8_t temp;
    uint8_t humid;
    uint8_t LDR_value;
} node_t;


esp_err_t mqtt_parse_data(char *mqtt_data, mqtt_obj_t *mqtt_obj);

#endif