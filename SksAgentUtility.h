#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
// #include <mqtt.h>
#include "IOTCAPIs.h" ///

#include <sys/socket.h>
#include <sys/file.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
// # =====cJSON========
#include <sys/types.h>
#include <sys/stat.h>
#include "cJSON.h"

#include <curl/curl.h>

#include "apmib.h"
#include <signal.h>
// #include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/statfs.h>
#include <dirent.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <sys/select.h>
#include <netdb.h>

// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <signal.h>
// #include <termios.h>
// #include <fcntl.h>
// #include <string.h>
// #include <time.h>
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <sys/statfs.h>
// #include <dirent.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>

// #include <sys/types.h>
// #include <sys/ipc.h>
// #include <sys/shm.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>

#define SHMSZ 17
key_t Key = 8888;
#ifdef DEBUG
#include <stdbool.h>
#endif

#ifndef DEBUG
#include "apmib.h"
#endif
#include <mtd/mtd-user.h>

// # release實際得改回false
#define DEBUG_PRINTF true

#define IOT_COMMAMD_AND_ITME false

#define THAI_ADDR_ENABLE false
// #define THAI_HB_API_ADDR "http://receiver101.skscloudsecurity.com/heartbeat"
// #define THAI_API_ADDR "http://receiver101.skscloudsecurity.com/signal"
#define THAI_PUSH_ADDR "http://push-receiver.skscloudsecurity.com/PushServiceSKT/PushService"
#define INTERFACE_NAME "eth1" //"enp0s3" //
#define MAX_SESSION 8
#define LICENSE_KEY "AQAAAHfKXuxVJXBuvlX1TivjsZ2q8LU4mIBB1oOxuoH9upOXCG5eph/EzFtOPTFeVhTsthJmjbY0zPvnJvzCAbZASrdwrouRP6d7WPUBZdvo4pCrFrjFV+hx503FAlM/idJpZ6vr3ZSqVst3tzXXDQoOwZYAuuelTj8PXYJIFqTT/131amDH114Bae/aTr51ykSdEtHqhpJtT+hqUTBcaSwuefQb"

#define WDATA "Socket_server"
#define FILE_PATH "/var/www/sys.log"
#define SKS_FILE_PATH "/var/www/sks.log"

#define DEFAULT_GW "2ewozeckh3"
#define DEFAULT_MNO "0592970"
#define DEFAULT_UID "F5KAAN1MKZVMSGPGYHRJ"
#define DEFAULT_APMIB false

#define REBOOT_STRING "echo 4 > /var/www/mode.ini"

#define HEARTBEATSTRING_EXEC "Heartbeat_8198c &"

#define SOCKET_SERVER_IP "127.0.0.1"

#define IOT_TXT_DATA "/var/www/iot.txt"
#define IOT_FILE_NAME "IOTCAPIs_Device2_8198c_h"
#define HEARTBEAT_FILE_NAME "Heartbeat_8198c"
#define HEARTBEAT_HBS_TXT_DATA "/var/www/hbs.txt"
#define HEARTBEAT_HBM_TXT_DATA "/var/www/hbm.txt"
#define SOCKETSERVER_FILE_NAME "Socket_server_8198c"
#define SOCKETSERVER_TXT_DATA "/var/www/ss.txt"
#define RUN_PATH "/var/www/run.txt"

#define MAX_IOTCAPI_MEM_SIZE 31262
#define MAX_SOCKERT_SEV_MEM_SIZE 14520

#define CHECK_RUN_TIME "15"
// #define CHECK_HBS_TIME "20"
// #define CHECK_HBM_TIME "36:50"
#define CHECK_SS_TIME 1
#define CHECK_IOT_TIME 8

#define HB_CHECK_SEC 60
#define HB_RESTART_SEC 720
#define HB_RESTART_DELAY_SEC 20 //+0~19
#define HB_RESTART_MOD_SEC 20

#define HEART_BEAT_SEC 120
#define HB_DELAY_SEC 40 //+0~19
#define HB_DELAY_MOD_SEC 20

