/*
 make all
 gcc cJSON.c create_objects.c -o create_objects -lm -I.

 msdk-linux-gcc -g -I ./include cJSON.c create_objects.c ReadZoneMerge.c ReadZoneStatus.c ReadFlash.c RealTime.c LogManage.c libapmib.so -lm -o create_objects
 gcc -g -I ./include cJSON.c create_objects.c ReadZoneMerge.c ReadZoneStatus.c ReadFlash.c RealTime.c LogManage.c -lm -o create_objects
 valgrind --leak-check=full ./create_objects
*/

#include "common.h"
#include "create_objects.h"
#include "LogManage.h"
#include <ctype.h>
#include <dirent.h>

char *buf = NULL; // 組回JSON值 // todo 寫在這裡不好,容易忘記釋放記憶體
int bufFlag = 0;  // 1分配過 0沒分配過

// sksiot log
const char *UIS_name_log = "/var/www/sks.log";
const char *Loacl_name_log = "sksiot_log.txt"; // VM 測試路徑

#ifdef DEBUGLEVEL2
int main(void)
{

    deviceNum = ReadAllData();

    //============Read time===================

    DeviceInfo = InitialDeviceInfoValue(DeviceInfo);

    printf("iot. A99 DeviceInfo.macAddress= %s\n", DeviceInfo.macAddress);
    printf("iot. A98 DeviceInfo.id= %s\n", DeviceInfo.id);

    int ok = create_JSONobjects(DeviceInfo.index[1]);

    free(RawDataInfo);

    if ((buf != NULL) && (ok == 0))
    {
        printf("iot. %s\n", buf);
        free(buf);
    }
    else
    {
        printf("iot. xxx\n");
        // todo 錯誤處理
    }

    return 0;
}
#endif

