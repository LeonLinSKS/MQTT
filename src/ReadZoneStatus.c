
// gcc -I ./include ReadZoneStatus.c -g -o ReadZoneStatus
// msdk-linux-gcc -I ./include ReadZoneStatus.c -g -o ReadZoneStatus
// valgrind --leak-check=full ./ReadZoneStatus

#include "common.h"
#include "ReadZoneStatus.h"

#ifdef VM
typedef unsigned char u_int8_t;
#endif

#ifdef DEBUG
int main(void)
{
    int sensorCount;
    Zone_Device_Status_RD *zone_status;

    printf("iot. Version: %s\n", "1.0.0");

    sensorCount = ReadZoneStatusSize(); // 讀取檔案中sensor資料數量(包含廢資料)
    printf("iot. sensorCount = %d\n", sensorCount);
    if (sensorCount == 0)
    {
        printf("iot. no sensor device \n");
    }
    else
    {
        zone_status = calloc(sizeof(Zone_Device_Status_RD), sensorCount);
        sensorCount = ReadZoneStatus(zone_status);
        printf("iot. sensorCount = %d\n", sensorCount);
        free(zone_status);
    }

    return 0;
}
#endif

int ReadZoneStatusSize(void)
{
    FILE *fp;
    int readSize = 0;

    // printf("iot. Zone_Device_Status_RD size = %d \n", (int)sizeof(Zone_Device_Status_RD));
    // u_int8_t buffer[sizeof(Zone_Device_Status_RD)];

#ifdef VM
    fp = fopen("www/zone_status_RD.lf", "rb");
    if (fp == NULL)
    {
        printf("iot. open fail errno = %d reason = %s \n", errno, strerror(errno));
        return readSize; // EXIT_FAILURE
    }
#else
    // todo need check path
    fp = fopen("/var/www/zone_status_RD.lf", "rb");
    if (fp == NULL)
    {
        printf("iot. open fail errno = %d reason = %s \n", errno, strerror(errno));
        return readSize; // EXIT_FAILURE
    }
#endif

    fseek(fp, 0L, SEEK_END); // 尋找到 EOF
    int size = ftell(fp);    // 獲取當前位置
    rewind(fp);              // 回退到文件開頭
    fclose(fp);

    #ifdef DEBUGLPRINT
    printf("iot. size =  %d \n", size);
    #endif
    
    if (size < (int)sizeof(Zone_Device_Status_RD))
    {
        printf("iot. failed file size.\n");
        return readSize; // EXIT_FAILURE
    }
    else
    {
        return size / sizeof(Zone_Device_Status_RD);
    }
}