#define MEM_CHECK_MIN 480

// #define SOCKET_SERVER_PORT 1234

#define BUFFER_CURL_JSON_WEBAPI_SIZE 100

#define WEBAPI_RETRY 2

#define HEARTBEAT_SEND_FAIL_COUNT 100

#define FLAG_SSL_BOOL true
#define ShowDataMassage false

#define Debug_WebCam_PlayBack_AppVoice true //

#define Debug_Tutk_Show56_Data false // app alarm zone set

#define Debug_Socket_server_webapi_String false

#define ShowIot_login_out_Data false

#define ShowIot_read_write_data false

#define ShowIotDetialData true //

#define Debug_file_data_show false

#define Debuf_51command_data_show true // remote alarm zone set //

#define Debug_log_file_save false

#define DATA_COUNT 300
#define SYS_LOG_COUNT 100
#define DATA_SIZE IOTC_MAX_PACKET_SIZE

#define FLASH_UISLOG 0x39000		  // Size 164K  0x39000 ~ 0x1A0210                                                 //0x62000
#define FLASH_GOLDENKEY_START 0x4A000 // 0x54000約40K 0x5E000 約80K
#define FLASH_DayRun_START 0x54000	  // 0x59000 約20K 1024約４筆;10240約40筆
#define FLASH_SKSLOG_START 0x59000	  // 0x5C000 約20K
// SKS_FILE_PATH
#define CONFIG_RTL_ROOT_IMAGE_OFFSET 0x262000
#define KEY_TEXT_FILE1 "/tmp/usb/sda1/abc.tar"
#define KEY_TEXT_FILE2 "/tmp/usb/sda1/cde.tar"
#define KEY_TEXT_FILE3 "/tmp/usb/sda1/efg.tar"

#define FLASH_INFO_TEXT_FILE1 "/tmp/usb/sda1/s.inf"
#define FLASH_INFO_TEXT_FILE2 "/tmp/usb/sda1/ss.inf"

#define HEAD_SHIFT 32
#define DayRun_HEAD_SHIFT 4096

#define SESSION_CHANNEL_IDLE_SECOND 30

#define MAX_DAY_NAME_ARRAY 7
#define DAY_NAME_SIZE 20
#define MIN_SWITCH 1
#define HOUR_SWITCH 2
#define DAY_SWITCH 3
#define MONTH_SWITCH 4

#define LISTENQ 20
#define PUSH_DATA_CANCEL_SEC 600

#define APP_LOGIN_LOGOUT_SAVE false

// #define FIRMWARE_VER   "001.00.040C"

#define DAYRUN_FILE "/var/www/DayRun"
#define PUSHDATA_FILE "/var/www/PushData"
#define DATE_TIME "202308300938 MAKE"
#define SKSLOG_FILE "/tmp/usb/sda1/log/sks.log"

#define HTTPS_DATA "uisgw/api"
#define PUSH_DATA "uiscoerd"

#define CHECK_USB_DISK_COMMAND "mount | grep usb > /var/www/mount.log"
#define USB_DISK_PATH "/var/tmp/usb/sda1"

#define NUM_VER "610"
#define SKS "C"
#define THAI "T"
#define CA_ENABLE false

#define MAX_CURL_FAIL 3
char FIRMWARE_VER2[20];
unsigned char Buffer_Data_Transfer[DATA_SIZE];
unsigned char Buffer_Data_Recive[DATA_SIZE];
unsigned char Buf_Data_Transfer_Webcam1[DATA_SIZE];
unsigned char Buf_Data_Transfer_Webcam2[DATA_SIZE];
unsigned char Buf_Data_Transfer_Webcam3[DATA_SIZE];
unsigned char Buf_Data_Transfer_Webcam4[DATA_SIZE];
unsigned char Buf_Data_Transfer_LogVedio[DATA_SIZE];