// # 讀取所有資料,並將資料轉換為DeviceInfo, RawDataInfo
// input *DeviceInfo, *RawDataInfo
// ouput:返回裝置數量
// # (1)讀取zone_merge資料
// # (2)讀取zone_status_RD.lf
// # (3)讀取Flash資料
// # (4)讀取time
int ReadAllData(void)
{
    zoneStatusEmptyFlag = 0;     // zone_status_RD.lf是否存在(不存在表示都離線)
    int deviceNumTemp;           // 幾個裝置,ZoneMerge files
    int deviceNumZoneStatus = 0; // 幾個裝置,ZoneStatus files
    int sensorCount;             // 檔案大小/感測器size
    char valueTemp[5];
    memset(valueTemp, 0, sizeof(valueTemp));
    //====ReadZoneMerge=====
    Zone_Device_Merge *zone_merges;
    //====ReadZoneStatus=====
    Zone_Device_Status_RD *zone_status = NULL;

    //====ReadFlash=========
    Flash_Data *p_flash_data;
#ifdef DEBUGLPRINT
    printf("iot. ****************************\n");
    printf("iot. ********ReadZoneMerge*******\n");
    printf("iot. ****************************\n");
#endif

    sensorCount = ReadZoneMergeSize(); // 讀取檔案中sensor資料(包含廢資料)

#ifdef DEBUGLPRINT
    printf("iot. ZoneMerge file sensorCount = %d \n", sensorCount);
#endif

    if (sensorCount == 0)
    {
        deviceNumTemp = 0;

#ifdef DEBUGLPRINT
        printf("iot. no sensor device \n");
#endif

        RealTimeFormat();
        memset(logbuf, 0, lofbufsize);
        sprintf(logbuf, "%s: %s/%s, no sensor device ", timestring_TimeFormat, __FILE__, __func__);
        WriteToLogFile(logbuf);
        return deviceNumTemp;
    }
    else
    {
        zone_merges = calloc(sensorCount, sizeof(Zone_Device_Merge));
        deviceNumTemp = ReadZoneMerge(zone_merges); // 取得真正有用資料數與zone_merges
// RawDataInfo = calloc(deviceNumTemp, sizeof(RawDataInfo_t));
// RawDataInfo_t RawDataInfo[deviceNumTemp];
#ifdef DEBUGLPRINT
        printf("iot. real number of ZoneMerge file = %d\n", deviceNumTemp);
#endif
    }

//============ReadZoneStatus==============
#ifdef DEBUGLPRINT
    printf("iot. ****************************\n");
    printf("iot. ********ReadZoneStatus*******\n");
    printf("iot. *****************************\n");
#endif
    // if (getMACAddress() < 0)
    // {
    //     printf("iot. Get MAC address, Error\n");
    //     //deviceNum todo
    // }
    strcpy(DeviceInfo.macAddress, (char *)getMACAddress());

    sensorCount = ReadZoneStatusSize(); // 讀取檔案中sensor資料數量(包含廢資料)

#ifdef DEBUGLPRINT
    printf("iot. ZoneStatus file sensorCount = %d\n", sensorCount);
#endif
    if (sensorCount == 0)
    {
        // === 智慧家全部感測器都離線(isAlive都為0),智慧家不會產生zone_status_RD.lf檔案 ===
        // 確認isAlive是否都為0
        int l;
        for (l = 0; l < deviceNumTemp; l++)
        {
            if (zone_merges[l].isAlive > 0)
            {
                deviceNumTemp = 0;
                printf("iot. no sensor device ; l = %d\n", l);
                RealTimeFormat();
                memset(logbuf, 0, lofbufsize);
                sprintf(logbuf, "%s: %s/%s, no sensor device ", timestring_TimeFormat, __FILE__, __func__);
                WriteToLogFile(logbuf);
                echo_FileToUSB(logbuf);
                return deviceNumTemp;
            }
            zoneStatusEmptyFlag = 1;
            zone_status = calloc(sizeof(Zone_Device_Status_RD), 215); // todo 這裡應該會清0
            printf("iot. dummy number = %d\n", 215);
        }
    }
    else
    {
        zone_status = calloc(sizeof(Zone_Device_Status_RD), sensorCount);
        deviceNumZoneStatus = ReadZoneStatus(zone_status); // 取得真正有用資料數
#ifdef DEBUGLPRINT
        printf("iot. real number of ZoneStatus file = %d\n", deviceNumZoneStatus);
#endif
    }

//=============ReadFlash==================
#ifdef DEBUGLPRINT
    printf("iot. ****************************\n");
    printf("iot. ********ReadFlash***********\n");
    printf("iot. ****************************\n");
#endif
    p_flash_data = (Flash_Data *)malloc(sizeof(Flash_Data));
    memset(p_flash_data->apmib_mno, 0, sizeof(p_flash_data->apmib_mno));
    memset(p_flash_data->apmib_UID, 0, sizeof(p_flash_data->apmib_UID));
    memset(p_flash_data->apmib_gw, 0, sizeof(p_flash_data->apmib_gw));
    memset(p_flash_data->UCN_TOTAL, 0, sizeof(p_flash_data->UCN_TOTAL));
    memset(p_flash_data->UCR_ALL, 0, sizeof(p_flash_data->UCR_ALL));
    memset(p_flash_data->UCR4, 0, sizeof(p_flash_data->UCR4));
    memset(p_flash_data->UCR, 0, sizeof(p_flash_data->UCR));
    memset(p_flash_data->UCR2, 0, sizeof(p_flash_data->UCR2));
    memset(p_flash_data->UCR3, 0, sizeof(p_flash_data->UCR3));
    ReadFlash(p_flash_data);
// PrintFlashDataMessage(p_flash_data);
#ifdef DEBUGLPRINT
    //============Read time===================
    printf("iot. ****************************\n");
    printf("iot. ********Read time***********\n");
    printf("iot. ****************************\n");
#endif
    RealTimeFormat();

//============Get MAC address===================
#ifdef VM

#else
    // strncpy(ifr.ifr_name, "eth0", IFNAMSIZE - 1);
    // if (ioctl(fd, SIOCGIFHWADDR, &ifr) == -1){
    //     perror("ioctl");
    //     close(fd);
    // }
#endif

    //===========rawdata conversion (zone merge)==============
    int j; //, k;
    sensorCount = 0;
    char ieeeAddressTemp[17]; // todo 這裡記憶體為何多一倍,還要確認
    memset(ieeeAddressTemp, 0, sizeof(ieeeAddressTemp));

#ifdef DEBUGLPRINT
    printf("iot. ****************************\n");
    printf("iot. ********rawdata conversion**\n");
    printf("iot. ****************************\n");
    printf("iot. RawDataInfo_t size= %u\n", sizeof(RawDataInfo_t));
#endif

    sensorCount = ReadZoneStatusSize();

#ifdef DEBUGLPRINT
    printf("iot. Status sensorCount = %d \n", sensorCount);
#endif

    if (zoneStatusEmptyFlag) // # 沒有zone_status_RD且isAlive都為0時
        RawDataInfo = calloc(deviceNumTemp, sizeof(RawDataInfo_t));
    else
        RawDataInfo = calloc(sensorCount, sizeof(RawDataInfo_t));
    RawDataInfoFlag = 1;

#ifdef DEBUGLPRINT
    printf("iot. RawDataInfo size= %u\n", sizeof(RawDataInfo));
#endif

    strcpy(DeviceInfo.id, p_flash_data->apmib_mno);
    for (j = 0; j < deviceNumTemp; j++)
    {
#ifdef VM // # VM資料最後byte改9,避免模擬時與機器重複
        zone_merges->ieeeAddress[0] = 99;
#endif
        RawDataInfo->type = zone_merges->type;
        sprintf(ieeeAddressTemp, "%.02x%.02x%.02x%.02x%.02x%.02x%.02x%.02x", zone_merges->ieeeAddress[7], zone_merges->ieeeAddress[6],
                zone_merges->ieeeAddress[5], zone_merges->ieeeAddress[4], zone_merges->ieeeAddress[3],
                zone_merges->ieeeAddress[2], zone_merges->ieeeAddress[1], zone_merges->ieeeAddress[0]);
#ifdef DEBUGLPRINT
        printf("iot. ieeeAddressTemp = %s \n", ieeeAddressTemp);
#endif
        strcpy(RawDataInfo->id, ieeeAddressTemp); // # 裝置ID
        strcpy(RawDataInfo->manufacturerId, "UIS");
        RawDataInfo->index[0] = zone_merges->index + 1;
#ifdef DEBUGLPRINT
        printf("iot. zone_merges[%d]->index = %02x \n", j, RawDataInfo->index[0]);
#endif
        strcpy(RawDataInfo->rawdataTime, timestring_TimeFormat);
        strcpy(RawDataInfo->attributes.name, IndexToNameString(zone_merges->type));
        // printf("iot. zone_merges[j].type = %d \n", zone_merges[j].type);
        // printf("iot. zone_merges[j].type = %s \n", IndexToNameString(zone_merges[j].type));
        strcpy(RawDataInfo->attributes.type, IndexToTypeString(zone_merges->type));
        strcpy(RawDataInfo->attributes.firmware, (char *)zone_status[RawDataInfo->index[0]].DateCode);
        strcpy(RawDataInfo->attributes.batch, "null");     // todo 再問漢唐是否有該資料
        strcpy(RawDataInfo->attributes.SetupDate, "null"); // todo 再問漢唐是否有該資料
        strcpy(RawDataInfo->attributes.MFD, "null");       // todo 再問漢唐是否有該資料
        // printf("002 len=%d , %s\n", strlen((char *)zone_merges->devDescp), (char *)zone_merges->devDescp);

        // # 漢唐漢字編碼有2byte也有3byte, 3byte編碼8個中文字剛好24bytes所以不會有結束符號\0
        RawDataInfo->attributes.devDescp[24] = '\0';
        if (strlen((char *)zone_merges->devDescp) > 23)
            strncpy(RawDataInfo->attributes.devDescp, (char *)zone_merges->devDescp, 24);
        else
            strcpy(RawDataInfo->attributes.devDescp, (char *)zone_merges->devDescp);
        RawDataInfo->attributes.devPlaceNumZone = zone_merges->devPlaceNum[1];
        RawDataInfo->attributes.Loop1 = zone_merges->devPlaceNum[0]; // 迴路不會重複
        DeviceInfo.attributes.looparray[j] = zone_merges->devPlaceNum[0];
        strcpy(RawDataInfo->attributes.data[0].sensor, "alive");
        strcpy(RawDataInfo->attributes.data[0].label, "連線狀態");
        strcpy(RawDataInfo->attributes.data[0].time, timestring_TimeFormat);
        strcpy(RawDataInfo->attributes.data[0].unit, "\0");
        strcpy(RawDataInfo->attributes.data[0].dimension, "1");
#ifdef DEBUGLPRINT
        printf("iot. zone_merges[j].isAlive = %d\n", zone_merges->isAlive);
#endif
        if (zone_merges->isAlive == 1)
            strcpy(valueTemp, "Yes");
        else
        {
            strcpy(valueTemp, "No");
        }
#ifdef DEBUGLPRINT
        printf("iot. isAlive valueTemp = %s\n", valueTemp);
#endif
        // strcpy(RawDataInfo->attributes.data[0].value, valueTemp); // todo value 暫兩Byte
        RawDataInfo->attributes.data[0].value[0] = zone_merges->isAlive; // 20231107 改為送數值

        strcpy(RawDataInfo->attributes.data[1].sensor, "voltage");
        strcpy(RawDataInfo->attributes.data[1].label, "裝置電壓");
        strcpy(RawDataInfo->attributes.data[1].time, timestring_TimeFormat);
        strcpy(RawDataInfo->attributes.data[1].unit, "V");
        strcpy(RawDataInfo->attributes.data[1].dimension, "1");
#ifdef DEBUGLPRINT
        printf("iot. zone_status[?].value[1] = %d \n", zone_status[RawDataInfo->index[0]].value[1]);
#endif
        RawDataInfo->attributes.data[1].value[1] = zone_status[RawDataInfo->index[0]].value[1];

        strcpy(RawDataInfo->attributes.data[2].sensor, "RSSI");
        strcpy(RawDataInfo->attributes.data[2].label, "信號強度");
        strcpy(RawDataInfo->attributes.data[2].time, timestring_TimeFormat);
        strcpy(RawDataInfo->attributes.data[2].unit, "dB");
        strcpy(RawDataInfo->attributes.data[2].dimension, "1");
        RawDataInfo->attributes.data[2].value[0] = zone_status[RawDataInfo->index[0]].value[0];

        strcpy(RawDataInfo->attributes.data[3].sensor, "action");
        strcpy(RawDataInfo->attributes.data[3].label, "開關狀態");
        strcpy(RawDataInfo->attributes.data[3].time, timestring_TimeFormat);
        strcpy(RawDataInfo->attributes.data[3].unit, "\0");
        strcpy(RawDataInfo->attributes.data[3].dimension, "1");
// todo 這裡要組合裝置狀態,新增一個函數來處理
#ifdef DEBUGLPRINT
        printf("iot. zone_merges[j].value[0] = %d\n", zone_merges->value[0]);
#endif
        strcpy(valueTemp, DeviceStatusTran((char *)zone_merges->value, zone_merges->type));
#ifdef DEBUGLPRINT
        printf("iot. valueTemp = %s\n", valueTemp);
#endif
        // strcpy(RawDataInfo->attributes.data[3].value, valueTemp); // todo value 暫兩Byte
        //  todo 20231107 ON:1 OFF:0 因為還有其他裝置回讀後不是ON跟OFF之後要注意
        if (strcmp(valueTemp, "off") == 0)
            RawDataInfo->attributes.data[3].value[0] = 0;
        else // todo // 因為要返回1或0 所以調成不是0的都是1
            RawDataInfo->attributes.data[3].value[0] = 1;

        if (RawDataInfo->type == 32)
        {
            printf("iot. zone_merges[j].value[0] = %d\n", zone_merges->value[0]);
            printf("iot. zone_merges[j].value[1] = %d\n", zone_merges->value[1]);
            printf("iot. zone_merges[j].value[2] = %d\n", zone_merges->value[2]);
            printf("iot. zone_merges[j].value[3] = %d\n", zone_merges->value[3]);
            printf("iot. zone_merges[j].value[4] = %d\n", zone_merges->value[4]);
        }
        //=================IOT 裝置====================
        // STH 溫濕度偵測器
        if (RawDataInfo->type == 22)
        {
#ifdef DEBUGLPRINT
            printf("iot. -------------STH-----------------\n");
            printf("iot. Temperature = %d:%d \n", zone_merges->value[0], zone_merges->value[1]);
            printf("iot. Humidity = %d:%d \n", zone_merges->value[2], zone_merges->value[3]);
#endif

            strcpy(RawDataInfo->attributes.data[4].sensor, "Temperature");
            strcpy(RawDataInfo->attributes.data[4].label, "溫度");
            strcpy(RawDataInfo->attributes.data[4].time, timestring_TimeFormat);
            strcpy(RawDataInfo->attributes.data[4].unit, "°C");
            strcpy(RawDataInfo->attributes.data[4].dimension, "1");
            RawDataInfo->attributes.data[4].value[0] = zone_merges->value[0];
            RawDataInfo->attributes.data[4].value[1] = zone_merges->value[1];

            strcpy(RawDataInfo->attributes.data[5].sensor, "Humidity");
            strcpy(RawDataInfo->attributes.data[5].label, "濕度");
            strcpy(RawDataInfo->attributes.data[5].time, timestring_TimeFormat);
            strcpy(RawDataInfo->attributes.data[5].unit, "%");
            strcpy(RawDataInfo->attributes.data[5].dimension, "1");
            RawDataInfo->attributes.data[5].value[0] = zone_merges->value[2];
            RawDataInfo->attributes.data[5].value[1] = zone_merges->value[3];
        }

        // WPW 電源控制器
        if (RawDataInfo->type == 11)
        {
#ifdef DEBUGLPRINT
            printf("iot. -------------WPW-----------------\n");
            printf("iot. I = %d \n", zone_merges->value[7]);
            printf("iot. V = %d \n", zone_merges->value[8]);
            printf("iot. F = %d \n", zone_merges->value[9]);
            printf("iot. P = %d:%d \n", zone_merges->value[10], zone_merges->value[11]);
            printf("iot. PF = %d:%d \n", zone_merges->value[12], zone_merges->value[13]);
            printf("iot. KWH = %d:%d:%d:%d \n", zone_merges->value[14], zone_merges->value[15], zone_merges->value[16], zone_merges->value[17]);
#endif

            RawDataInfo->attributes.data[1].value[1] = zone_merges->value[8];

            strcpy(RawDataInfo->attributes.data[4].sensor, "Current");
            strcpy(RawDataInfo->attributes.data[4].label, "電流");
            strcpy(RawDataInfo->attributes.data[4].time, timestring_TimeFormat);
            strcpy(RawDataInfo->attributes.data[4].unit, "A");
            strcpy(RawDataInfo->attributes.data[4].dimension, "1");
            // memset(valueTemp, 0, sizeof(valueTemp));
            // sprintf(valueTemp, "%d.%d", zone_merges->value[7] / 10, zone_merges->value[7] % 10);
            RawDataInfo->attributes.data[4].value[0] = zone_merges->value[7];

            strcpy(RawDataInfo->attributes.data[5].sensor, "Frequency");
            strcpy(RawDataInfo->attributes.data[5].label, "頻率");
            strcpy(RawDataInfo->attributes.data[5].time, timestring_TimeFormat);
            strcpy(RawDataInfo->attributes.data[5].unit, "Hz");
            strcpy(RawDataInfo->attributes.data[5].dimension, "1");
            RawDataInfo->attributes.data[5].value[0] = zone_merges->value[9];

            strcpy(RawDataInfo->attributes.data[6].sensor, "Power");
            strcpy(RawDataInfo->attributes.data[6].label, "功率");
            strcpy(RawDataInfo->attributes.data[6].time, timestring_TimeFormat);
            strcpy(RawDataInfo->attributes.data[6].unit, "W");
            strcpy(RawDataInfo->attributes.data[6].dimension, "1");
            RawDataInfo->attributes.data[6].value[0] = zone_merges->value[10];
            RawDataInfo->attributes.data[6].value[1] = zone_merges->value[11];

            strcpy(RawDataInfo->attributes.data[7].sensor, "PF");
            strcpy(RawDataInfo->attributes.data[7].label, "功率因素");
            strcpy(RawDataInfo->attributes.data[7].time, timestring_TimeFormat);
            strcpy(RawDataInfo->attributes.data[7].unit, "");
            strcpy(RawDataInfo->attributes.data[7].dimension, "1");
            RawDataInfo->attributes.data[7].value[0] = zone_merges->value[12];
            RawDataInfo->attributes.data[7].value[1] = zone_merges->value[13];

            strcpy(RawDataInfo->attributes.data[8].sensor, "KWH");
            strcpy(RawDataInfo->attributes.data[8].label, "KWH");
            strcpy(RawDataInfo->attributes.data[8].time, timestring_TimeFormat);
            strcpy(RawDataInfo->attributes.data[8].unit, "");
            strcpy(RawDataInfo->attributes.data[8].dimension, "1");
            RawDataInfo->attributes.data[8].value[0] = zone_merges->value[14];
            RawDataInfo->attributes.data[8].value[1] = zone_merges->value[15];
            RawDataInfo->attributes.data[8].value[2] = zone_merges->value[16];
            RawDataInfo->attributes.data[8].value[3] = zone_merges->value[17];
        }

        // RawDataInfo2 = RawDataInfo + sizeof(RawDataInfo_t);
        // printf("iot. 3 RawDataInfo_t size = %lu \n", sizeof(RawDataInfo_t));
        if (j < deviceNumTemp - 1)
        {
            RawDataInfo++; // # 指標加法,使用請小心,務必法指標放回原來位置
            zone_merges++;
        }
#ifdef DEBUGLPRINT
        printf("iot. 4 RawDataInfo address = %p \n", RawDataInfo);
#endif
    }

    if (deviceNumTemp > 0)
    {
        RawDataInfo = RawDataInfo - (deviceNumTemp - 1); // 指標放回原來位置
        zone_merges = zone_merges - (deviceNumTemp - 1);
    }
#ifdef DEBUGLPRINT
    printf("iot. 4 RawDataInfo address = %p \n", RawDataInfo);
#endif
    //============ Free ===========================

    if (deviceNumTemp != 0)
        free(zone_merges);

    if (deviceNumZoneStatus != 0)
        free(zone_status);
    free(p_flash_data);
    return deviceNumTemp;
}

