
#pragma once
#ifndef MQTTCOMM_H
#define MQTTCOMM_H

// #ifdef true
// #undef true
// #endif
// #define true ((cJSON_bool)1)

// #ifdef false
// #undef false
// #endif
// #define false ((cJSON_bool)0)

// #include <sys/types.h>
// #include <sys/stat.h>

#include "create_objects.h"
// #include "cJSON.h"

#define offline 0
#define online 1
#define flag_yes 0
#define flag_no 1
#define connectFail 0
#define connectOK 1


struct mqtt_client client;
char internet_healthy; // 0連線異常  1:連線正常
char mqttconnectFlag = 1 ;  // 0連線異常  1:連線正常
// extern char logbuf[lofbufsize]; // log字串長度256
char first_run;   // 0第一次執行(等待開機初始完成)
char errorCount;  // 共做mqtt publish幾次
char errorCount2; // 連續幾次傳送不成功,五分鐘算一次
char errorCount3;

typedef struct
{
    const char *hostname;
    const char *port;
    const char *recv_topic;
    unsigned char *sendbuf;
    size_t sendbufsz;
    unsigned char *recvbuf;
    size_t recvbufsz;
    const char *client_id;
} reconnect_state_t;

typedef enum
{
    EXIT_FAILURE_NULL = 1,
    EXIT_FAILURE_hostname_to_ip = 2,
    EXIT_FAILURE_open_nb_socket,
    EXIT_FAILURE_mqtt_init,
    EXIT_FAILURE_mqtt_connect,
    EXIT_FAILURE_pthread_create_daemon,
    EXIT_FAILURE_mqtt_subscribe,
    EXIT_FAILURE_mqtt_publish,
    EXIT_FAILURE_dehealthy1,
    EXIT_FAILURE_dehealthy2 = 10,
    EXIT_FAILURE_load_certificate
} e_EXIT_FAILURE_MODEL;

unsigned short SNO = 0;
const int SIZE = 128; // # 檔案要對齊4096bytes?
const char *name = "shm_mqtt";
const char *FirmwareUpgrade = "FirmwareUpgrade";

// shared file for FW upgrage
const char *UIS_name_Upgrade = "/var/www/shm_mqtt.txt";
const char *Loacl_name = "shm_mqtt.txt"; // VM 測試路徑

// shared file for internet status
const char *UIS_name_internet = "/var/www/shf_internet.txt"; // off
const char *Loacl_name_internet = "shf_internet.txt";        // VM 測試路徑

void *mqtt_listener(void *arg);
void *client_refresher(void *client);
void MqttExecute(void);
void display(char *prog, char *bytes, int n);
int hostname_to_ip(char *hostname, char *ip);
int mqttreconnect(reconnect_state_t *reconnect_state);
int SetReconnectState(reconnect_state_t *reconnect_state);

#endif