size_t Buffer_Transfer_Data_Size;
size_t Buffer_Recive_Data_Size;
size_t Buf_Data_Size_Webcam1;
size_t Buf_Data_Size_Webcam2;
size_t Buf_Data_Size_Webcam3;
size_t Buf_Data_Size_Webcam4;
size_t Buf_Data_Size_LogVedio;

bool Flag_Trans_LogVedio[MAX_SESSION];
bool Trans_LogVedio_mark[MAX_SESSION];
unsigned char Logvedio_UDP_port_status;
unsigned char Webcam_UDP_port_status;
bool Flag_WebCam1SID[MAX_SESSION], Flag_WebCam2SID[MAX_SESSION], Flag_WebCam3SID[MAX_SESSION], Flag_WebCam4SID[MAX_SESSION];
unsigned char SID_WebCam_Mask[MAX_SESSION];
// int WebCam1Qulity,WebCam2Qulity,WebCam3Qulity,WebCam4Qulity;
// bool Flag_IOT_SID[4];

int CountMaxSession;

// #define CONFIG_RTL_ROOT_IMAGE_OFFSET				0x262000  // root filesystem offset

// typedef	enum
// {
//         EVT_LOG_TYPE_ZIGBEE_EXIT = 1,
//         EVT_LOG_TYPE_ZIGBEE_HUNG,
//         EVT_LOG_TYPE_ZIGBEE_UART_HUNG,
//         EVT_LOG_TYPE_AGENT_EXIT,
//         EVT_LOG_TYPE_HB_HUNG
// } e_EVT_LOG_TYPE_daemon;

typedef struct
{
	int fd;
	unsigned int offset;
	unsigned int length;
} MTDEntry, *PMTDEntry;

typedef struct _targetChannel
{
	int nSID;
	int ChID;
	unsigned char Buffer_TUTK_data[DATA_SIZE];
	int nRead;
	int Flag_TUTK_read;
	char mno[30];
	char UID[30];
	char gw[30];
	int read_data_size;
	unsigned char read_data_array[DATA_SIZE];
	unsigned char IOT_response[DATA_SIZE];
} TargetChannel;

typedef struct _apmib
{
	char apmib_mno[30];
	char apmib_UID[30];
	char apmib_gw[30];
} Apmib;

typedef struct _webapiUSB
{
	char urlstr[256];
	char postfile[512];
	char outputstr[1024];
	char methodstr[32];
	char mno[32];
	bool ssl_bool;
	long response_code;
	char response_message[128];
	char inputstring[DATA_SIZE];

} WebApiUSBData;

typedef struct _webapiupload
{
	char urlstr[256];
	char postfile[512];
	char outputstr[1024];
	char methodstr[32];
	bool ssl_bool;
	long response_code;
	char response_message[128];
	char inputstring[DATA_SIZE];

} WebApiUploadData;

typedef struct _webapi
{
	char urlstr[256];
	char jsonstr[256];
	char outputstr[1024];
	char methodstr[32];
	char curlcommandstr[256];
	bool ssl_bool;
	char apmib_mno[30];
	char apmib_UID[30];
	char apmib_gw[30];
	long response_code;
	char response_message[128];
	char CurrIP[64];
	char CurrIP6[64];
	char inputstring[DATA_SIZE];
	// char buffer_string[BUFFER_CURL_JSON_WEBAPI_SIZE][DATA_SIZE];

	int COUNT_HTTP_OTHER_FAIL;
	// int http_non_curle_ok_count;
	// int push_non_curle_ok_count;
	// int buffer_string_begin;
	// int buffer_string_end;
	// int buffer_residual;

	int connfd;
	int sockfdd;

	int listenfd;
	int maxfd;
	int maxi;

	int nready;
	int client[FD_SETSIZE];

	// int port;
	char socket_input[DATA_SIZE];
	bool Webapi_run;
	// bool buffer_full;
	bool Curl_exec;
	bool webapi_send_fail;
	bool Push_data_run;
	// char socket_output[DATA_SIZE];
	int hb_delay_mod_sec;
	int sys_backup_count;
	bool flag_run_once;

} WebApiData;