char *DeviceStatusTran(char *p_value, char type)
{
    static char valueStr[5];
    memset(valueStr, 0, sizeof(valueStr));
    valueStr[4] = '\0';

#ifdef DEBUGLPRINT
    printf("iot. type = %d \n", type);
    printf("iot. p_value[0] (>0:on ; =0:off)= %d \n", p_value[0]);
#endif

    switch (type)
    {
    case 1:
    case 8:
    case 10:
    case 11:
    case 13:
    case 23:
    case 27:
    case 28:
    case 34:
    case 35:
    case 42:
    case 52:
    case 96:
    case 97:
    case 103:
        if (p_value[0] == 1)
            strcpy(valueStr, "on");
        else
        {
            strcpy(valueStr, "off");
        }
        break;
    default:
        // 因為要返回1或0 所以調成不是0的都是1
        if (p_value[0] > 0)
            strcpy(valueStr, "on");
        else
        {
            strcpy(valueStr, "off");
        }
        break;
    }

#ifdef DEBUGLPRINT
    printf("iot. =========valueStr===== %s \n", valueStr);
#endif

    return valueStr;
}

static int print_preallocated(cJSON *root)
{
    /* declarations */
    char *out = NULL;
    // char *buf = NULL;
    char *buf_fail = NULL;
    size_t len = 0;
    size_t len_fail = 0;

    /* formatted print */
    out = cJSON_Print(root);

    /* create buffer to succeed */
    /* the extra 5 bytes are because of inaccuracies when reserving memory */
    len = strlen(out) + 5;
    buf = (char *)malloc(len);
    bufFlag = 1;
    if (buf == NULL)
    {
        printf("iot. Failed to allocate memory.\n");
        exit(1); // todo 程式不能跑這裡
    }

    /* create buffer to fail */
    len_fail = strlen(out);
    buf_fail = (char *)malloc(len_fail);
    if (buf_fail == NULL)
    {
        printf("iot. Failed to allocate memory.\n");
        exit(1); // todo 程式不能跑這裡
    }

    /* Print to buffer */
    if (!cJSON_PrintPreallocated(root, buf, (int)len, 1))
    {
        printf("iot. cJSON_PrintPreallocated failed!\n");
        if (strcmp(out, buf) != 0)
        {
            printf("iot. cJSON_PrintPreallocated not the same as cJSON_Print!\n");
            printf("iot. cJSON_Print result:\n%s\n", out);
            printf("iot. cJSON_PrintPreallocated result:\n%s\n", buf);
        }
        free(out);
        free(buf_fail);
        // free(buf);
        return -1;
    }

    /* success */
    // printf("iot. %s\n", buf);  //# 不要print

    /* force it to fail */
    if (cJSON_PrintPreallocated(root, buf_fail, (int)len_fail, 1))
    {
        printf("iot. cJSON_PrintPreallocated failed to show error with insufficient memory!\n");
        printf("iot. cJSON_Print result:\n%s\n", out);
        printf("iot. cJSON_PrintPreallocated result:\n%s\n", buf_fail);
        free(out);
        free(buf_fail);
        // free(buf);
        return -1;
    }

    free(out);
    free(buf_fail);
    // free(buf);

    return 0;
}

