#pragma once
#ifndef READZONEMERGE_H
#define READZONEMERGE_H

typedef struct ZoneDeviceMerge
{
    unsigned char type;
    unsigned char endpoint;
    unsigned short shortAddr;
    unsigned char ieeeAddress[8];
    unsigned char alarmMode;
    unsigned char camEventTrig;
    unsigned char pad1[2];
    unsigned char devParam[16];
    unsigned char devPlaceNum[2];
    unsigned char devDescp[24];
    unsigned char bindList[5];
    unsigned char param1;
    unsigned char isAlive;
    unsigned char value[19];
    unsigned char index;
    unsigned char unsed[3];
} Zone_Device_Merge;

typedef enum
{
    NGW_240,        // 0
    SCT_220 = 1,    // 1
    WCD,            // 2
    STP_220,        // 3
    WHS,        // 4
    WGS,        // 5
    WGD,        // 6
    WGDD,       // 7
    SMT_220,        // 8
    WTP,        // 9
    SSM_220,        // 10
    WPW_220,        // 11
    WAL_220,        // 12
    TDI_220,        // 13
    TAI_220,        // 14
    WLO_220,        // 15
    CBT_220,        // 16
    CPN_260,        // 17
    NRT_220,        // 18
    WRL_220,        // 19
    CPN_240C,       // 20
    WRL_240,        // 21
    STH_220,        // 22
    WRS_220,        // 23
    DMB_220,            // 24
    SCO_220,        // 25
    WPL_100,        // 26
    SCT_230,        // 27
    SMT_250,        // 28
    SGR_220,        // 29
    SIR_220,        // 30
    WLL_220,        // 31
    SCO_230,        // 32
    WSI_220,        // 33
    SCT_240,       // 34 
    SMT_240,        // 35
    WAL_250,        // 36
    SKL_220,        // 37
    SKC_220,
    SKL_230,
    SKC_210,
    unknown,
    SMT_230N = 42,
    CPT_220 = 45,
    WRL_250 = 46,
    CNT_220 = 47,
    CPT_220LR = 48,
    DSI_220 = 49,
    WIR_220 = 50,
    FORCE_220 = 51,
    WRS_232 = 52,
    WLK_220 = 53,
    WCP_220 = 54,
    MOBILE_APP = 0x50,
    IP_CAMERA = 0x51,
    SCT_300 = 0x60,
    SMT_300 = 0x61,
    CPT_300 = 0x62,
    WDI_300 = 0x63,
    HAMAN_UNDEFINED = 0x64,
    RPT_300 = 0x65,
    WSI_300 = 0x66,
    WRS_300 = 0x67,
    CT_300 = 0x68,
    WAL_300 = 0x69,
    UNKNOW = 0xFF,
} e_DEVICE_TYPE;

// typedef enum
// {
//     NGW_240 = 0,
//     門窗偵測器 = 1,
//     一氧化碳偵測器,
//     TemperatureSensor,
//     HumiditySensor,
//     Glasssensor,
//     Gasdetector,
//     Gasdetector,
//     位移偵測器,
//     ThermoPile,
//     光電式煙霧偵測器,
//     電源控制器,
//     警報蜂鳴器,
//     數位訊號轉換器,
//     類比訊號轉換器,
//     電鎖控制器,
//     遠端遙控器,
//     CPN260,
//     WirelessRouter,
//     迴路控制器,
//     終端控制器,
//     電源迴路,
//     溫濕度偵測器,
//     RS485_RS232,
//     MobileDialer,
//     二氧化碳偵測器,
//     WPL100,
//     門窗偵測器,
//     位移偵測器,
//     位移偵測器,
//     紅外線控制器,
//     照明,
//     一氧化碳偵測器,
//     狀態指示器,
//     門窗偵測器,
//     位移偵測器,
//     紅外線報警器,
//     電燈控制,
//     窗簾控制,
//     電燈控制,
//     窗簾控制,
//     IPCam,
//     SMT_230N = 42,
//     警報按鈕 = 45,
//     鐵捲門開關,
//     CNT,
//     警報按鈕L,
//     門鈴讀卡機,
//     無線紅外線控制器,
//     電源迴路220A,
//     無線控制門鎖,
//     漏水偵測器,
//     激光罩杯,
//     MOBILE_APP = 80,
//     IP_CAMERA = 81,
//     RF門窗偵測器 = 96,
//     RF位移偵測器,
//     RF警報按鈕,
//     RF數位訊號轉換器,
//     未定義裝置,
//     RF中繼器,
//     RF狀態指示器,
//     RF無線耶魯門鎖,
//     CT_300 = 104,
//     WAL_300 = 105,
//     UNKNOW = 0xFF
// } e_DEVICE_NAME;

int ReadZoneMergeSize(void);
int ReadZoneMerge(Zone_Device_Merge *zone_merges);
void PrintZoneMerge(Zone_Device_Merge *p_zone_device_merge);

#endif