typedef struct _pushapi
{
	char urlstr[128];
	char datastr[256];
	char pushkey[256];
	char outputstr[1024];
	bool ssl_bool;

	long response_code;
	char response_message[128];

	char methodstr[32];

	char inputstring[DATA_SIZE];
	int COUNT_PUSH_NON_OK;
	char GROUP_STRING[128];
	char GW_STRING[128];

} PushApiData;

typedef struct _socket
{
	int conn;
	int sockfd;
	int port;
	char socket_input[DATA_SIZE];
	char socket_output[DATA_SIZE];
	// bool flag_socket_server_read;
	// bool flag_socket_server_write;
	char apmib_mno[30];
	char apmib_UID[30];
	char apmib_gw[30];
	int hb_restart_sec;
	int upload_sec;
	bool flag_run_once;
	int sys_backup_count;
} SocketData;

// typedef struct
// {
// 	int id;
// 	char sno[20];
// 	char sigtime[25];
// 	char catg[10];
// 	char mno[30];
// 	char uid[30];
// 	char ip[20];
// 	char gw[30];
// 	char power[10];
// 	char lan[10];
// 	char scode[30];
// }jsonString_t;

int echo_file(char *data)
{
	char echo_buf[DATA_SIZE];
	printf("data to sks.log\n");
	sprintf(echo_buf, "echo \"%s\" >> /tmp/usb/sda1/log/sks.log &", data);
	system(echo_buf);
	sync();
	return 0;
}

int setMTDEntry(PMTDEntry ee, int offset, int length)
{
	if (offset < CONFIG_RTL_ROOT_IMAGE_OFFSET)
	{
		// printf("\n mtdblock0\n");
		ee[0].fd = open("/dev/mtdblock0", O_RDWR);
		if (ee[0].fd < 0)
			return -1;
		ee[0].offset = offset;
		if (offset + length > CONFIG_RTL_ROOT_IMAGE_OFFSET)
		{
			ee[0].length = CONFIG_RTL_ROOT_IMAGE_OFFSET - offset;
			ee[1].fd = open("/dev/mtdblock1", O_RDONLY);
			if (ee[1].fd < 0)
				return -1;
			ee[1].offset = 0;
			ee[1].length = offset + length - CONFIG_RTL_ROOT_IMAGE_OFFSET;
		}
		else
			ee[0].length = length;
	}
	else
	{
		// printf("\n mtdblock1\n");
		ee[1].fd = open("/dev/mtdblock1", O_RDWR);
		if (ee[1].fd < 0)
			return -1;
		ee[1].offset = offset;
		ee[1].length = length;
	}

	return 0;
}

int flash_write(int offset, int length, char *filename)
{
	int usrFd = -1, count, i, ret = -1;
	char buf[0x1000];
	MTDEntry eDev[2] = {{-1, 0, 0}, {-1, 0, 0}};

	do
	{
		if (setMTDEntry(eDev, offset, length) < 0)
			break;

		usrFd = open(filename, O_RDONLY, 0644);
		if (usrFd < 0)
			break;

		for (i = 0; i < 2; i++)
		{
			if (eDev[i].fd >= 0)
			{
				lseek(eDev[i].fd, eDev[i].offset, SEEK_SET);
				for (count = 0; count < eDev[i].length; count += sizeof(buf))
				{
					read(usrFd, buf, sizeof(buf));
					write(eDev[i].fd, buf, sizeof(buf));
				}
			}
		}
		ret = 0;
	} while (0);

	if (usrFd >= 0)
		close(usrFd);
	if (eDev[0].fd >= 0)
		close(eDev[0].fd);
	if (eDev[1].fd >= 0)
		close(eDev[1].fd);

	return ret;
}