// # 產生JSON
int CJSON_CDECL create_JSONobjects(unsigned char index)
{
    // todo 將資料結構化
    char bat_val[36] = {0};

    cJSON *root = NULL;
    cJSON *att = NULL;
    cJSON *raw = NULL;
    cJSON *raw1 = NULL;
    cJSON *raw1_1 = NULL;
    cJSON *att1 = NULL;
    cJSON *att2 = NULL;
    // cJSON *att3 = NULL;
    cJSON *data1 = NULL;

    int i;
    double fbat, fbat2;
    unsigned short temp;
    int deviceNum2;

    root = cJSON_CreateObject();
    // printf("iot. A01 DeviceInfo.id= %s\n", DeviceInfo.id);
    cJSON_AddStringToObject(root, "MNO", DeviceInfo.id);
    // printf("iot. A02 DeviceInfo.macAddress= %s\n", DeviceInfo.macAddress);
    cJSON_AddStringToObject(root, "MAC", DeviceInfo.macAddress);
    cJSON_AddStringToObject(root, "rawdataTime", DeviceInfo.rawdataTime);
    if (index == 0)
        cJSON_AddItemToObject(root, "loopArray", cJSON_CreateIntArray(DeviceInfo.attributes.looparray, deviceNum)); // 數值
    cJSON_AddItemToObject(root, "attributes", att = cJSON_CreateObject());
    cJSON_AddStringToObject(att, "mnoVersion", DeviceInfo.attributes.version);
    cJSON_AddStringToObject(att, "mnoFirmware", DeviceInfo.attributes.firmware);
    cJSON_AddStringToObject(att, "mnoType", DeviceInfo.attributes.type);
    cJSON_AddStringToObject(att, "mnoModel", DeviceInfo.attributes.model);
    char combinedTypeAndFirmware[50];
    cJSON_AddItemToObject(root, "rawdatalist", raw = cJSON_CreateArray());

#ifdef DEBUGLPRINT
    printf("iot. deviceNum = %d\n", deviceNum);
    printf("iot. index = %d\n", index);
#endif

    if ((deviceNum - (index * 10)) > 9)
        deviceNum2 = (index + 1) * 10;
    else
        deviceNum2 = deviceNum;
    for (i = index * 10; i < deviceNum2; i++)
    {
        cJSON_AddItemToObject(raw, "attributes", att1 = cJSON_CreateObject());
        cJSON_AddStringToObject(att1, "id", RawDataInfo[i].id);
        cJSON_AddStringToObject(att1, "manufacturerId", RawDataInfo->manufacturerId);
        cJSON_AddStringToObject(att1, "rawdataTime", RawDataInfo[i].rawdataTime);
        cJSON_AddItemToObject(att1, "attributes", att2 = cJSON_CreateObject());
        cJSON_AddStringToObject(att2, "name", RawDataInfo[i].attributes.name);
        cJSON_AddStringToObject(att2, "deviceName", RawDataInfo[i].attributes.devDescp);
        cJSON_AddNumberToObject(att2, "zoneNumber", RawDataInfo[i].attributes.devPlaceNumZone + 1); // 檔案讀出的數值從0開始數,顯示從1數
        // 將firmware做判斷 避免主機異常出現異常內容 只允許下面所表示之字符出現 其餘字符一律判斷為abnormal
        char *firmware = RawDataInfo[i].attributes.firmware;
        int is_valid = 1;
        int j;
        for (j = 0; j < strlen(firmware); j++)
        {
            if (!(isalnum(firmware[j]) || firmware[j] == '_' || firmware[j] == '/' || firmware[j] == '.' || firmware[j] == '-' || firmware[j] == ' '))
            {
                is_valid = 0;
                break;
            }
        }
        // 如果firmware為不正常的FW，則將其設為 "abnormal"
        if (!is_valid)
        {
            strcpy(firmware, "abnormal");
        }
        // 將type和經過檢查/修正的firmware合併到一個字符串
        sprintf(combinedTypeAndFirmware, "%s,%s", RawDataInfo[i].attributes.type, firmware);
        // 將合併後的字符串作為machType的值
        cJSON_AddStringToObject(att2, "machType", combinedTypeAndFirmware);
        // cJSON_AddStringToObject(att2, "machType", RawDataInfo[i].attributes.type);
        cJSON_AddStringToObject(att2, "firmware", firmware);
        // cJSON_AddStringToObject(att2, "batch", RawDataInfo->attributes.batch);
        // cJSON_AddStringToObject(att2, "SetupDate", RawDataInfo->attributes.SetupDate);
        // cJSON_AddStringToObject(att2, "MFD", RawDataInfo->attributes.MFD);
        cJSON_AddNumberToObject(att2, "Loop", RawDataInfo[i].attributes.Loop1);
        cJSON_AddItemToObject(att1, "data", raw1 = cJSON_CreateArray());

        //-------------------------------------------------------------------------------------------------------
        cJSON_AddItemToObject(raw1, "attributes", data1 = cJSON_CreateObject());
        cJSON_AddStringToObject(data1, "sensor", RawDataInfo[i].attributes.data[0].sensor);
        cJSON_AddStringToObject(data1, "label", RawDataInfo[i].attributes.data[0].label);
        cJSON_AddItemToObject(data1, "value", raw1_1 = cJSON_CreateArray());
        // cJSON_AddStringToObject(raw1_1, "", RawDataInfo->attributes.data[0].value);
        cJSON_AddNumberToObject(raw1_1, "", RawDataInfo[i].attributes.data[0].value[0]);
        cJSON_AddStringToObject(data1, "time", RawDataInfo[i].attributes.data[0].time);
        cJSON_AddStringToObject(data1, "unit", RawDataInfo[i].attributes.data[0].unit);
        cJSON_AddStringToObject(data1, "dimension", RawDataInfo->attributes.data[0].dimension);

        //-------------------------------------------------------------------------------------------------------
        cJSON_AddItemToObject(raw1, "attributes", data1 = cJSON_CreateObject());
        cJSON_AddStringToObject(data1, "sensor", RawDataInfo[i].attributes.data[1].sensor);
        cJSON_AddStringToObject(data1, "label", RawDataInfo[i].attributes.data[1].label);
        cJSON_AddItemToObject(data1, "value", raw1_1 = cJSON_CreateArray());
// todo 轉換程電壓寫這裡不好, 目前也沒比較好方法
#ifdef DEBUGLPRINT
        printf("iot. RawDataInfo->attributes.data[1].value[1] = %d \n", RawDataInfo[i].attributes.data[1].value[1]);
#endif
        memset(bat_val, 0, sizeof(bat_val));
        // sprintf(bat_val, "%d.%d", (((RawDataInfo[i].attributes.data[0].value[1] & 0xE0) >> 2) | ((RawDataInfo[i].attributes.data[0].value[1] & 0x0E) >> 1)) / 10,
        //         (((RawDataInfo[i].attributes.data[0].value[1] & 0xE0) >> 2) | ((RawDataInfo[i].attributes.data[0].value[1] & 0x0E) >> 1)) % 10);
        sprintf(bat_val, "%d.%d", (((RawDataInfo[i].attributes.data[1].value[1] & 0xF0) >> 1) | (RawDataInfo[i].attributes.data[1].value[1] & 0x0F)) / 10, (((RawDataInfo[i].attributes.data[1].value[1] & 0xF0) >> 1) | (RawDataInfo[i].attributes.data[1].value[1] & 0x0F)) % 10);
        fbat = atof(bat_val);
#ifdef DEBUGLPRINT
        printf("iot. Device Voltage: %f\n", fbat);
#endif

        fbat2 = round(fbat * 10.0) / 10.0;
        // sprintf(voltageTemp, "%f", fbat);
        if (RawDataInfo[i].type == 11) // WPW
        {
            cJSON_AddNumberToObject(raw1_1, "", RawDataInfo[i].attributes.data[1].value[1]);
        }
        else
        {
            cJSON_AddNumberToObject(raw1_1, "", fbat2); // todo 這裡要判斷dimension
        }
        cJSON_AddStringToObject(data1, "time", RawDataInfo[i].attributes.data[1].time);
        cJSON_AddStringToObject(data1, "unit", RawDataInfo[i].attributes.data[1].unit);
        cJSON_AddStringToObject(data1, "dimension", RawDataInfo[i].attributes.data[1].dimension);
        //------------------------------------------------------------------------------------------------------

        cJSON_AddItemToObject(raw1, "attributes", data1 = cJSON_CreateObject());
        cJSON_AddStringToObject(data1, "sensor", RawDataInfo[i].attributes.data[2].sensor);
        cJSON_AddStringToObject(data1, "label", RawDataInfo[i].attributes.data[2].label);
        cJSON_AddItemToObject(data1, "value", raw1_1 = cJSON_CreateArray());
#ifdef DEBUGLPRINT
        printf("iot. RawDataInfo[i].attributes.data[2].value[0] = %d \n", RawDataInfo[i].attributes.data[2].value[0]);
#endif
        cJSON_AddNumberToObject(raw1_1, "", RawDataInfo[i].attributes.data[2].value[0]);
        cJSON_AddStringToObject(data1, "time", RawDataInfo[i].attributes.data[2].time);
        cJSON_AddStringToObject(data1, "unit", RawDataInfo[i].attributes.data[2].unit);
        cJSON_AddStringToObject(data1, "dimension", RawDataInfo[i].attributes.data[2].dimension);
        //------------------------------------------------------------------------------------------------------

        cJSON_AddItemToObject(raw1, "attributes", data1 = cJSON_CreateObject());
        cJSON_AddStringToObject(data1, "sensor", RawDataInfo[i].attributes.data[3].sensor);
        cJSON_AddStringToObject(data1, "label", RawDataInfo[i].attributes.data[3].label);
        cJSON_AddItemToObject(data1, "value", raw1_1 = cJSON_CreateArray());
#ifdef DEBUGLPRINT
        printf("iot. RawDataInfo[i].attributes.data[3].value[0] = %s \n", RawDataInfo[i].attributes.data[3].value);
#endif
        // cJSON_AddStringToObject(raw1_1, "", RawDataInfo[i].attributes.data[3].value);
        cJSON_AddNumberToObject(raw1_1, "", RawDataInfo[i].attributes.data[3].value[0]);
        cJSON_AddStringToObject(data1, "time", RawDataInfo[i].attributes.data[3].time);
        cJSON_AddStringToObject(data1, "dimension", RawDataInfo[i].attributes.data[3].dimension);

        //-----------------STH------------------
        if (RawDataInfo[i].type == 22)
        {
            cJSON_AddItemToObject(raw1, "attributes", data1 = cJSON_CreateObject());
            cJSON_AddStringToObject(data1, "sensor", RawDataInfo[i].attributes.data[4].sensor);
            cJSON_AddStringToObject(data1, "label", RawDataInfo[i].attributes.data[4].label);
            cJSON_AddItemToObject(data1, "value", raw1_1 = cJSON_CreateArray());
            temp = ((unsigned char)RawDataInfo[i].attributes.data[4].value[0] * 256 + (unsigned char)RawDataInfo[i].attributes.data[4].value[1]) / 10;
            cJSON_AddNumberToObject(raw1_1, "", temp * 0.1);
            cJSON_AddStringToObject(data1, "time", RawDataInfo[i].attributes.data[4].time);
            cJSON_AddStringToObject(data1, "unit", RawDataInfo[i].attributes.data[4].unit);
            cJSON_AddStringToObject(data1, "dimension", RawDataInfo->attributes.data[4].dimension);

            cJSON_AddItemToObject(raw1, "attributes", data1 = cJSON_CreateObject());
            cJSON_AddStringToObject(data1, "sensor", RawDataInfo[i].attributes.data[5].sensor);
            cJSON_AddStringToObject(data1, "label", RawDataInfo[i].attributes.data[5].label);
            cJSON_AddItemToObject(data1, "value", raw1_1 = cJSON_CreateArray());
            temp = ((unsigned char)RawDataInfo[i].attributes.data[5].value[0] * 256 + (unsigned char)RawDataInfo[i].attributes.data[5].value[1]) / 10;
            cJSON_AddNumberToObject(raw1_1, "", temp * 0.1);
            cJSON_AddStringToObject(data1, "time", RawDataInfo[i].attributes.data[5].time);
            cJSON_AddStringToObject(data1, "unit", RawDataInfo[i].attributes.data[5].unit);
            cJSON_AddStringToObject(data1, "dimension", RawDataInfo->attributes.data[5].dimension);
        }

        //-----------------WPW------------------
        if (RawDataInfo[i].type == 11)
        {
            cJSON_AddItemToObject(raw1, "attributes", data1 = cJSON_CreateObject());
            cJSON_AddStringToObject(data1, "sensor", RawDataInfo[i].attributes.data[4].sensor);
            cJSON_AddStringToObject(data1, "label", RawDataInfo[i].attributes.data[4].label);
            cJSON_AddItemToObject(data1, "value", raw1_1 = cJSON_CreateArray());
            // printf("iot. RawDataInfo[i].attributes.data[4].value[0] = %d \n", RawDataInfo[i].attributes.data[4].value[0]);
            cJSON_AddNumberToObject(raw1_1, "", (unsigned char)RawDataInfo[i].attributes.data[4].value[0] * 0.1);
            cJSON_AddStringToObject(data1, "time", RawDataInfo[i].attributes.data[4].time);
            cJSON_AddStringToObject(data1, "unit", RawDataInfo[i].attributes.data[4].unit);
            cJSON_AddStringToObject(data1, "dimension", RawDataInfo->attributes.data[4].dimension);

            cJSON_AddItemToObject(raw1, "attributes", data1 = cJSON_CreateObject());
            cJSON_AddStringToObject(data1, "sensor", RawDataInfo[i].attributes.data[5].sensor);
            cJSON_AddStringToObject(data1, "label", RawDataInfo[i].attributes.data[5].label);
            cJSON_AddItemToObject(data1, "value", raw1_1 = cJSON_CreateArray());
#ifdef DEBUGLPRINT
            printf("iot. RawDataInfo[i].attributes.data[4].value[0] = %d \n", RawDataInfo[i].attributes.data[5].value[0]);
#endif
            cJSON_AddNumberToObject(raw1_1, "", RawDataInfo[i].attributes.data[5].value[0]);
            cJSON_AddStringToObject(data1, "time", RawDataInfo[i].attributes.data[5].time);
            cJSON_AddStringToObject(data1, "unit", RawDataInfo[i].attributes.data[5].unit);
            cJSON_AddStringToObject(data1, "dimension", RawDataInfo[i].attributes.data[5].dimension);

            cJSON_AddItemToObject(raw1, "attributes", data1 = cJSON_CreateObject());
            cJSON_AddStringToObject(data1, "sensor", RawDataInfo[i].attributes.data[6].sensor);
            cJSON_AddStringToObject(data1, "label", RawDataInfo[i].attributes.data[6].label);
            cJSON_AddItemToObject(data1, "value", raw1_1 = cJSON_CreateArray());
            temp = (unsigned char)RawDataInfo[i].attributes.data[6].value[0] * 256 + (unsigned char)RawDataInfo[i].attributes.data[6].value[1];
            // printf("iot. RawDataInfo[i].attributes.data[6].value[0] = %d \n", temp * 0.1);
            cJSON_AddNumberToObject(raw1_1, "", temp * 0.1);
            cJSON_AddStringToObject(data1, "time", RawDataInfo[i].attributes.data[6].time);
            cJSON_AddStringToObject(data1, "unit", RawDataInfo[i].attributes.data[6].unit);
            cJSON_AddStringToObject(data1, "dimension", RawDataInfo[i].attributes.data[6].dimension);
        }
    }
    if (print_preallocated(root) != 0)
    {
        cJSON_Delete(root);
        exit(EXIT_FAILURE); // todo 程式不能跑這裡,要處理
    }
    cJSON_Delete(root);
    return 0;
}

