/*
  命名規則參照

*/

#pragma once
#ifndef CREATE_OBJECTS_H
#define CREATE_OBJECTS_H

#include "ReadZoneMerge.h"
#include "ReadZoneStatus.h"
#include "ReadFlash.h"
#include "RealTime.h"
#include <linux/if.h>  //struct ifreq
#include <sys/ioctl.h> //SIOCGIFHWADDR
#include <math.h>

#define MAX_MAC_LEN 40
// # =====定義JSON資料結構===========
//  todo 還沒寫完
typedef struct deviceInfoattr_t
{
    // todo 要再確認以下變數長度是否足夠
    char version[5];
    char firmware[40];
    char type[10];
    char model[10];
    int looparray[99];
} DeviceInfoAttr_t;

typedef struct deviceInfo_t
{
    // todo 要再確認以下變數長度是否足夠
    char id[40];
    char macAddress[40];
    char rawdataTime[20];
    unsigned char index[1];
    DeviceInfoAttr_t attributes;
} DeviceInfo_t;

// # 該裝置的數據
typedef struct rawdata_t
{
    // todo 要再確認以下變數長度是否足夠
    char sensor[20]; // 裝置數據
    char label[20];  // 敘述
    char time[20];
    char unit[5];      // 單位
    char dimension[3]; // 三個感測數據的dimension
    char value[6];     // 定義該項的dimension最多為5 // todo 是否需要改成動態？
} RawData_t;

typedef struct rawdataattr_t
{
    // todo 要再確認以下變數長度是否足夠
    char name[30]; // 裝置名稱
    char type[20]; // 機材總類
    char firmware[40];
    char batch[10];     // 生產批號
    char SetupDate[10]; // 安裝日期
    char MFD[10];       // 出廠日期
    char Loop1;
    char devDescp[25];
    char devPlaceNumZone;
    RawData_t data[9]; // 20231020 原本宣告3,電源控制器關係給8 大部分空間浪費
} RawDataAttr_t;

typedef struct rawdataInfo_t
{
    // todo 要再確認以下變數長度是否足夠
    unsigned char type;
    char id[20];
    char manufacturerId[10];
    char rawdataTime[20];
    unsigned char index[1];
    RawDataAttr_t attributes;
} RawDataInfo_t;

int deviceNum; // 幾個裝置,ZoneMerge files
DeviceInfo_t DeviceInfo;
RawDataInfo_t *RawDataInfo;
int RawDataInfoFlag;     // 1:有分配記憶體 0:沒分配
int zoneStatusEmptyFlag; // 1:代表沒有zone_status_RD.lf檔案

int CJSON_CDECL create_JSONobjects(unsigned char index); // deviceNum:幾個裝置 DeviceInfo:裝置資訊
int ReadAllData(void);
char *DeviceStatusTran(char *p_value, char type);
DeviceInfo_t InitialDeviceInfoValue(DeviceInfo_t DeviceInfo);
void InitialRawDataInfoValue(int deviceNum, RawDataInfo_t *DeviceInfo);
void PrintDeviceInfo(DeviceInfo_t DeviceInfo);
void PrintRawDataInfo(int deviceNum, RawDataInfo_t *RawDataInfo);
char *IndexToNameString(unsigned char nameIndex);
char *IndexToTypeString(unsigned char nameIndex);
char *getMACAddress(void);
void WriteToLogFile(char *arg);

#endif