int flash_read(int offset, int length, char *filename)
{

	// flash_read()
	int usrFd_flash_read = -1, count_flash_read, i_flash_read, ret_flash_read = -1;
	char buf_flash_read[0x1000];
	MTDEntry eDev_flash_read[2] = {{-1, 0, 0}, {-1, 0, 0}};
	// flash_read()

	do
	{
		if (setMTDEntry(eDev_flash_read, offset, length) < 0)
			break;

		usrFd_flash_read = open(filename, O_WRONLY | O_TRUNC | O_CREAT, 0644);
		if (usrFd_flash_read < 0)
			break;

		for (i_flash_read = 0; i_flash_read < 2; i_flash_read++)
		{

			if (eDev_flash_read[i_flash_read].fd >= 0)
			{
				lseek(eDev_flash_read[i_flash_read].fd, eDev_flash_read[i_flash_read].offset, SEEK_SET);
				for (count_flash_read = 0; count_flash_read < eDev_flash_read[i_flash_read].length; count_flash_read += sizeof(buf_flash_read))
				{
					read(eDev_flash_read[i_flash_read].fd, buf_flash_read, sizeof(buf_flash_read));
					write(usrFd_flash_read, buf_flash_read, sizeof(buf_flash_read));
				}
			}
		}
		ret_flash_read = 0;
	} while (0);

	if (usrFd_flash_read >= 0)
		close(usrFd_flash_read);
	if (eDev_flash_read[0].fd >= 0)
		close(eDev_flash_read[0].fd);
	if (eDev_flash_read[1].fd >= 0)
		close(eDev_flash_read[1].fd);

	return ret_flash_read;
}

void error_print(char *ptr)
{
	perror(ptr);
	// exit(EXIT_FAILURE);
}

// void quit_tranmission(int sig)
// {
//     printf("recv a quit signal = %d\n",sig);
//     exit(EXIT_SUCCESS);
// }

char *TimeFormat()
{ // 時間處理程式
	time_t timep_TimeFormat;
	struct tm *p_TimeFormat;
	static char timestring_TimeFormat[50];
	char *timestr_TimeFormat;

	time(&timep_TimeFormat);
	p_TimeFormat = localtime(&timep_TimeFormat); /*取得當地時間*/
	sprintf(timestring_TimeFormat, "%d/%02d/%02d %02d:%02d:%02d", (1900 + p_TimeFormat->tm_year), (1 + p_TimeFormat->tm_mon), p_TimeFormat->tm_mday, p_TimeFormat->tm_hour, p_TimeFormat->tm_min, p_TimeFormat->tm_sec);
	// printf(timestring);
	timestr_TimeFormat = timestring_TimeFormat;
	return timestr_TimeFormat;
}

char *HourFormat()
{ // 時間處理程式
	time_t timep_HourFormat;
	struct tm *p_HourFormat;
	static char timestring_HourFormat[50];
	char *timestr_HourFormat;
	time(&timep_HourFormat);
	p_HourFormat = localtime(&timep_HourFormat); /*取得當地時間*/
	sprintf(timestring_HourFormat, "%02d:%02d:%02d", p_HourFormat->tm_hour, p_HourFormat->tm_min, p_HourFormat->tm_sec);
	// printf(timestring);
	timestr_HourFormat = timestring_HourFormat;
	return timestr_HourFormat;
}

char *MinFormat()
{ // 時間處理程式
	time_t timep_MinFormat;
	struct tm *p_MinFormat;
	static char timestring_MinFormat[50];
	char *timestr_MinFormat;
	time(&timep_MinFormat);
	p_MinFormat = localtime(&timep_MinFormat); /*取得當地時間*/
	sprintf(timestring_MinFormat, "%02d:%02d", p_MinFormat->tm_min, p_MinFormat->tm_sec);
	// printf(timestring);
	timestr_MinFormat = timestring_MinFormat;
	return timestr_MinFormat;
}

char *SecFormat()
{ // 時間處理程式
	time_t timep_SecFormat;
	struct tm *p_SecFormat;
	static char timestring_SecFormat[50];
	char *timestr_SecFormat;
	time(&timep_SecFormat);
	p_SecFormat = localtime(&timep_SecFormat); /*取得當地時間*/
	sprintf(timestring_SecFormat, "%02d", p_SecFormat->tm_sec);
	// printf(timestring);
	timestr_SecFormat = timestring_SecFormat;
	return timestr_SecFormat;
}

