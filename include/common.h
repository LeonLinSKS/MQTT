#define VERSION "097C 20260211 正式區"

#ifndef COMMON_H
#define COMMON_H

// 程式架構第一層的debug
#ifndef DEBUG
// #define DEBUG
#endif

// 程式架構第二層的debug
#ifndef DEBUGLEVEL2
// #define DEBUGLEVEL2
#endif

// define in VM test
// undefine in Machine
#ifndef VM
// #define VM
#endif

// 決定是否印出debug訊息
#ifndef DEBUGLPRINT
// #define DEBUGLPRINT
#endif

// define 正式區
// undefine 測試區
#ifndef IOTMQTT
#define IOTMQTT
#endif
// define 開啟TLS(MQTTS)
// 不開TLS注意要complier要移除-D MQTT_USE_BIO
#ifndef TLSON
#define TLSON
#endif

// 將log過mqtt送出
#ifndef SEND_LOG_TO_MQTT
// #define SEND_LOG_TO_MQTT
#endif

// # 取消沒使用
//  define 開啟TLS(MQTTS)
//  #ifndef AUTORECONNECT
//  #define AUTORECONNECT
//  #endif

#define BOOL int
#define TRUE 1
#define FALSE 0
#define lofbufsize 256
char logbuf[lofbufsize]; // log字串長度256

#include <stdio.h>  // printf...
#include <stdlib.h> // for exit(0);
#include <string.h> // memset,strcpy...
#include <errno.h>  // errno
// #include <bits/stdint-uintn.h>
// #include <stdbool.h>
#include <pthread.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h> // hostent
#include <arpa/inet.h>
#include <signal.h>

#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "cJSON.h"

// sksiot log
extern const char *UIS_name_log;
extern const char *Loacl_name_log; // VM 測試路徑

#endif
