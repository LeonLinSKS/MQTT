
// gcc -I ./include ReadZoneMerge.c -g -o ReadZoneMerge
// msdk-linux-gcc -I ./include ReadZoneMerge.c -g -o ReadZoneMerge
// valgrind --leak-check=full ./ReadZoneMerge

#include "common.h"
#include "ReadZoneMerge.h"

#ifdef VM
typedef unsigned char u_int8_t;
#endif


#ifdef DEBUG
int main(void)
{
	int sensorCount;
	Zone_Device_Merge *zone_merges;

	printf("iot. Version: %s\n", "1.1.1");

	sensorCount = ReadZoneMergeSize(); // 讀取檔案中sensor資料(包含廢資料)
	printf("iot. sensorCount = %d\n", sensorCount);
	if (sensorCount == 0)
	{
		printf("iot. no sensor device \n");
	}
	else
	{
		zone_merges = calloc(sizeof(Zone_Device_Merge), sensorCount);
		sensorCount = ReadZoneMerge(zone_merges); // 取得真正有用資料數
		free(zone_merges);
	}

	return 0;
}
#endif

// read ZoneMerge file size
int ReadZoneMergeSize(void)
{
	FILE *fp;
	int readSize = 0;

#ifdef VM
	fp = fopen("www/zone_merge", "rb");
	if (fp == NULL)
	{
		printf("iot. open fail errno = %d reason = %s \n", errno, strerror(errno));
		return readSize; // EXIT_FAILURE
	}
#else
	// todo need check path
	fp = fopen("/var/www/zone_merge", "rb");
	if (fp == NULL)
	{
		printf("iot. open fail errno = %d reason = %s \n", errno, strerror(errno));
		return readSize; // EXIT_FAILURE
	}
#endif

	fseek(fp, 0L, SEEK_END); // 尋找到 EOF
	int size = ftell(fp);	 // 獲取當前位置
	rewind(fp);				 // 回退到文件開頭
	fclose(fp);

    #ifdef DEBUGLPRINT
	printf("iot. size =  %d \n", size);
    #endif
	
	if (size < 88)
	{
		printf("iot. failed file size.\n");
		return readSize; // EXIT_FAILURE
	}
	else
	{
		return size / 88;
	}
}

