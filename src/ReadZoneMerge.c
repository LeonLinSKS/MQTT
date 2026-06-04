
// gcc -I ./include ReadZoneMerge.c -g -o ReadZoneMerge
// msdk-linux-gcc -I ./include ReadZoneMerge.c -g -o ReadZoneMerge
// valgrind --leak-check=full ./ReadZoneMerge

#include "common.h"
#include "ReadZoneMerge.h"

#ifdef VM
typedef unsigned char u_int8_t;
#endif

static int IsZeroIeeeAddress(const unsigned char *ieeeAddress)
{
	int i;

	for (i = 0; i < 8; i++)
	{
		if (ieeeAddress[i] != 0)
			return 0;
	}

	return 1;
}

static int IsSameIeeeAddress(const unsigned char *a, const unsigned char *b)
{
	int i;

	for (i = 0; i < 8; i++)
	{
		if (a[i] != b[i])
			return 0;
	}

	return 1;
}

static void PrintZoneMergeDecision(int record_index, const Zone_Device_Merge *record, const char *decision)
{
	printf("iot. ZoneMerge record #%d %s: type=%u endpoint=%u loop=%u zone=%u index=%u ieee=%02X %02X %02X %02X %02X %02X %02X %02X\n",
		   record_index,
		   decision,
		   record->type,
		   record->endpoint,
		   record->devPlaceNum[0],
		   record->devPlaceNum[1],
		   record->index,
		   record->ieeeAddress[0],
		   record->ieeeAddress[1],
		   record->ieeeAddress[2],
		   record->ieeeAddress[3],
		   record->ieeeAddress[4],
		   record->ieeeAddress[5],
		   record->ieeeAddress[6],
		   record->ieeeAddress[7]);
}


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

	printf("iot. ZoneMerge file size = %d bytes, record size = %u bytes\n", size, (unsigned)sizeof(Zone_Device_Merge));
	
	if (size < (int)sizeof(Zone_Device_Merge))
	{
		printf("iot. failed file size.\n");
		return readSize; // EXIT_FAILURE
	}
	else
	{
		int record_count = size / sizeof(Zone_Device_Merge);
		int remain_size = size % sizeof(Zone_Device_Merge);

		printf("iot. ZoneMerge file raw record count = %d, remain bytes = %d\n", record_count, remain_size);
		return record_count;
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
	int record_index = 0;
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
		// 一次讀一筆 Zone_Device_Merge
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
			if (IsZeroIeeeAddress(p_zone_device_merge->ieeeAddress))
			{
				PrintZoneMergeDecision(record_index, p_zone_device_merge, "drop zero-ieee");
			}
			else
			{
				// todo 因為目前出力裝置配對會出現五組資料,因此先將資料合併唯一,判斷方式為與前一筆ieeeaddress比對,一樣則放棄這一筆
				if (count > 0 && IsSameIeeeAddress(p_zone_device_merge->ieeeAddress, zone_merges[count - 1].ieeeAddress))
				{
					PrintZoneMergeDecision(record_index, p_zone_device_merge, "drop duplicate-ieee");
				}
				else
				{
					zone_merges[count] = *p_zone_device_merge;
					PrintZoneMergeDecision(record_index, p_zone_device_merge, "keep");
					count++;
				}
			}
			record_index++;
		}
	} while (readSize == 1);
	sync();
	fclose(fp);
	printf("iot. ZoneMerge accepted record count = %d\n", count);

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