int ReadZoneStatus(Zone_Device_Status_RD *zone_status)
{
    FILE *fp;
    Zone_Device_Status_RD *p_zone_device_status;
    // Zone_Device_Status_RD *p_zone_status;
    int readSize = 0;
    u_int8_t buffer[sizeof(Zone_Device_Status_RD)];
    int count = 0; //, xx = 0;

#ifdef VM
    fp = fopen("www/zone_status_RD.lf", "rb");
    if (fp == NULL)
    {
        printf("iot. open fail errno = %d reason = %s \n", errno, strerror(errno));
        return FALSE; // EXIT_FAILURE
    }
#else
    // todo need check path
    fp = fopen("/var/www/zone_status_RD.lf", "rb");
    if (fp == NULL)
    {
        printf("iot. open fail errno = %d reason = %s \n", errno, strerror(errno));
        fp = fopen("/var/www/zone_status_RD.lf", "rb");
        if (fp == NULL)
        {
            printf("iot. second, open fail errno = %d reason = %s \n", errno, strerror(errno));
        }
        return readSize; // EXIT_FAILURE
    }
#endif

    do
    {
        readSize = fread(buffer, sizeof(buffer), 1, fp);

        if (readSize == 1)
        {
            // printf("iot. === read count%d ===\n", xx++);

            p_zone_device_status = (Zone_Device_Status_RD *)buffer;

            // #ifdef DEBUG
            //             PrintZoneStatus(p_zone_device_status);
            // #endif

            // # DateCode == 0,表示該段 36byte資料無效
            // int zeroFlag = 0;
            // int i;
            // printf("iot. p_zone_device_status->DateCode size = %d \n", sizeof(p_zone_device_status->DateCode));

            // for (i = 0; i < sizeof(p_zone_device_status->DateCode); i++)
            // {
            //     // printf("iot. ip_zone_device_status->DateCode[%d] = %.2x,  \n", i, p_zone_device_status->DateCode[i]);
            //     if (p_zone_device_status->DateCode[i] != 0)
            //     {
            //         zeroFlag = 1;
            //     }
            // }

            // if (zeroFlag == 0)
            // {
            //     // printf("iot. invalid data\n");
            // }
            // else
            // {
            zone_status[count] = *p_zone_device_status;
            count++;
            // }
        }
    } while (readSize == 1);
    sync();
    fclose(fp);

#ifdef DEBUG
    printf("iot. count = %d\n", count);
    int i;
    for (i = 0; i < count; i++)
    {
        printf("iot. === run %d === \n", i + 1);
        PrintZoneStatus(zone_status + i);
    }
#endif

    // int newlineNum = 16;
    // for(int i=0; i<sizeof(Zone_Device_Status_RD); i++)
    //    	if(i % newlineNum ==0){
    // 		printf("iot. \n%p\t %.2x\t ", &buffer[i+i],buffer[i]);
    // 	}else{
    // 		printf("iot. %.2x\t ", buffer[i]);
    // 	}
    return count;
}