char *getMACAddress(void)
{
    static char mac_addr[MAX_MAC_LEN] = {0};
    struct ifreq ifr;
    int fd;

    // get mac addr
    // Open a socket
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1)
    {
        perror("socket");
        // return -1; // todo
    }

#ifdef DEBUGLPRINT
    printf("iot. getting Mac address... \n");
#endif

#ifdef VM
    strncpy(ifr.ifr_name, "enp0s3", IFNAMSIZ - 1);
#else
    strncpy(ifr.ifr_name, "wlan0", IFNAMSIZ - 1);
#endif

    if (ioctl(fd, SIOCGIFHWADDR, &ifr) == -1)
    {
        perror("ioctl");
        close(fd);
        // return -1; // todo
    }
    close(fd);
    // printf("iot. ok\n");

    sprintf(mac_addr, "%02X%02X%02X%02X%02X%02X",
            (unsigned char)ifr.ifr_hwaddr.sa_data[0],
            (unsigned char)ifr.ifr_hwaddr.sa_data[1],
            (unsigned char)ifr.ifr_hwaddr.sa_data[2],
            (unsigned char)ifr.ifr_hwaddr.sa_data[3],
            (unsigned char)ifr.ifr_hwaddr.sa_data[4],
            (unsigned char)ifr.ifr_hwaddr.sa_data[5]);