char *TimeFormatDay_()
{ // 時間處理程式

	time_t timep;
	struct tm *p;
	static char timestring[50];
	char *timestr;

	time(&timep);
	p = localtime(&timep); /*取得當地時間*/
	sprintf(timestring, "%04d_%02d_%02d_%02d_%02d_%02d", (1900 + p->tm_year), (1 + p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
	timestr = timestring;
	return timestr;
}

char *TimeFormatDay()
{ // 時間處理程式

	time_t timep;
	struct tm *p;
	static char timestring[50];
	char *timestr;

	time(&timep);
	p = localtime(&timep); /*取得當地時間*/
	sprintf(timestring, "%04d%02d%02d%02d%02d", (1900 + p->tm_year), (1 + p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min);
	timestr = timestring;
	return timestr;
}

char *TimeFormatlog_rm()
{ // 時間處理程式

	time_t timep;
	struct tm *p;
	static char timestring[50];
	char *timestr;

	time(&timep);
	p = localtime(&timep); /*取得當地時間*/
	// sprintf(timestring,"%04d%02d%02d%02d%02d\n",(1900+p->tm_year),( 1+p-> tm_mon), p->tm_mday,p->tm_hour, p->tm_min);
	// sprintf(timestring,"%04d%02d%02d%02d",(1900+p->tm_year),( 1+p-> tm_mon), p->tm_mday,p->tm_hour-1);
	sprintf(timestring, "%04d", (1900 + p->tm_year - 2));
	timestr = timestring;
	return timestr;
}

int check_ps_log(char *filename, char *check_str, int count)
{
	FILE *fp;
	int rows = 0;
	char log[DATA_SIZE];
	int i;
	const char *d = "  ";
	char *p;
	char CHECK_PID_SRT[10];
	int CHECK_MEM_SIZE = 0;
	int CHECK_HB = 0;
	char buf[512];

	memset(log, 0, DATA_SIZE);
	if (DEBUG_PRINTF)
		printf("filename=%s\n", filename);
	if (NULL == (fp = fopen(filename, "r")))
	{

		if (DEBUG_PRINTF)
			printf("no file,Create New file %s\n", filename);
		fp = fopen(filename, "w");
	}

	if (fp != NULL)
	{
		rows = 0;
		while (fgets(log, sizeof(log), fp) != NULL)
		{
			if (strncmp(HEARTBEAT_FILE_NAME, check_str, 14) != 0)
			{

				if ((strstr(log, check_str)))
				{
					if (DEBUG_PRINTF)
						printf("%d)%s", rows, log);
					i = 1;
					p = strtok(log, d);

					while (p != NULL)
					{
						// printf("%d)%s\n", i, p);
						if (i == 1)
						{
							strcpy(CHECK_PID_SRT, p);
							if (DEBUG_PRINTF)
								printf("%s(PID=%s,", check_str, p);
						}
						if (i == 3)
						{
							// strcpy(IOTCAPI_PID_SRT,p);
							CHECK_MEM_SIZE = atoi(p);
							if (DEBUG_PRINTF)
								printf("MEM SIZE=%d)\n", CHECK_MEM_SIZE);
						}
						p = strtok(NULL, d);
						i++;
					}
					break;
				}
				rows++;
			}
			else
			{
				if ((strstr(log, check_str)))
				{
					CHECK_HB = 1;

					break;
				}
			}
		}
	}
	fclose(fp); ///////////

	if (strncmp(HEARTBEAT_FILE_NAME, check_str, 14) != 0)
	{
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "%s - aw.%s mem size=>(%d)! \n", TimeFormat(), check_str, CHECK_MEM_SIZE);
		if (DEBUG_PRINTF)
			printf("%s", buf);
		if (count >= MEM_CHECK_MIN)
			echo_file(buf);

		return CHECK_MEM_SIZE; //!=HEARTBEAT_FILE_NAME
	}
	else
	{
		sprintf(buf, "%s - aw.chech %s mem have no data!! \n", TimeFormat(), check_str);
		if (DEBUG_PRINTF)
			printf("%s", buf);
		if (count >= MEM_CHECK_MIN)
			echo_file(buf);
		return CHECK_HB;
	}
}

int WriteToFileSys(char *filename, char *NewEvent)
{
	FILE *fp_WriteToFileSys;
	int Rows_WriteToFileSys = 0;
	char webapibuf_WriteToFileSys[SYS_LOG_COUNT][128];
	int i_WriteToFileSys;
	for (i_WriteToFileSys = 0; i_WriteToFileSys < SYS_LOG_COUNT; i_WriteToFileSys++)
	{
		memset(webapibuf_WriteToFileSys[i_WriteToFileSys], 0, 128);
	}
	if (Debug_Socket_server_webapi_String)
		printf("filename=%s\n", filename);
	if (NULL == (fp_WriteToFileSys = fopen(filename, "r")))
	{
		if (DEBUG_PRINTF)
			printf("no file,Create New file %s\n", filename);
		fp_WriteToFileSys = fopen(filename, "w");
	}

	if (fp_WriteToFileSys != NULL)
	{
		Rows_WriteToFileSys = 0;
		while (fgets(webapibuf_WriteToFileSys[Rows_WriteToFileSys], sizeof(webapibuf_WriteToFileSys[Rows_WriteToFileSys]), fp_WriteToFileSys) != NULL)
		{

			// printf("%d)%s",Rows,webapibuf[Rows]); //#讀出每一行
			Rows_WriteToFileSys++;
			if (Rows_WriteToFileSys == SYS_LOG_COUNT)
			{
				break;
			}
		}
		if (Debug_Socket_server_webapi_String)
			printf("\nTotal Rows=%d \n", Rows_WriteToFileSys); // # 總共有幾筆
	}

	fclose(fp_WriteToFileSys); ///////////
	usleep(100);

	fp_WriteToFileSys = fopen(filename, "w+"); // # 把檔案重寫
	if (DEBUG_PRINTF)
		printf("NewEvent=%s\n\n", NewEvent);
	fwrite(NewEvent, strlen(NewEvent), 1, fp_WriteToFileSys);

	if (Rows_WriteToFileSys >= SYS_LOG_COUNT)
	{
		for (i_WriteToFileSys = 0; i_WriteToFileSys < (Rows_WriteToFileSys - 1); i_WriteToFileSys++)
		{
			// printf("%d)%s))\n",i+1,webapibuf[Rows]);
			fwrite(webapibuf_WriteToFileSys[i_WriteToFileSys], strlen(webapibuf_WriteToFileSys[i_WriteToFileSys]), 1, fp_WriteToFileSys);
		}
		if (Debug_Socket_server_webapi_String)
			printf("%d)%s-the last list\n", i_WriteToFileSys, webapibuf_WriteToFileSys[i_WriteToFileSys - 1]);
	}
	else
	{
		if (Debug_Socket_server_webapi_String)
			printf("< data count\n");
		for (i_WriteToFileSys = 0; i_WriteToFileSys < (Rows_WriteToFileSys); i_WriteToFileSys++)
		{
			// printf("%d)%s))\n",i+1,webapibuf[Rows]);
			fwrite(webapibuf_WriteToFileSys[i_WriteToFileSys], strlen(webapibuf_WriteToFileSys[i_WriteToFileSys]), 1, fp_WriteToFileSys);
		}
		if (Debug_Socket_server_webapi_String)
			printf("%d)%s-the last list\n", i_WriteToFileSys, webapibuf_WriteToFileSys[i_WriteToFileSys - 1]);
		Rows_WriteToFileSys++;
	}
	if (DEBUG_PRINTF)
		printf("WriteToFileSys The NEW total Rows=%d \n", Rows_WriteToFileSys); // # 總共有幾筆
	usleep(100);
	fclose(fp_WriteToFileSys);
	sync();
	if (Debug_Socket_server_webapi_String)
		printf("sys.log file write over\n\n");
	return 0;
}