void PrintZoneStatus(Zone_Device_Status_RD *p_zone_device_status)
{
    // char str[8], str1[8];
    // int j;
    char bat_val[36] = {0};
    float fbat = 0;

    // // # 記憶體位置
    // //  printf("iot. \n");
    // //  printf("iot. point = %p \n",              p_zone_device_merge);
    // //  printf("iot. zoneType point = %p \n",     &((*p_zone_device_merge).zoneType));
    // //  printf("iot. endpoint point = %p \n",     &((*p_zone_device_merge).endpoint));
    // //  printf("iot. shortAddr point = %p \n",    &((*p_zone_device_merge).shortAddr));
    // //  printf("iot. ieeeAddress point = %p \n",     &((*p_zone_device_merge).ieeeAddress));
    // //  printf("iot. alarmMode point = %p \n",     &((*p_zone_device_merge).alarmMode));
    // //  printf("iot. camEventTrig point = %p \n",    &((*p_zone_device_merge).camEventTrig));
    // //  printf("iot. zoneType point = 0x%p \n",     &((*p_zone_device_merge).zoneType));

    // sprintf(bat_val, "%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x", p_zone_device_status->DateCode[16],
    //         p_zone_device_status->DateCode[15], p_zone_device_status->DateCode[14], p_zone_device_status->DateCode[13],
    //         p_zone_device_status->DateCode[12], p_zone_device_status->DateCode[11], p_zone_device_status->DateCode[10],
    //         p_zone_device_status->DateCode[9], p_zone_device_status->DateCode[8], p_zone_device_status->DateCode[7],
    //         p_zone_device_status->DateCode[6], p_zone_device_status->DateCode[5], p_zone_device_status->DateCode[4],
    //         p_zone_device_status->DateCode[3], p_zone_device_status->DateCode[2], p_zone_device_status->DateCode[1],
    //         p_zone_device_status->DateCode[0]);
    // printf("iot. %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s", p_zone_device_status->DateCode[16],
    //        p_zone_device_status->DateCode[15], p_zone_device_status->DateCode[14], p_zone_device_status->DateCode[13],
    //        p_zone_device_status->DateCode[12], p_zone_device_status->DateCode[11], p_zone_device_status->DateCode[10],
    //        p_zone_device_status->DateCode[9], p_zone_device_status->DateCode[8], p_zone_device_status->DateCode[7],
    //        p_zone_device_status->DateCode[6], p_zone_device_status->DateCode[5], p_zone_device_status->DateCode[4],
    //        p_zone_device_status->DateCode[3], p_zone_device_status->DateCode[2], p_zone_device_status->DateCode[1],
    //        p_zone_device_status->DateCode[0]);
    printf("iot. Firmware Version: %s\n", p_zone_device_status->DateCode);
    // printf("iot. Firmware Version: %s\n", bat_val);

    memset(bat_val, 0, sizeof(bat_val));
    // sprintf(bat_val, "%d.%d", (((p_zone_device_status->value[1] & 0xE0) >> 2) | ((p_zone_device_status->value[1] & 0x0E) >> 1)) / 10,
    //         (((p_zone_device_status->value[1] & 0xE0) >> 2) | ((p_zone_device_status->value[1] & 0x0E) >> 1)) % 10);
    sprintf(bat_val, "%d.%d", (((p_zone_device_status->value[1] & 0xF0) >> 1) | (p_zone_device_status->value[1] & 0x0F)) / 10, (((p_zone_device_status->value[1] & 0xF0) >> 1) | (p_zone_device_status->value[1] & 0x0F)) % 10);

    fbat = atof(bat_val);
    printf("iot. Device Voltage: %f\n", fbat);

    printf("iot. RSSI: %x\n", p_zone_device_status->value[0]);

    // printf("iot. zoneType = 0x%.2x \n", (unsigned char)(p_zone_device_merge->zoneType));
    // printf("iot. endpoint = 0x%.2x \n", (unsigned char)(p_zone_device_merge->endpoint));
    // printf("iot. shortAddr = 0x%.4x \n", (unsigned short)(p_zone_device_merge->shortAddr));

    // j = sprintf(str, "%x", ((p_zone_device_merge->ieeeAddress[4]) + (p_zone_device_merge->ieeeAddress[5] << 8) + (p_zone_device_merge->ieeeAddress[6] << 16) + (p_zone_device_merge->ieeeAddress[7] << 24)));
    // sprintf(str + j, "%x", ((p_zone_device_merge->ieeeAddress[0]) + (p_zone_device_merge->ieeeAddress[1] << 8) + (p_zone_device_merge->ieeeAddress[2] << 16) + (p_zone_device_merge->ieeeAddress[3] << 24)));
    // printf("iot. ieeeAddress = 0x%s \n", str);

    // printf("iot. alarmMode = 0x%.2x \n", (unsigned char)(p_zone_device_merge->alarmMode));
    // printf("iot. camEventTrig = 0x%.2x \n", (unsigned char)(p_zone_device_merge->camEventTrig));
    // printf("iot. camEventTrig = 0x%.4x \n", (p_zone_device_merge->pad1[1] << 8) + p_zone_device_merge->pad1[0]);

    // printf("iot. devPlaceNum = 0x%.4x \n", (p_zone_device_merge->devPlaceNum[1] << 8) + p_zone_device_merge->devPlaceNum[0]);
    // printf("iot. devDescp = %s \n", p_zone_device_merge->devDescp);

    // memset(str, 0, sizeof(str));
    // j = sprintf(str1, "%x", ((p_zone_device_merge->bindList[1]) + (p_zone_device_merge->bindList[2] << 8) + (p_zone_device_merge->bindList[3] << 16) + (p_zone_device_merge->bindList[4] << 24)));
    // sprintf(str1 + j, "%x", (p_zone_device_merge->bindList[0]));
    // printf("iot. bindList = 0x%s \n", str);

    // printf("iot. param1 = 0x%.2x \n", (unsigned char)(p_zone_device_merge->param1));
    // printf("iot. isAlive = 0x%.2x \n", (unsigned char)(p_zone_device_merge->isAlive));
    // printf("iot. index = 0x%.2x \n", (unsigned char)(p_zone_device_merge->index));
    // printf("iot. unsed = 0x%.6x \n", (p_zone_device_merge->unsed[2] << 16) + (p_zone_device_merge->unsed[1] << 8) + p_zone_device_merge->unsed[0]);
}