#ifdef DEBUGLPRINT
    printf("iot. Got MAC Address: %s\n", mac_addr);
#endif

    return mac_addr;
}

DeviceInfo_t InitialDeviceInfoValue(DeviceInfo_t DeviceInfo)
{
#ifdef VM
    strcpy(DeviceInfo.id, "0780015");
    strcpy(DeviceInfo.macAddress, "A02EF3009DB0");
    strcpy(DeviceInfo.rawdataTime, timestring_TimeFormat);
    strcpy(DeviceInfo.attributes.version, "1.0");
    strcpy(DeviceInfo.attributes.firmware, "079C 2023-12-07"); // 固定,每次進板要跟Ben確認
    strcpy(DeviceInfo.attributes.type, "Gateway");
    strcpy(DeviceInfo.attributes.model, "NGW400");
#else
    // strcpy(DeviceInfo.id, p_flash_data->apmib_mno);
    strcpy(DeviceInfo.rawdataTime, timestring_TimeFormat);
    strcpy(DeviceInfo.attributes.version, "1.0");
    // strcpy(DeviceInfo.attributes.firmware, FIRMWARE_VER2);
#ifndef IOTMQTT
    strcpy(DeviceInfo.attributes.firmware, "090C 2024-12-10 !測試區版本!"); // 固定,每次進板要跟Ben確認
    printf("iot. ~測試區rabbitmq~   ~測試區rabbitmq~    ~測試區rabbitmq~");
#else
    strcpy(DeviceInfo.attributes.firmware, VERSION); // 固定,每次進板要跟Ben確認
#endif
    strcpy(DeviceInfo.attributes.type, "Gateway");

    DIR *dp = opendir("/tmp/usb/sda1"); // 判斷 sda1 是否存在
    if (dp == NULL)
    {
// 無法開啟目錄，處理錯誤情況
#ifdef DEBUGLPRINT
        printf("iot.Directory /tmp/usb/sda1 not found.\n");
#endif
        strcpy(DeviceInfo.attributes.model, "NGW400"); // 預設為 "NGW400"
    }
    else
    {
        struct dirent *entry;
        strcpy(DeviceInfo.attributes.model, "NGW400"); // 預設為 "NGW400"

        while ((entry = readdir(dp)) != NULL)
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                // 找到任何項目
                strcpy(DeviceInfo.attributes.model, "NGW400_USB");
                break;
            }
        }

        closedir(dp); // 關閉目錄
    }

#endif
    return DeviceInfo;
}