// output:
int ReadZoneMerge(Zone_Device_Merge *zone_merges)
{
	FILE *fp;
	Zone_Device_Merge *p_zone_device_merge;
	// Zone_Device_Merge *p_zone_merges;
	int readSize = 0;
	u_int8_t buffer[sizeof(Zone_Device_Merge)];
	int count = 0; 
	#ifdef DEBUGLPRINT
	int xx = 0;
	#endif


#ifdef VM
	fp = fopen("www/zone_merge", "rb");
	if (fp == NULL)
	{
		printf("iot. open fail errno = %d reason = %s \n", errno, strerror(errno));
		return FALSE; // EXIT_FAILURE
	}
#else
	// todo need check path
	fp = fopen("/var/www/zone_merge", "rb");
	if (fp == NULL)
	{
		printf("iot. open fail errno = %d reason = %s \n", errno, strerror(errno));
		return readSize; // EXIT_FAILURE
	}
#endif

	do
	{
		// 一次讀88byte
		readSize = fread(buffer, sizeof(buffer), 1, fp);

		if (readSize == 1)
		{
			#ifdef DEBUGLPRINT
			printf("iot. === read count%d ===\n", xx++);
			#endif
			p_zone_device_merge = (Zone_Device_Merge *)buffer;

#ifdef DEBUG
			PrintZoneMerge(p_zone_device_merge);
#endif

			// # ieeeAddress == 0表示該段 88byte資料無效
			if ((p_zone_device_merge->ieeeAddress[0] == 0) & (p_zone_device_merge->ieeeAddress[1] == 0) &
				(p_zone_device_merge->ieeeAddress[2] == 0) & (p_zone_device_merge->ieeeAddress[3] == 0) &
				(p_zone_device_merge->ieeeAddress[4] == 0) & (p_zone_device_merge->ieeeAddress[5] == 0) &
				(p_zone_device_merge->ieeeAddress[6] == 0) & (p_zone_device_merge->ieeeAddress[7] == 0))
			{
				#ifdef DEBUGLPRINT
				printf("iot. invalid data\n");
				#endif
			}
			else
			{
				// todo 因為目前出力裝置配對會出現五組資料,因此先將資料合併唯一,判斷方式為與前一筆ieeeaddress比對,一樣則放棄這一筆
				if ((p_zone_device_merge->ieeeAddress[0] == zone_merges[count - 1].ieeeAddress[0]) &
					(p_zone_device_merge->ieeeAddress[1] == zone_merges[count - 1].ieeeAddress[1]) &
					(p_zone_device_merge->ieeeAddress[2] == zone_merges[count - 1].ieeeAddress[2]) &
					(p_zone_device_merge->ieeeAddress[3] == zone_merges[count - 1].ieeeAddress[3]) &
					(p_zone_device_merge->ieeeAddress[4] == zone_merges[count - 1].ieeeAddress[4]) &
					(p_zone_device_merge->ieeeAddress[5] == zone_merges[count - 1].ieeeAddress[5]) &
					(p_zone_device_merge->ieeeAddress[6] == zone_merges[count - 1].ieeeAddress[6]) &
					(p_zone_device_merge->ieeeAddress[7] == zone_merges[count - 1].ieeeAddress[7]) )
				{
					#ifdef DEBUGLPRINT
					printf("iot. duplicate data\n");
					#endif
				}
				else
				{
					zone_merges[count] = *p_zone_device_merge;
					count++;
				}
			}
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
			PrintZoneMerge(zone_merges + i);
		}

		int newlineNum = 16;
		for (int i = 0; i < sizeof(Zone_Device_Merge); i++)
			if (i % newlineNum == 0)
			{
				printf("iot. \n%p\t %.2x\t ", &buffer[i + i], buffer[i]);
			}
			else
			{
				printf("iot. %.2x\t ", buffer[i]);
			}
		#endif
	return count;
}

void PrintZoneMerge(Zone_Device_Merge *p_zone_device_merge)
{
	char str[16], str1[10];
	int j;

	// # 記憶體位置
	//  printf("iot. \n");
	//  printf("iot. point = %p \n",              p_zone_device_merge);
	//  printf("iot. zoneType point = %p \n",     &((*p_zone_device_merge).zoneType));
	//  printf("iot. endpoint point = %p \n",     &((*p_zone_device_merge).endpoint));
	//  printf("iot. shortAddr point = %p \n",    &((*p_zone_device_merge).shortAddr));
	//  printf("iot. ieeeAddress point = %p \n",     &((*p_zone_device_merge).ieeeAddress));
	//  printf("iot. alarmMode point = %p \n",     &((*p_zone_device_merge).alarmMode));
	//  printf("iot. camEventTrig point = %p \n",    &((*p_zone_device_merge).camEventTrig));
	//  printf("iot. zoneType point = 0x%p \n",     &((*p_zone_device_merge).zoneType));

	memset(str, 0, sizeof(str));
	memset(str1, 0, sizeof(str1));

	printf("iot. type = 0x%.2x \n", (unsigned char)(p_zone_device_merge->type));
	printf("iot. endpoint = 0x%.2x \n", (unsigned char)(p_zone_device_merge->endpoint));
	printf("iot. shortAddr = 0x%.4x \n", (unsigned short)(p_zone_device_merge->shortAddr));

	j = sprintf(str, "%x", ((p_zone_device_merge->ieeeAddress[4]) + (p_zone_device_merge->ieeeAddress[5] << 8) + (p_zone_device_merge->ieeeAddress[6] << 16) + (p_zone_device_merge->ieeeAddress[7] << 24)));
	sprintf(str + j, "%x", ((p_zone_device_merge->ieeeAddress[0]) + (p_zone_device_merge->ieeeAddress[1] << 8) + (p_zone_device_merge->ieeeAddress[2] << 16) + (p_zone_device_merge->ieeeAddress[3] << 24)));
	printf("iot. ieeeAddress = 0x%s \n", str);

	printf("iot. alarmMode = 0x%.2x \n", (unsigned char)(p_zone_device_merge->alarmMode));
	printf("iot. camEventTrig = 0x%.2x \n", (unsigned char)(p_zone_device_merge->camEventTrig));
	printf("iot. camEventTrig = 0x%.4x \n", (p_zone_device_merge->pad1[1] << 8) + p_zone_device_merge->pad1[0]);

	printf("iot. devPlaceNum = 0x%.4x \n", (p_zone_device_merge->devPlaceNum[1] << 8) + p_zone_device_merge->devPlaceNum[0]);
	printf("iot. devDescp = %s \n", p_zone_device_merge->devDescp);

	memset(str, 0, sizeof(str));
	j = sprintf(str1, "%x", ((p_zone_device_merge->bindList[1]) + (p_zone_device_merge->bindList[2] << 8) + (p_zone_device_merge->bindList[3] << 16) + (p_zone_device_merge->bindList[4] << 24)));
	sprintf(str1 + j, "%x", (p_zone_device_merge->bindList[0]));
	printf("iot. bindList = 0x%s \n", str);

	printf("iot. param1 = 0x%.2x \n", (unsigned char)(p_zone_device_merge->param1));
	printf("iot. isAlive = 0x%.2x \n", (unsigned char)(p_zone_device_merge->isAlive));
	printf("iot. index = 0x%.2x \n", (unsigned char)(p_zone_device_merge->index));
	printf("iot. unsed = 0x%.6x \n", (p_zone_device_merge->unsed[2] << 16) + (p_zone_device_merge->unsed[1] << 8) + p_zone_device_merge->unsed[0]);
}
