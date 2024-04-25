/**
 * @file mqtt.h
 * @author Kyuubi0323 (khoi.nv202647@sis.hust.edu.vn)
 * @brief 
 * @version 0.1
 * @date 2023-12-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _MQTT_HADILAO_H_
#define _MQTT_HADILAO_H_

#include <math.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

//#define MQTT_BROKER "broker.hivemq.com"
#define MQTT_BROKER "test.mosquitto.org"
//#define MQTT_BROKER "0.0.0.0"
//#define MQTT_BROKER "mqtt.eclipseprojects.io"
//#define MQTT_BROKER "38.105.209.36"
//#define MQTT_USERNAME "device_01"
//#define MQTT_ID "device_01"
//#define MQTT_USER       "unused"
//#define MQTT_PASS       "unused"
//#define MQTT_BROKER "mqtt.thingsboard.cloud"

//#define MQTT_USERNAME "device01"
//#define MQTT_PASSWORD "device01"
// #define MQTT_TOKEN "zzcrb6epswmwu0oygjg1


void mqtt_client_start(void);

#endif