void InitialRawDataInfoValue(int deviceNum, RawDataInfo_t *RawDataInfo)
{
    // RawDataInfo_t RawDataInfo2 = RawDataInfo;
    int j;
    for (j = 0; j < deviceNum; j++)
    {
        printf("iot. 2 RawDataInfo_t size = %p \n", RawDataInfo);
        strcpy(RawDataInfo->id, "A000001");
        strcpy(RawDataInfo->manufacturerId, "SKS");
        strcpy(RawDataInfo->rawdataTime, timestring_TimeFormat);
        strcpy(RawDataInfo->attributes.name, "reed sensor");
        strcpy(RawDataInfo->attributes.type, "WSI999");
        strcpy(RawDataInfo->attributes.firmware, "1.2");
        strcpy(RawDataInfo->attributes.batch, "001");
        strcpy(RawDataInfo->attributes.SetupDate, "20230321");
        strcpy(RawDataInfo->attributes.MFD, "20230321");
        RawDataInfo->attributes.Loop1 = 1;
        strcpy(RawDataInfo->attributes.data[1].sensor, "voltage");
        strcpy(RawDataInfo->attributes.data[1].label, "裝置電壓");
        strcpy(RawDataInfo->attributes.data[1].time, timestring_TimeFormat);
        strcpy(RawDataInfo->attributes.data[1].unit, "V");
        strcpy(RawDataInfo->attributes.data[1].dimension, "1");
        RawDataInfo->attributes.data[1].value[1] = 7;

        strcpy(RawDataInfo->attributes.data[2].sensor, "RSSI");
        strcpy(RawDataInfo->attributes.data[2].label, "信號強度");
        strcpy(RawDataInfo->attributes.data[2].time, timestring_TimeFormat);
        strcpy(RawDataInfo->attributes.data[2].unit, "dB");
        strcpy(RawDataInfo->attributes.data[2].dimension, "1");
        RawDataInfo->attributes.data[2].value[0] = -36;

        strcpy(RawDataInfo->attributes.data[3].sensor, "action");
        strcpy(RawDataInfo->attributes.data[3].label, "裝置動作");
        strcpy(RawDataInfo->attributes.data[3].time, timestring_TimeFormat);
        strcpy(RawDataInfo->attributes.data[3].unit, "\0");
        strcpy(RawDataInfo->attributes.data[3].dimension, "1");
        strcpy(RawDataInfo->attributes.data[3].value, "on"); // todo value 暫兩Byte
        // RawDataInfo2 = RawDataInfo + sizeof(RawDataInfo_t);
        // printf("iot. 3 RawDataInfo_t size = %lu \n", sizeof(RawDataInfo_t));
        if (j < deviceNum - 1)
            RawDataInfo = RawDataInfo + 1;
        printf("iot. 4 RawDataInfo_t size = %p \n", RawDataInfo);
    }
}

void PrintDeviceInfo(DeviceInfo_t DeviceInfo)
{
    printf("iot. ===== Device Information ====== \n");
    printf("iot. DeviceInfoID = %s \n", DeviceInfo.id);
    printf("iot. DeviceInfoTime = %s \n", DeviceInfo.rawdataTime);
    printf("iot. DeviceInfoVersion = %s \n", DeviceInfo.attributes.version);
    printf("iot. DeviceInfoFirmware = %s \n", DeviceInfo.attributes.firmware);
    printf("iot. DeviceInfoType = %s \n", DeviceInfo.attributes.type);
    printf("iot. ===== Device Information End====== \n\n");
}

void PrintRawDataInfo(int deviceNum, RawDataInfo_t *RawDataInfo)
{
    int j, k;
    for (j = 0; j < deviceNum; j++)
    {
        printf("iot. ===== Raw Data Information %d====== \n", j);
        printf("iot. RawDataInfoID = %s \n", RawDataInfo->id);
        printf("iot. RawDataInfomanufacturerId = %s \n", RawDataInfo->manufacturerId);
        printf("iot. RawDataInfoTime = %s \n", RawDataInfo->rawdataTime);
        printf("iot. RawDataInfoAttrName = %s \n", RawDataInfo->attributes.name);
        printf("iot. RawDataInfoAttrType = %s \n", RawDataInfo->attributes.type);
        printf("iot. RawDataInfoAttrFirmware = %s \n", RawDataInfo->attributes.firmware);
        printf("iot. RawDataInfoAttrBatch = %s \n", RawDataInfo->attributes.batch);
        printf("iot. RawDataInfoAttrSetupDate = %s \n", RawDataInfo->attributes.SetupDate);
        printf("iot. RawDataInfoAttrMFD = %s \n", RawDataInfo->attributes.MFD);
        printf("iot. RawDataInfoAttrLoop = %d \n", RawDataInfo->attributes.Loop1);

        for (k = 0; k < 3; k++)
        {
            printf("iot. RawDataInfoSensor = %s \n", RawDataInfo->attributes.data[k].sensor);
            printf("iot. RawDataInfoLabel = %s \n", RawDataInfo->attributes.data[k].label);
            printf("iot. RawDataInfoTime = %s \n", RawDataInfo->attributes.data[k].time);
            printf("iot. RawDataInfoLabel = %s \n", RawDataInfo->attributes.data[k].unit);
            printf("iot. RawDataInfoDimension = %s \n", RawDataInfo->attributes.data[k].dimension);
            if (k == 2)
            {
                printf("iot. RawDataInfovalValue = %s \n", RawDataInfo->attributes.data[k].value);
            }
            else if (k == 1)
            {
                printf("iot. RawDataInfoValue = %i \n", RawDataInfo->attributes.data[k].value[0]);
            }
            else
            {
                printf("iot. RawDataInfoValue = %d \n", RawDataInfo->attributes.data[k].value[0]);
            }
        }
        printf("iot. ===== Raw Data Information End %d====== \n\n", j);
    }
}

char *IndexToNameString(unsigned char nameIndex)
{
    static char name[30];
    memset(name, 0, sizeof(name));
    switch (nameIndex)
    {
    case 0:
        strcpy(name, "NGW_240");
        break;
    case 1:
        strcpy(name, "ZB磁鐵");
        break;
    case 2:
        strcpy(name, "ZB一氧化碳偵測");
        break;
    case 3:
        strcpy(name, "Temperature Sensor");
        break;
    case 4:
        strcpy(name, "Humidity Sensor");
        break;
    case 5:
        strcpy(name, "Glass sensor");
        break;
    case 6:
        strcpy(name, "ZB瓦斯偵測");
        break;
    case 7:
        strcpy(name, "ZB瓦斯偵測");
        break;
    case 8:
        strcpy(name, "ZB紅外線");
        break;
    case 9:
        strcpy(name, "ThermoPile");
        break;
    case 10:
        strcpy(name, "ZB偵煙");
        break;
    case 11:
        strcpy(name, "ZB智慧插座");
        break;
    case 12:
        strcpy(name, "ZB警報器");
        break;
    case 13:
        strcpy(name, "ZB有線轉無線");
        break;
    case 14:
        strcpy(name, "類比訊號轉換器");
        break;
    case 15:
        strcpy(name, "電鎖控制器");
        break;
    case 16:
        strcpy(name, "ZB遙控器");
        break;
    case 17:
        strcpy(name, "CPN260");
        break;
    case 18:
        strcpy(name, "Wireless Router");
        break;
    case 19:
        strcpy(name, "迴路控制器");
        break;
    case 20:
        strcpy(name, "終端控制器");
        break;
    case 21:
        strcpy(name, "電源迴路");
        break;
    case 22:
        strcpy(name, "ZB溫濕度偵測");
        break;
    case 23:
        strcpy(name, "RS485_RS232");
        break;
    case 24:
        strcpy(name, "Mobile Dialer");
        break;
    case 25:
        strcpy(name, "二氧化碳偵測器");
        break;
    case 26:
        strcpy(name, "WPL100");
        break;
    case 27:
        strcpy(name, "ZB磁鐵");
        break;
    case 28:
        strcpy(name, "ZB紅外線");
        break;
    case 29:
        strcpy(name, "ZB紅外線");
        break;
    case 30:
        strcpy(name, "紅外線控制器");
        break;
    case 31:
        strcpy(name, "照明");
        break;
    case 32:
        strcpy(name, "ZB一氧化碳偵測");
        break;
    case 33:
        strcpy(name, "ZB讀卡機");
        break;
    case 34:
        strcpy(name, "ZB磁鐵");
        break;
    case 35:
        strcpy(name, "ZB紅外線");
        break;
    case 36:
        strcpy(name, "紅外線報警器");
        break;
    case 37:
        strcpy(name, "電燈控制");
        break;
    case 38:
        strcpy(name, "窗簾控制");
        break;
    case 39:
        strcpy(name, "電燈控制");
        break;
    case 40:
        strcpy(name, "窗簾控制");
        break;
    case 41:
        strcpy(name, "IPCam");
        break;
    case 42:
        strcpy(name, "SMT_230N");
        break;
    case 45:
        strcpy(name, "ZB緊急按鈕");
        break;
    case 46:
        strcpy(name, "ZB鐵捲門控制器");
        break;
    case 47:
        strcpy(name, "CNT");
        break;
    case 48:
        strcpy(name, "警報按鈕L");
        break;
    case 49:
        strcpy(name, "門鈴讀卡機");
        break;
    case 50:
        strcpy(name, "RF紅外線轉發器");
        break;
    case 51:
        strcpy(name, "ZB出力裝置");
        break;
    case 52:
        strcpy(name, "無線控制門鎖");
        break;
    case 53:
        strcpy(name, "漏水偵測器");
        break;
    case 54:
        strcpy(name, "激光罩杯");
        break;
    case 80:
        strcpy(name, "MOBILE_APP");
        break;
    case 81:
        strcpy(name, "IP_CAMERA");
        break;
    case 96:
        strcpy(name, "RF磁鐵");
        break;
    case 97:
        strcpy(name, "RF紅外線");
        break;
    case 98:
        strcpy(name, "RF緊急按鈕");
        break;
    case 99:
        strcpy(name, "RF數位訊號轉換器");
        break;
    case 100:
        strcpy(name, "未定義裝置");
        break;
    case 101:
        strcpy(name, "RF中繼器");
        break;
    case 102:
        strcpy(name, "RF讀卡機");
        break;
    case 103:
        strcpy(name, "RF耶魯鎖模組");
        break;
    case 104:
        strcpy(name, "CT_300");
        break;
    case 105:
        strcpy(name, "WAL_300");
        break;
    case 107:
        strcpy(name, "RF鐵捲門控制器");
        break;
    default:
        strcpy(name, "未定義裝置");
        break;
    }
    return name;
}

char *IndexToTypeString(unsigned char nameIndex)
{
    static char name[30];
    memset(name, 0, sizeof(name));
    switch (nameIndex)
    {
    case 0:
        strcpy(name, "NGW_240");
        break;
    case 1:
        strcpy(name, "SCT_220");
        break;
    case 2:
        strcpy(name, "WCD");
        break;
    case 3:
        strcpy(name, "STP_220");
        break;
    case 4:
        strcpy(name, "WHS");
        break;
    case 5:
        strcpy(name, "WGS");
        break;
    case 6:
        strcpy(name, "WGD");
        break;
    case 7:
        strcpy(name, "WGDD");
        break;
    case 8:
        strcpy(name, "SMT_220");
        break;
    case 9:
        strcpy(name, "WTP");
        break;
    case 10:
        strcpy(name, "SSM_220");
        break;
    case 11:
        strcpy(name, "WPW_220");
        break;
    case 12:
        strcpy(name, "WAL_220");
        break;
    case 13:
        strcpy(name, "TDI_220");
        break;
    case 14:
        strcpy(name, "TAI_220");
        break;
    case 15:
        strcpy(name, "WLO_220");
        break;
    case 16:
        strcpy(name, "CBT_220");
        break;
    case 17:
        strcpy(name, "CPN260");
        break;
    case 18:
        strcpy(name, "NRT_220");
        break;
    case 19:
        strcpy(name, "WRL_220");
        break;
    case 20:
        strcpy(name, "CPN_240C");
        break;
    case 21:
        strcpy(name, "WRL_240");
        break;
    case 22:
        strcpy(name, "STH_220");
        break;
    case 23:
        strcpy(name, "WRS_220");
        break;
    case 24:
        strcpy(name, "DMB_220");
        break;
    case 25:
        strcpy(name, "SCO_220");
        break;
    case 26:
        strcpy(name, "WPL100");
        break;
    case 27:
        strcpy(name, "SCT_230");
        break;
    case 28:
        strcpy(name, "SMT_250");
        break;
    case 29:
        strcpy(name, "SGR_220");
        break;
    case 30:
        strcpy(name, "SIR_220");
        break;
    case 31:
        strcpy(name, "WLL_220");
        break;
    case 32:
        strcpy(name, "SCO_230");
        break;
    case 33:
        strcpy(name, "WSI_220");
        break;
    case 34:
        strcpy(name, "SCT_240");
        break;
    case 35:
        strcpy(name, "SMT_240");
        break;
    case 36:
        strcpy(name, "WAL_250");
        break;
    case 37:
        strcpy(name, "SKL_220");
        break;
    case 38:
        strcpy(name, "SKC_220");
        break;
    case 39:
        strcpy(name, "SKL_230");
        break;
    case 40:
        strcpy(name, "SKC_210");
        break;
    case 41:
        strcpy(name, "unknown");
        break;
    case 42:
        strcpy(name, "SMT_230N");
        break;
    case 45:
        strcpy(name, "CPT_220");
        break;
    case 46:
        strcpy(name, "WRL_250");
        break;
    case 47:
        strcpy(name, "CNT_220");
        break;
    case 48:
        strcpy(name, "CPT_220LR");
        break;
    case 49:
        strcpy(name, "DSI_220");
        break;
    case 50:
        strcpy(name, "WIR_220");
        break;
    case 51:
        strcpy(name, "FORCE_220");
        break;
    case 52:
        strcpy(name, "WRS_232");
        break;
    case 53:
        strcpy(name, "WLK_220");
        break;
    case 54:
        strcpy(name, "WCP_220");
        break;
    case 80:
        strcpy(name, "MOBILE_APP");
        break;
    case 81:
        strcpy(name, "IP_CAMERA");
        break;
    case 96:
        strcpy(name, "SCT_300");
        break;
    case 97:
        strcpy(name, "SMT_300");
        break;
    case 98:
        strcpy(name, "CPT_300");
        break;
    case 99:
        strcpy(name, "WDI_300");
        break;
    case 100:
        strcpy(name, "未定義裝置");
        break;
    case 101:
        strcpy(name, "RPT_300");
        break;
    case 102:
        strcpy(name, "WSI_300");
        break;
    case 103:
        strcpy(name, "WRS_300");
        break;
    case 104:
        strcpy(name, "CT_300");
        break;
    case 105:
        strcpy(name, "WAL_300");
        break;
    case 107:
        strcpy(name, "WRL-300");
        break;
    default:
        strcpy(name, "未定義裝置");
        break;
    }
    return name;
}

void WriteToLogFile(char *pArg)
{
    int msgLen;
    msgLen = strlen(pArg);
    FILE *fp_WriteToLog;

#ifdef VM
    fp_WriteToLog = fopen(Loacl_name_log, "a");
#else
    fp_WriteToLog = fopen(UIS_name_log, "a");
#endif
    fwrite(pArg, msgLen, 1, fp_WriteToLog);

    // free resource
    fclose(fp_WriteToLog);
}
