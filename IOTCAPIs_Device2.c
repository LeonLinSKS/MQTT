// msdk-linux-gcc -I /home/kk/8198c_toolchain/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/include -g -Wall -O2 IOTCAPIs_Device2.c libapmib.so cJSON.c -lIOTCAPIs -lNebulaAPIs -lTUTKGlobalAPIs -lm -lcurl -lpthread -L /home/kk/8198c_toolchain/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/lib -Wl,-rpath=/home/kk/8198c_toolchain/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/lib -lpthread -o IOTCAPIs_Device2_8198c_h
#include "SksAgentUtility.h"
SocketData socketDatainfo;
bool Flag_SID_Login_Run_IOT[4];
/*处理系统调用中产生的错误*/
void error_print(char *ptr);
char *TimeFormat();

int echo_file(char *data);
int check_ps_log(char *filename, char *check_str, int count);

int WriteToFileSys(char *filename, char *NewEvent);
unsigned char Flag_restart_HB = 0;
unsigned char Flag_upload_log = 0;
unsigned char Flag_del_log = 0;
unsigned char Flag_upload_config = 0;
unsigned char Flag_download_config = 0;

char *hexToAscii(char hex[]);
int valueOf(char symbol);
char write_sys_log_buf2[40];

int check_command_response_string(char *command, char *check)
{
	// char command_str[64];
	char buffer[1024];
	char *str;
	// int i=0;
	// bool flag_found_string=false;
	printf("check usb disk command=>%s!\n", command);

	system(command);
	printf("check string=>%s\n", check);

	FILE *fp = fopen("/var/www/mount.log", "r");

	if (fp == NULL)
	{
		printf("1DATA=>***(\n");
		// system("cat /var/www/mount.log");
		// printf(")***\n");
		// perror("ponen() failed");
		return 1;
	}
	else
	{
		// printf("found usb disk");
		printf("2DATA=>***\n");
		// system("cat /var/www/mount.log");
		// printf(")***\n");
		fgets(buffer, sizeof(buffer), fp);
		str = strstr(buffer, check);
		if (str)
		{
			printf("found %s)\n", str);
			return 0;
		}
		else
		{
			printf("non found %s)\n", str);
			return 2;
		}
	}
	// while (fgets(buffer,sizeof(buffer),fp)!=NULL){
	// 	i++;
	// 	if (strstr(buffer,check)){
	// 		printf("%d)%s\n",i,buffer);
	// 		printf("%d)%s\n",i,strstr(buffer,check));
	// 		flag_found_string=true;
	// 		break;
	// 	}

	//     // printf("%d)%s",i,strstr(buffer,check));
	//     // // strcat(out,buffer);
	// }
	// fclose(fp);
	// if (flag_found_string){
	// 	return 0;
	// }
	// else{
	// 	return 1;
	// }
}

void *ServerSocket_thread_webcam1(void *arg) // socket udp
{
	int i;
	char temp_str[30];
	bool flag_first_run = true;
	strcpy(temp_str, TimeFormat());
	char write_sys_log_buf[128];
	while (true)
	{
		SocketData *socketDatainfo = (SocketData *)arg;
		// int sockfd = socket(AF_INET, SOCK_STREAM, 0);/*IPV4流式协议即TCP协议*/
		socketDatainfo->sockfd = socket(AF_INET, SOCK_DGRAM, 0); /*IPV4流式协议即UDP协议*/
		if (DEBUG_PRINTF)
			printf("\n=====thread_ServerSocket_webcam 1 & log vedio port:%d=======\n", socketDatainfo->port);
		if (socketDatainfo->sockfd < 0)
			error_print("socket webcam1 & log vedio\n");
		struct sockaddr_in servaddr;
		socklen_t cliaddr_len = sizeof(servaddr);
		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET; /*IPV4*/

		// servaddr.sin_port = socketDatainfo->port;
		servaddr.sin_port = htons(socketDatainfo->port);
		servaddr.sin_addr.s_addr = inet_addr(SOCKET_SERVER_IP); /*使用本地环回地址做测试*/

		int on = 1;
		if (setsockopt(socketDatainfo->sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
			error_print("setsockopt webcam1 & logvideo");
		/*绑定本地Socket地址*/
		if (DEBUG_PRINTF)
			printf("2server loop  webcam1 port:%d!!\n", socketDatainfo->port);
		if (bind(socketDatainfo->sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
			error_print("bind webcam1 & logvideo");
		if (DEBUG_PRINTF)
			printf("*****5.server loop  webcam1 & logvideo port:%d!!\n", socketDatainfo->port);
		while (true)
		{
			if (Webcam_UDP_port_status & 0x01)
			{

				if (flag_first_run)
				{
					if (DEBUG_PRINTF)
						printf("\n\nwebcam1 Webcam_UDP_port_status=%x)\n\n", Webcam_UDP_port_status);
					flag_first_run = false;
				}

				int ret;
				bzero(Buf_Data_Transfer_Webcam1, strlen((const char *)Buf_Data_Transfer_Webcam1));
				// // printf("\n*****************socket server read \n\n");
				ret = recvfrom(socketDatainfo->sockfd, Buf_Data_Transfer_Webcam1, IOTC_MAX_PACKET_SIZE, 0, (struct sockaddr *)&servaddr, &cliaddr_len);
				/*读取conn连接发送过来的数据*/
				// int ret = read(socketDatainfo->conn, Buf_Data_Transfer_Webcam1, strlen((const char *)Buf_Data_Transfer_Webcam1));
				if (ret < 0)
				{
					if (DEBUG_PRINTF)
						printf("read : Connection reset by peer，Webcam1 socket read ret=%d!\n", ret);
					break;
				}
				else if (ret == 0)
				{
					if (DEBUG_PRINTF)
						printf("Ctrl+C，Webcam1 socket read ret=%d!\n", ret);
					break;

					// break;//父进程收到服务器端退出的信息（服务器Ctrl+C结束通信进程，read函数返回值为0，退出循环）
				}
				else
				{

					Buf_Data_Size_Webcam1 = ret;
					// printf("WebCam1 :");
					for (i = 0; i < MAX_SESSION; i++)
					{
						if (Flag_WebCam1SID[i])
						{
							ret = IOTC_Session_Write(i, (const char *)Buf_Data_Transfer_Webcam1, Buf_Data_Size_Webcam1, 1);

							if (ret == IOTC_ER_CH_NOT_ON)
							{
								if (DEBUG_PRINTF)
									printf("====IOT webcam1 IOTC_ER_CH_NOT_ON!!!!!!!!!!\n");
								IOTC_Session_Channel_ON(i, 1);
							}
							else if (ret < 0)
							{ // other error < 0 means this session cant use anymore
								if (DEBUG_PRINTF)
									printf("====IOT webcam1 IOTC_Session_Write error, code[%d]\n", ret);
								// IOTC_Session_Close(i);

								sprintf(write_sys_log_buf, "%s - aw.SWebcam1_logout_SID=%d_CID=%d(%d)! \n", TimeFormat(), i, 1, ret);
								echo_file(write_sys_log_buf);

								// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf);  //SKS_FILE_PATH

								// sync();
								// system("cp /var/www/sks.log /var/www/sks.logtmp");
								// if(flash_write(FLASH_SKSLOG_START, 4096*2, "/var/www/sks.logtmp")){
								// 	if (DEBUG_PRINTF)
								// 		printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
								// }
								// else{
								// 	if (DEBUG_PRINTF)
								// 		printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
								// }
								// system("rm /var/www/sks.logtmp");
								// sync();

								if (DEBUG_PRINTF)
									printf("%s\n\n", write_sys_log_buf);
								IOTC_Session_Channel_ON(i, 1);
								socketDatainfo->flag_run_once = true;
								Webcam_UDP_port_status = 0;
								pthread_exit(0);
							}
							else
							{
								IOTC_Session_Channel_ON(i, 1); /// 20211221
															   // printf("====IOT webcam1 TUTK write over!!\n\n");
							}
						}
						else
						{
						}
					}
					if (strcmp(temp_str, TimeFormat()) != 0)
					{
						if (DEBUG_PRINTF)
							printf("\nthread_webcam1 status=%02x,ret=%d))\n", Webcam_UDP_port_status, ret);
						strcpy(temp_str, TimeFormat());
						// printf("IOT_webcam1(%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x)(ret=%d)(%s)\n",Buf_Data_Transfer_Webcam1[0],Buf_Data_Transfer_Webcam1[1],Buf_Data_Transfer_Webcam1[2],Buf_Data_Transfer_Webcam1[3],Buf_Data_Transfer_Webcam1[4],Buf_Data_Transfer_Webcam1[5],Buf_Data_Transfer_Webcam1[6],Buf_Data_Transfer_Webcam1[7],Buf_Data_Transfer_Webcam1[8],Buf_Data_Transfer_Webcam1[9],Buf_Data_Transfer_Webcam1[10],Buf_Data_Transfer_Webcam1[11],Buf_Data_Transfer_Webcam1[12],Buf_Data_Transfer_Webcam1[13],Buf_Data_Transfer_Webcam1[14],Buf_Data_Transfer_Webcam1[15],Buf_Data_Transfer_Webcam1[16],Buf_Data_Transfer_Webcam1[17],Buf_Data_Transfer_Webcam1[18],Buf_Data_Transfer_Webcam1[19],ret,TimeFormat());
						for (i = 0; i < MAX_SESSION; i++)
						{
							if (Flag_WebCam1SID[i])
							{
								if (DEBUG_PRINTF)
									printf("*******(SID=%d,on)\n", i);
							}
							else
							{
								// printf("(SID=%d,off)******\n",i);
							}
						}
					}
					// strcpy(temp_str,TimeFormat());
				}
			}
			else if (Logvedio_UDP_port_status & 0x01)
			{
				int ret;
				bzero(Buf_Data_Transfer_LogVedio, IOTC_MAX_PACKET_SIZE);
				// // printf("\n*****************socket server read \n\n");
				ret = recvfrom(socketDatainfo->sockfd, Buf_Data_Transfer_LogVedio, IOTC_MAX_PACKET_SIZE, 0, (struct sockaddr *)&servaddr, &cliaddr_len);
				/*读取conn连接发送过来的数据*/
				// int ret = read(socketDatainfo->conn, Buf_Data_Transfer_LogVedio, strlen((const char *)Buf_Data_Transfer_LogVedio));
				if (ret < 0)
				{
					if (DEBUG_PRINTF)
						printf("read : Connection reset by peer，LogVedio socket read ret=%d!\n", ret);
					break;
				}
				else if (ret == 0)
				{
					if (DEBUG_PRINTF)
						printf("Ctrl+C，LogVedio socket read ret=%d!\n", ret);
					break;
					// break;//父进程收到服务器端退出的信息（服务器Ctrl+C结束通信进程，read函数返回值为0，退出循环）
				}
				else
				{
					Buf_Data_Size_LogVedio = ret;
					for (i = 0; i < MAX_SESSION; i++)
					{
						if (Flag_Trans_LogVedio[i])
						{
							ret = IOTC_Session_Write(i, (const char *)Buf_Data_Transfer_LogVedio, Buf_Data_Size_LogVedio, 1);
							if (ret == IOTC_ER_CH_NOT_ON)
							{
								if (DEBUG_PRINTF)
									printf("====IOT webcam1 IOTC_ER_CH_NOT_ON!!!!!!!!!!\n");
								IOTC_Session_Channel_ON(i, 1);
							}
							else if (ret < 0)
							{ // other error < 0 means this session cant use anymore
								if (DEBUG_PRINTF)
									printf("====IOT LogVedioSID IOTC_Session_Write error, code[%d]\n", ret);

								sprintf(write_sys_log_buf, "%s - aw.SLogVedio_logout_SID=%d_CID=%d(%d)! \n", TimeFormat(), i, 1, ret);
								if (DEBUG_PRINTF)
									printf("%s\n\n", write_sys_log_buf);
								echo_file(write_sys_log_buf);

								// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf);  //SKS_FILE_PATH
								// sync();
								// system("cp /var/www/sks.log /var/www/sks.logtmp");
								// if(flash_write(FLASH_SKSLOG_START, 4096*2, "/var/www/sks.logtmp")){
								// 	if (DEBUG_PRINTF)
								// 		printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
								// }
								// else{
								// 	if (DEBUG_PRINTF)
								// 		printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
								// }
								// system("rm /var/www/sks.logtmp");
								// sync();

								IOTC_Session_Channel_ON(i, 1);
								socketDatainfo->flag_run_once = true;
								Logvedio_UDP_port_status = 0;
								pthread_exit(0);
							}
							else
							{
								// printf("====IOT webcam1 TUTK write over!!\n\n");
								IOTC_Session_Channel_ON(i, 1); // 20211221
							}
							break;
						}
					}
					if (strcmp(temp_str, TimeFormat()) != 0)
					{
						strcpy(temp_str, TimeFormat());
						if (DEBUG_PRINTF)
							printf("IOT_LogVedio(%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x)(ret=%d)(port:%d)(%s)\n", Buf_Data_Transfer_LogVedio[0], Buf_Data_Transfer_LogVedio[1], Buf_Data_Transfer_LogVedio[2], Buf_Data_Transfer_LogVedio[3], Buf_Data_Transfer_LogVedio[4], Buf_Data_Transfer_LogVedio[5], Buf_Data_Transfer_LogVedio[6], Buf_Data_Transfer_LogVedio[7], Buf_Data_Transfer_LogVedio[8], Buf_Data_Transfer_LogVedio[9], Buf_Data_Transfer_LogVedio[10], Buf_Data_Transfer_LogVedio[11], Buf_Data_Transfer_LogVedio[12], Buf_Data_Transfer_LogVedio[13], Buf_Data_Transfer_LogVedio[14], Buf_Data_Transfer_LogVedio[15], Buf_Data_Transfer_LogVedio[16], Buf_Data_Transfer_LogVedio[17], Buf_Data_Transfer_LogVedio[18], Buf_Data_Transfer_LogVedio[19], ret, socketDatainfo->port, TimeFormat());
						for (i = 0; i < MAX_SESSION; i++)
						{
							if (Flag_Trans_LogVedio[i])
							{
								if (DEBUG_PRINTF)
									printf("*******(SID=%d,on)\n", i);
							}
							else
							{
								// printf("(SID=%d,off)******\n",i);
							}
						}
					}
				}
			}
			else
			{
				usleep(10000);
			}
		}
		close(socketDatainfo->conn);
		close(socketDatainfo->sockfd);
		if (DEBUG_PRINTF)
			printf("wepcam1, logvedio port:%d\n", socketDatainfo->port);
	}
	pthread_exit(0);
}
void *ServerSocket_thread_webcam2(void *arg)
{
	char temp_str[30];
	strcpy(temp_str, TimeFormat());
	int i;
	bool flag_first_run = true;
	char write_sys_log_buf[128];
	while (true)
	{
		SocketData *socketDatainfo = (SocketData *)arg;
		socketDatainfo->sockfd = socket(AF_INET, SOCK_DGRAM, 0); /*IPV4流式协议即UDP协议*/
		if (DEBUG_PRINTF)
			printf("\n=====thread_ServerSocket_webcam2 port:%d=======\n", socketDatainfo->port);
		if (socketDatainfo->sockfd < 0)
			error_print("socket webcam2\n");
		struct sockaddr_in servaddr;
		socklen_t cliaddr_len = sizeof(servaddr);
		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET; /*IPV4*/

		// servaddr.sin_port = socketDatainfo->port;
		servaddr.sin_port = htons(socketDatainfo->port);
		servaddr.sin_addr.s_addr = inet_addr(SOCKET_SERVER_IP); /*使用本地环回地址做测试*/
		if (DEBUG_PRINTF)
			printf("1server loop  webcam2 port:%d!!\n", socketDatainfo->port);
		int on = 1;
		if (setsockopt(socketDatainfo->sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
			error_print("setsockopt webcam2");
		/*绑定本地Socket地址*/
		if (DEBUG_PRINTF)
			printf("2server loop  webcam2 port:%d!!\n", socketDatainfo->port);
		if (bind(socketDatainfo->sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
			error_print("bind webcam2");
		if (DEBUG_PRINTF)
			printf("*****5server loop  webcam2 port:%d!!\n", socketDatainfo->port);

		while (true)
		{
			if (Webcam_UDP_port_status & 0x02)
			{

				if (flag_first_run)
				{
					if (DEBUG_PRINTF)
						printf("\n\nwebcam2 Webcam_UDP_port_status=%x)\n", Webcam_UDP_port_status);
					flag_first_run = false;
				}

				int ret;
				bzero(Buf_Data_Transfer_Webcam2, strlen((const char *)Buf_Data_Transfer_Webcam2));

				ret = recvfrom(socketDatainfo->sockfd, Buf_Data_Transfer_Webcam2, IOTC_MAX_PACKET_SIZE, 0, (struct sockaddr *)&servaddr, &cliaddr_len);

				if (ret < 0)
				{
					if (DEBUG_PRINTF)
						printf("read : Connection reset by peer，Webcam2 socket read ret=%d!\n", ret);
					break;
				}
				else if (ret == 0)
				{
					printf("Ctrl+C，Webcam2 socket read ret=%d!\n", ret);
					break;

					// break;//父进程收到服务器端退出的信息（服务器Ctrl+C结束通信进程，read函数返回值为0，退出循环）
				}
				else
				{
					// printf("****webcam2 socket read_info right\n");
					Buf_Data_Size_Webcam2 = ret;

					for (i = 0; i < MAX_SESSION; i++)
					{
						// printf("%d\n",i);
						if (Flag_WebCam2SID[i])
						{
							ret = IOTC_Session_Write(i, (const char *)Buf_Data_Transfer_Webcam2, Buf_Data_Size_Webcam2, 1);

							// pritnf("*******(SID=%d,on)\n",i);
							if (ret == IOTC_ER_CH_NOT_ON)
							{
								if (DEBUG_PRINTF)
									printf("====IOT webcam2 IOTC_ER_CH_NOT_ON!!!!!!!!!!\n");
								IOTC_Session_Channel_ON(i, 1);
							}
							else if (ret < 0)
							{ // other error < 0 means this session cant use anymore
								if (DEBUG_PRINTF)
									printf("====IOT webcam2 IOTC_Session_Write error, code[%d]\n", ret);

								// IOTC_Session_Close(i);
								sprintf(write_sys_log_buf, "%s - aw.SWebcam2_logout_SID=%d_CID=%d(%d)! \n", TimeFormat(), i, 1, ret);
								if (DEBUG_PRINTF)
									printf("%s\n\n", write_sys_log_buf);

								echo_file(write_sys_log_buf);

								// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf);  //SKS_FILE_PATH

								// sync();
								// system("cp /var/www/sks.log /var/www/sks.logtmp");
								// if(flash_write(FLASH_SKSLOG_START, 4096*2, "/var/www/sks.logtmp")){
								// 	if (DEBUG_PRINTF)
								// 		printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
								// }
								// else{
								// 	if (DEBUG_PRINTF)
								// 		printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
								// }
								// system("rm /var/www/sks.logtmp");
								// sync();

								IOTC_Session_Channel_ON(i, 1);
								socketDatainfo->flag_run_once = true;
								Webcam_UDP_port_status = 0;
								pthread_exit(0);
							}
							else
							{
								IOTC_Session_Channel_ON(i, 1); // 20211221
															   // printf("write (%d) ok",ret);
							}
						}
						else
						{
							// pritnf("(SID=%d,off)*******\n",i);
						}
					}

					if (strcmp(temp_str, TimeFormat()) != 0)
					{
						strcpy(temp_str, TimeFormat());
						if (DEBUG_PRINTF)
							printf("thread_webcam2 status=%02x,ret=%d))\n", Webcam_UDP_port_status, ret);

						for (i = 0; i < MAX_SESSION; i++)
						{
							if (Flag_WebCam2SID[i])
							{
								if (DEBUG_PRINTF)
									printf("*******(SID=%d,on)\n", i);
							}
							else
							{
								// printf("(SID=%d,off)******\n",i);
							}
						}
					}
				}
			}
			else if (Logvedio_UDP_port_status & 0x02)
			{
				int ret;
				bzero(Buf_Data_Transfer_LogVedio, IOTC_MAX_PACKET_SIZE);
				// // printf("\n*****************socket server read \n\n");
				ret = recvfrom(socketDatainfo->sockfd, Buf_Data_Transfer_LogVedio, IOTC_MAX_PACKET_SIZE, 0, (struct sockaddr *)&servaddr, &cliaddr_len);
				/*读取conn连接发送过来的数据*/
				// int ret = read(socketDatainfo->conn, Buf_Data_Transfer_LogVedio, strlen((const char *)Buf_Data_Transfer_LogVedio));
				if (ret < 0)
				{
					if (DEBUG_PRINTF)
						printf("read : Connection reset by peer，LogVedio socket read ret=%d!\n", ret);
					break;
				}
				else if (ret == 0)
				{
					if (DEBUG_PRINTF)
						printf("Ctrl+C，LogVedio socket read ret=%d!\n", ret);
					break;
					// break;//父进程收到服务器端退出的信息（服务器Ctrl+C结束通信进程，read函数返回值为0，退出循环）
				}
				else
				{
					Buf_Data_Size_LogVedio = ret;
					for (i = 0; i < MAX_SESSION; i++)
					{
						if (Flag_Trans_LogVedio[i])
						{
							ret = IOTC_Session_Write(i, (const char *)Buf_Data_Transfer_LogVedio, Buf_Data_Size_LogVedio, 1);
							if (ret == IOTC_ER_CH_NOT_ON)
							{
								if (DEBUG_PRINTF)
									printf("====IOT webcam1 IOTC_ER_CH_NOT_ON!!!!!!!!!!\n");
								IOTC_Session_Channel_ON(i, 1);
							}
							else if (ret < 0)
							{ // other error < 0 means this session cant use anymore
								if (DEBUG_PRINTF)
									printf("====IOT LogVedioSID IOTC_Session_Write error, code[%d]\n", ret);

								sprintf(write_sys_log_buf, "%s - aw.SLogVedio_logout_SID=%d_CID=%d(%d)! \n", TimeFormat(), i, 1, ret);
								if (DEBUG_PRINTF)
									printf("%s\n\n", write_sys_log_buf);

								echo_file(write_sys_log_buf);
								// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf);  //SKS_FILE_PATH
								// sync();
								// system("cp /var/www/sks.log /var/www/sks.logtmp");
								// if(flash_write(FLASH_SKSLOG_START, 4096*2, "/var/www/sks.logtmp")){
								// 	if (DEBUG_PRINTF)
								// 		printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
								// }
								// else{
								// 	if (DEBUG_PRINTF)
								// 		printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
								// }
								// system("rm /var/www/sks.logtmp");
								// sync();

								IOTC_Session_Channel_ON(i, 1);
								socketDatainfo->flag_run_once = true;
								Logvedio_UDP_port_status = 0;
								pthread_exit(0);
							}
							else
							{
								// printf("====IOT webcam1 TUTK write over!!\n\n");
								IOTC_Session_Channel_ON(i, 1); // 20211221
							}
							break;
						}
					}
					if (strcmp(temp_str, TimeFormat()) != 0)
					{
						strcpy(temp_str, TimeFormat());
						if (DEBUG_PRINTF)
							printf("IOT_LogVedio(%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x)(ret=%d)(port:%d)(%s)\n", Buf_Data_Transfer_LogVedio[0], Buf_Data_Transfer_LogVedio[1], Buf_Data_Transfer_LogVedio[2], Buf_Data_Transfer_LogVedio[3], Buf_Data_Transfer_LogVedio[4], Buf_Data_Transfer_LogVedio[5], Buf_Data_Transfer_LogVedio[6], Buf_Data_Transfer_LogVedio[7], Buf_Data_Transfer_LogVedio[8], Buf_Data_Transfer_LogVedio[9], Buf_Data_Transfer_LogVedio[10], Buf_Data_Transfer_LogVedio[11], Buf_Data_Transfer_LogVedio[12], Buf_Data_Transfer_LogVedio[13], Buf_Data_Transfer_LogVedio[14], Buf_Data_Transfer_LogVedio[15], Buf_Data_Transfer_LogVedio[16], Buf_Data_Transfer_LogVedio[17], Buf_Data_Transfer_LogVedio[18], Buf_Data_Transfer_LogVedio[19], ret, socketDatainfo->port, TimeFormat());
						for (i = 0; i < MAX_SESSION; i++)
						{
							if (Flag_Trans_LogVedio[i])
							{
								if (DEBUG_PRINTF)
									printf("*******(SID=%d,on)\n", i);
							}
							else
							{
								// printf("(SID=%d,off)******\n",i);
							}
						}
					}
				}
			}
			else
			{
				usleep(10000);
			}
		}
		close(socketDatainfo->conn);
		close(socketDatainfo->sockfd);
		if (DEBUG_PRINTF)
			printf("wepcam2 port:%d\n", socketDatainfo->port);
	}
	pthread_exit(0);
}
void *ServerSocket_thread_webcam3(void *arg)
{
	char temp_str[30];
	int i;
	bool flag_first_run = true;
	char write_sys_log_buf[128];
	strcpy(temp_str, TimeFormat());
	while (true)
	{
		SocketData *socketDatainfo = (SocketData *)arg;
		socketDatainfo->sockfd = socket(AF_INET, SOCK_DGRAM, 0); /*IPV4流式协议即TCP协议*/
		if (DEBUG_PRINTF)
			printf("\n=====thread_ServerSocket_webcam3 port:%d=======\n", socketDatainfo->port);
		if (socketDatainfo->sockfd < 0)
			error_print("socket webcam3\n");
		struct sockaddr_in servaddr;
		socklen_t cliaddr_len = sizeof(servaddr);
		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET; /*IPV4*/
		servaddr.sin_port = htons(socketDatainfo->port);
		servaddr.sin_addr.s_addr = inet_addr(SOCKET_SERVER_IP); /*使用本地环回地址做测试*/
		// servaddr.sin_addr.s_addr = INADDR_ANY; // 在sockfd_one綁定bind之前，設置其埠復用
		/*inet_aton("127.0.0.1",&servaddr.sin_addr);//与inet_addr函数作用相同*/
		// pthread_t Thread_ServerSocket_write;
		/*setsockopt确保服务器不用等待TIME_WAIT状态结束就可以重启服务器，继续使用原来的端口号*/
		if (DEBUG_PRINTF)
			printf("1server loop  webcam3 port:%d!!\n", socketDatainfo->port);
		int on = 1;
		if (setsockopt(socketDatainfo->sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
			error_print("setsockopt webcam3");
		/*绑定本地Socket地址*/
		if (DEBUG_PRINTF)
			printf("2server loop  webcam3 port:%d!!\n", socketDatainfo->port);
		if (bind(socketDatainfo->sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
			error_print("bind webcam3");
		if (DEBUG_PRINTF)
			printf("*****5server loop  webcam3 port:%d!!\n", socketDatainfo->port);

		while (true)
		{
			if (Webcam_UDP_port_status & 0x04)
			{

				if (flag_first_run)
				{
					if (DEBUG_PRINTF)
						printf("webcam3 Webcam_UDP_port_status=%x)\n", Webcam_UDP_port_status);
					flag_first_run = false;
				}
				int ret;
				bzero(Buf_Data_Transfer_Webcam3, strlen((const char *)Buf_Data_Transfer_Webcam3));
				ret = recvfrom(socketDatainfo->sockfd, Buf_Data_Transfer_Webcam3, IOTC_MAX_PACKET_SIZE, 0, (struct sockaddr *)&servaddr, &cliaddr_len);

				// int ret = read(socketDatainfo->conn, Buf_Data_Transfer_Webcam,2 strlen((const char *)Buf_Data_Transfer_Webcam3));
				if (ret < 0)
				{
					if (DEBUG_PRINTF)
						printf("read : Connection reset by peer，Webcam3 socket read ret=%d!\n", ret);
					// break;
				}
				else if (ret == 0)
				{
					if (DEBUG_PRINTF)
						printf("Ctrl+C，Webcam3 socket read ret=%d!\n", ret);
					// break;
					// break;//父进程收到服务器端退出的信息（服务器Ctrl+C结束通信进程，read函数返回值为0，退出循环）
				}
				else
				{

					// printf("****webcam3 socket read_info right\n");
					Buf_Data_Size_Webcam3 = ret;
					// printf("UDC webcam3(%02x,%02x,%02x,%02x)(ret=%d)(count=%d)(%s)IOT ON\n",Buf_Data_Transfer_Webcam3[0],Buf_Data_Transfer_Webcam3[1],Buf_Data_Transfer_Webcam3[2],Buf_Data_Transfer_Webcam3[3],ret,count,TimeFormat());
					for (i = 0; i < MAX_SESSION; i++)
					{
						if (Flag_WebCam3SID[i])
						{
							ret = IOTC_Session_Write(i, (const char *)Buf_Data_Transfer_Webcam3, Buf_Data_Size_Webcam3, 1);
							if (ret == IOTC_ER_CH_NOT_ON)
							{
								if (DEBUG_PRINTF)
									printf("====IOT webcam3 IOTC_ER_CH_NOT_ON!!!!!!!!!!\n");
								IOTC_Session_Channel_ON(i, 1);
							}
							else if (ret < 0)
							{ // other error < 0 means this session cant use anymore
								if (DEBUG_PRINTF)
									printf("====IOT webcam3 IOTC_Session_Write error, code[%d]\n", ret);

								// IOTC_Session_Close(i);
								sprintf(write_sys_log_buf, "%s - aw.SWebcam3_logout_SID=%d_CID=%d(%d)! \n", TimeFormat(), i, 1, ret);
								// sprintf(write_sys_log_buf,"%s - aw.SLogVedio_logout_SID=%d_CID=%d(%d)! \n",TimeFormat(),i,1,ret);
								if (DEBUG_PRINTF)
									printf("%s\n\n", write_sys_log_buf);

								echo_file(write_sys_log_buf);

								// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf);  //SKS_FILE_PATH
								// sync();
								// system("cp /var/www/sks.log /var/www/sks.logtmp");
								// if(flash_write(FLASH_SKSLOG_START, 4096*2, "/var/www/sks.logtmp")){
								// 	if (DEBUG_PRINTF)
								// 		printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
								// }
								// else{
								// 	if (DEBUG_PRINTF)
								// 		printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
								// }
								// system("rm /var/www/sks.logtmp");
								// sync();
								IOTC_Session_Channel_ON(i, 1);
								socketDatainfo->flag_run_once = true;
								Webcam_UDP_port_status = 0;
								pthread_exit(0);
							}
							else
							{
								IOTC_Session_Channel_ON(i, 1); // 20211221
							}
						}
						else
						{
						}
					}
					if (strcmp(temp_str, TimeFormat()) != 0)
					{
						strcpy(temp_str, TimeFormat());
						if (DEBUG_PRINTF)
							printf("\nthread_webcam3 status=%02x,ret=%d))\n", Webcam_UDP_port_status, ret);
						if (DEBUG_PRINTF)
							printf("IOT_webcam3(%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x)(ret=%d)(%s)\n", Buf_Data_Transfer_Webcam3[0], Buf_Data_Transfer_Webcam3[1], Buf_Data_Transfer_Webcam3[2], Buf_Data_Transfer_Webcam3[3], Buf_Data_Transfer_Webcam3[4], Buf_Data_Transfer_Webcam3[5], Buf_Data_Transfer_Webcam3[6], Buf_Data_Transfer_Webcam3[7], Buf_Data_Transfer_Webcam3[8], Buf_Data_Transfer_Webcam3[9], Buf_Data_Transfer_Webcam3[10], Buf_Data_Transfer_Webcam3[11], Buf_Data_Transfer_Webcam3[12], Buf_Data_Transfer_Webcam3[13], Buf_Data_Transfer_Webcam3[14], Buf_Data_Transfer_Webcam3[15], Buf_Data_Transfer_Webcam3[16], Buf_Data_Transfer_Webcam3[17], Buf_Data_Transfer_Webcam3[18], Buf_Data_Transfer_Webcam3[19], ret, TimeFormat());
						for (i = 0; i < MAX_SESSION; i++)
						{
							if (Flag_WebCam3SID[i])
							{
								if (DEBUG_PRINTF)
									printf("*******(SID=%d,on)\n", i);
							}
							else
							{
								// printf("(SID=%d,off)******\n",i);
							}
						}
					}
				}
			}
			else if (Logvedio_UDP_port_status & 0x04)
			{
				int ret;
				bzero(Buf_Data_Transfer_LogVedio, IOTC_MAX_PACKET_SIZE);
				// // printf("\n*****************socket server read \n\n");
				ret = recvfrom(socketDatainfo->sockfd, Buf_Data_Transfer_LogVedio, IOTC_MAX_PACKET_SIZE, 0, (struct sockaddr *)&servaddr, &cliaddr_len);
				/*读取conn连接发送过来的数据*/
				// int ret = read(socketDatainfo->conn, Buf_Data_Transfer_LogVedio, strlen((const char *)Buf_Data_Transfer_LogVedio));
				if (ret < 0)
				{
					if (DEBUG_PRINTF)
						printf("read : Connection reset by peer，LogVedio socket read ret=%d!\n", ret);
					break;
				}
				else if (ret == 0)
				{
					if (DEBUG_PRINTF)
						printf("Ctrl+C，LogVedio socket read ret=%d!\n", ret);
					break;
					// break;//父进程收到服务器端退出的信息（服务器Ctrl+C结束通信进程，read函数返回值为0，退出循环）
				}
				else
				{
					Buf_Data_Size_LogVedio = ret;
					for (i = 0; i < MAX_SESSION; i++)
					{
						if (Flag_Trans_LogVedio[i])
						{
							ret = IOTC_Session_Write(i, (const char *)Buf_Data_Transfer_LogVedio, Buf_Data_Size_LogVedio, 1);
							if (ret == IOTC_ER_CH_NOT_ON)
							{
								if (DEBUG_PRINTF)
									printf("====IOT webcam1 IOTC_ER_CH_NOT_ON!!!!!!!!!!\n");
								IOTC_Session_Channel_ON(i, 1);
							}
							else if (ret < 0)
							{ // other error < 0 means this session cant use anymore
								if (DEBUG_PRINTF)
									printf("====IOT LogVedioSID IOTC_Session_Write error, code[%d]\n", ret);

								// IOTC_Session_Close(i);
								sprintf(write_sys_log_buf, "%s - aw.SLogVedio_logout_SID=%d_CID=%d(%d)! \n", TimeFormat(), i, 1, ret);
								// sprintf(write_sys_log_buf,"%s - aw.SLogVedio_logout_SID=%d_CID=%d(%d)! \n",TimeFormat(),i,1,ret);
								if (DEBUG_PRINTF)
									printf("%s\n\n", write_sys_log_buf);

								echo_file(write_sys_log_buf);
								// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf);  //SKS_FILE_PATH
								// sync();
								// system("cp /var/www/sks.log /var/www/sks.logtmp");
								// if(flash_write(FLASH_SKSLOG_START, 4096*2, "/var/www/sks.logtmp")){
								// 	if (DEBUG_PRINTF)
								// 		printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
								// }
								// else{
								// 	if (DEBUG_PRINTF)
								// 		printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
								// }
								// system("rm /var/www/sks.logtmp");
								// sync();
								IOTC_Session_Channel_ON(i, 1);
								socketDatainfo->flag_run_once = true;
								Logvedio_UDP_port_status = 0;
								pthread_exit(0);
							}
							else
							{
								// printf("====IOT webcam1 TUTK write over!!\n\n");
								IOTC_Session_Channel_ON(i, 1); // 20211221
							}
							break;
						}
					}
					if (strcmp(temp_str, TimeFormat()) != 0)
					{
						strcpy(temp_str, TimeFormat());
						if (DEBUG_PRINTF)
							printf("IOT_LogVedio(%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x)(ret=%d)(port:%d)(%s)\n", Buf_Data_Transfer_LogVedio[0], Buf_Data_Transfer_LogVedio[1], Buf_Data_Transfer_LogVedio[2], Buf_Data_Transfer_LogVedio[3], Buf_Data_Transfer_LogVedio[4], Buf_Data_Transfer_LogVedio[5], Buf_Data_Transfer_LogVedio[6], Buf_Data_Transfer_LogVedio[7], Buf_Data_Transfer_LogVedio[8], Buf_Data_Transfer_LogVedio[9], Buf_Data_Transfer_LogVedio[10], Buf_Data_Transfer_LogVedio[11], Buf_Data_Transfer_LogVedio[12], Buf_Data_Transfer_LogVedio[13], Buf_Data_Transfer_LogVedio[14], Buf_Data_Transfer_LogVedio[15], Buf_Data_Transfer_LogVedio[16], Buf_Data_Transfer_LogVedio[17], Buf_Data_Transfer_LogVedio[18], Buf_Data_Transfer_LogVedio[19], ret, socketDatainfo->port, TimeFormat());
						for (i = 0; i < MAX_SESSION; i++)
						{
							if (Flag_Trans_LogVedio[i])
							{
								if (DEBUG_PRINTF)
									printf("*******(SID=%d,on)\n", i);
							}
							else
							{
								// printf("(SID=%d,off)******\n",i);
							}
						}
					}
				}
			}
			else
			{
				usleep(10000);
			}
		}
		close(socketDatainfo->conn);
		close(socketDatainfo->sockfd);
		printf("wepcam3 port:%d\n", socketDatainfo->port);
	}
	pthread_exit(0);
}

void *ServerSocket_thread_webcam4(void *arg)
{
	char temp_str[30];
	strcpy(temp_str, TimeFormat());
	int i;
	bool flag_first_run = true;
	char write_sys_log_buf[128];
	while (true)
	{
		SocketData *socketDatainfo = (SocketData *)arg;
		// int sockfd = socket(AF_INET, SOCK_STREAM, 0);/*IPV4流式协议即TCP协议*/
		socketDatainfo->sockfd = socket(AF_INET, SOCK_DGRAM, 0); /*IPV4流式协议即TCP协议*/
		if (DEBUG_PRINTF)
			printf("\n=====thread_ServerSocket_webcam4 port:%d=======\n", socketDatainfo->port);
		if (socketDatainfo->sockfd < 0)
			error_print("socket webcam4\n");
		struct sockaddr_in servaddr;
		socklen_t cliaddr_len = sizeof(servaddr);
		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET; /*IPV4*/
		// servaddr.sin_port = socketDatainfo->port;
		servaddr.sin_port = htons(socketDatainfo->port);
		servaddr.sin_addr.s_addr = inet_addr(SOCKET_SERVER_IP); /*使用本地环回地址做测试*/
		// servaddr.sin_addr.s_addr = INADDR_ANY; // 在sockfd_one綁定bind之前，設置其埠復用
		/*inet_aton("127.0.0.1",&servaddr.sin_addr);//与inet_addr函数作用相同*/
		// pthread_t Thread_ServerSocket_write;
		/*setsockopt确保服务器不用等待TIME_WAIT状态结束就可以重启服务器，继续使用原来的端口号*/
		if (DEBUG_PRINTF)
			printf("1server loop  webcam4 port:%d!!\n", socketDatainfo->port);
		int on = 1;
		if (setsockopt(socketDatainfo->sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
			error_print("setsockopt webcam4");
		/*绑定本地Socket地址*/
		if (DEBUG_PRINTF)
			printf("2server loop  webcam4 port:%d!!\n", socketDatainfo->port);
		if (bind(socketDatainfo->sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
			error_print("bind webcam4");
		if (DEBUG_PRINTF)
			printf("*****5server loop  webcam4 port:%d!!\n", socketDatainfo->port);

		while (true)
		{

			if (Webcam_UDP_port_status & 0x08)
			{

				if (flag_first_run)
				{
					if (DEBUG_PRINTF)
						printf("\nwebcam2 Webcam_UDP_port_status=%x)\n", Webcam_UDP_port_status);
					flag_first_run = false;
				}

				int ret;
				bzero(Buf_Data_Transfer_Webcam4, IOTC_MAX_PACKET_SIZE);
				ret = recvfrom(socketDatainfo->sockfd, Buf_Data_Transfer_Webcam4, IOTC_MAX_PACKET_SIZE, 0, (struct sockaddr *)&servaddr, &cliaddr_len);

				/*读取conn连接发送过来的数据*/
				// int ret = read(socketDatainfo->conn, Buf_Data_Transfer_Webcam4, strlen((const char *)Buf_Data_Transfer_Webcam4));

				if (ret < 0)
				{
					if (DEBUG_PRINTF)
						printf("read : Connection reset by peer，Webcam4 socket read ret=%d!\n", ret);
					// break;
				}
				else if (ret == 0)
				{
					if (DEBUG_PRINTF)
						printf("Ctrl+C，Webcam4 socket read ret=%d!\n", ret);
					// break;
					// break;//父进程收到服务器端退出的信息（服务器Ctrl+C结束通信进程，read函数返回值为0，退出循环）
				}
				else
				{
					Buf_Data_Size_Webcam4 = ret;
					// printf("UDC webcam4(%02x,%02x,%02x,%02x)(ret=%d)(count=%d)(%s)IOT ON\n",Buf_Data_Transfer_Webcam4[0],Buf_Data_Transfer_Webcam4[1],Buf_Data_Transfer_Webcam4[2],Buf_Data_Transfer_Webcam4[3],ret,count,TimeFormat());
					for (i = 0; i < MAX_SESSION; i++)
					{
						if (Flag_WebCam4SID[i])
						{
							ret = IOTC_Session_Write(i, (const char *)Buf_Data_Transfer_Webcam4, Buf_Data_Size_Webcam4, 1);
							if (ret == IOTC_ER_CH_NOT_ON)
							{
								if (DEBUG_PRINTF)
									printf("====IOT webcam4 IOTC_ER_CH_NOT_ON!!!!!!!!!!\n");
								IOTC_Session_Channel_ON(i, 1);
							}
							else if (ret < 0)
							{ // other error < 0 means this session cant use anymore
								if (DEBUG_PRINTF)
									printf("====IOT webcam4 IOTC_Session_Write error, code[%d]\n", ret);

								// IOTC_Session_Close(i);
								sprintf(write_sys_log_buf, "%s - aw.SWebcam4_logout_SID=%d_CID=%d(%d)! \n", TimeFormat(), i, 1, ret);
								// sprintf(write_sys_log_buf,"%s - aw.SLogVedio_logout_SID=%d_CID=%d(%d)! \n",TimeFormat(),i,1,ret);
								if (DEBUG_PRINTF)
									printf("%s\n\n", write_sys_log_buf);

								echo_file(write_sys_log_buf);
								// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf);  //SKS_FILE_PATH
								// sync();
								// system("cp /var/www/sks.log /var/www/sks.logtmp");
								// if(flash_write(FLASH_SKSLOG_START, 4096*2, "/var/www/sks.logtmp")){
								// 	if (DEBUG_PRINTF)
								// 		printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
								// }
								// else{
								// 	if (DEBUG_PRINTF)
								// 		printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
								// }
								// system("rm /var/www/sks.logtmp");
								// sync();
								IOTC_Session_Channel_ON(i, 1);
								socketDatainfo->flag_run_once = true;
								Webcam_UDP_port_status = 0;
								pthread_exit(0);
							}
							else
							{
								IOTC_Session_Channel_ON(i, 1); // 20211221
							}
						}
					}
					if (strcmp(temp_str, TimeFormat()) != 0)
					{
						strcpy(temp_str, TimeFormat());
						// printf("IOT_webcam4(%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x)(ret=%d)(%s)\n",Buf_Data_Transfer_Webcam4[0],Buf_Data_Transfer_Webcam4[1],Buf_Data_Transfer_Webcam4[2],Buf_Data_Transfer_Webcam4[3],Buf_Data_Transfer_Webcam4[4],Buf_Data_Transfer_Webcam4[5],Buf_Data_Transfer_Webcam4[6],Buf_Data_Transfer_Webcam4[7],Buf_Data_Transfer_Webcam4[8],Buf_Data_Transfer_Webcam4[9],Buf_Data_Transfer_Webcam4[10],Buf_Data_Transfer_Webcam4[11],Buf_Data_Transfer_Webcam4[12],Buf_Data_Transfer_Webcam4[13],Buf_Data_Transfer_Webcam4[14],Buf_Data_Transfer_Webcam4[15],Buf_Data_Transfer_Webcam4[16],Buf_Data_Transfer_Webcam4[17],Buf_Data_Transfer_Webcam4[18],Buf_Data_Transfer_Webcam4[19],ret,TimeFormat());
						if (DEBUG_PRINTF)
							printf("\nthread_webcam4 status=%02x,ret=%d))\n", Webcam_UDP_port_status, ret);
						for (i = 0; i < MAX_SESSION; i++)
						{
							if (Flag_WebCam4SID[i])
							{
								if (DEBUG_PRINTF)
									printf("*******(SID=%d,on)\n", i);
							}
							else
							{
								// printf("(SID=%d,off)******\n",i);
							}
						}
					}
				}
			}
			else if (Logvedio_UDP_port_status & 0x08)
			{
				int ret;
				bzero(Buf_Data_Transfer_LogVedio, IOTC_MAX_PACKET_SIZE);
				// // printf("\n*****************socket server read \n\n");
				ret = recvfrom(socketDatainfo->sockfd, Buf_Data_Transfer_LogVedio, IOTC_MAX_PACKET_SIZE, 0, (struct sockaddr *)&servaddr, &cliaddr_len);
				/*读取conn连接发送过来的数据*/
				// int ret = read(socketDatainfo->conn, Buf_Data_Transfer_LogVedio, strlen((const char *)Buf_Data_Transfer_LogVedio));
				if (ret < 0)
				{
					if (DEBUG_PRINTF)
						printf("read : Connection reset by peer，LogVedio socket read ret=%d!\n", ret);
					break;
				}
				else if (ret == 0)
				{
					if (DEBUG_PRINTF)
						printf("Ctrl+C，LogVedio socket read ret=%d!\n", ret);
					break;
					// break;//父进程收到服务器端退出的信息（服务器Ctrl+C结束通信进程，read函数返回值为0，退出循环）
				}
				else
				{
					Buf_Data_Size_LogVedio = ret;
					for (i = 0; i < MAX_SESSION; i++)
					{
						if (Flag_Trans_LogVedio[i])
						{
							ret = IOTC_Session_Write(i, (const char *)Buf_Data_Transfer_LogVedio, Buf_Data_Size_LogVedio, 1);
							if (ret == IOTC_ER_CH_NOT_ON)
							{
								if (DEBUG_PRINTF)
									printf("====IOT webcam1 IOTC_ER_CH_NOT_ON!!!!!!!!!!\n");
								IOTC_Session_Channel_ON(i, 1);
							}
							else if (ret < 0)
							{ // other error < 0 means this session cant use anymore

								if (DEBUG_PRINTF)
									printf("====IOT LogVedioSID IOTC_Session_Write error, code[%d]\n", ret);

								// IOTC_Session_Close(i);
								sprintf(write_sys_log_buf, "%s - aw.SLogVedio_logout_SID=%d_CID=%d(%d)! \n", TimeFormat(), i, 1, ret);
								// sprintf(write_sys_log_buf,"%s - aw.SLogVedio_logout_SID=%d_CID=%d(%d)! \n",TimeFormat(),i,1,ret);
								if (DEBUG_PRINTF)
									printf("%s\n", write_sys_log_buf);
								// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf);  //SKS_FILE_PATH
								// socketDatainfo.sys_backup_count++;
								echo_file(write_sys_log_buf);

								// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf);  //SKS_FILE_PATH
								// sync();
								// system("cp /var/www/sks.log /var/www/sks.logtmp");
								// if(flash_write(FLASH_SKSLOG_START, 4096*2, "/var/www/sks.logtmp")){
								// 	if (DEBUG_PRINTF)
								// 		printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
								// }
								// else{
								// 	if (DEBUG_PRINTF)
								// 		printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
								// }
								// system("rm /var/www/sks.logtmp");
								// sync();
								IOTC_Session_Channel_ON(i, 1);
								socketDatainfo->flag_run_once = true;
								Logvedio_UDP_port_status = 0;
								pthread_exit(0);
							}
							else
							{
								// printf("====IOT webcam1 TUTK write over!!\n\n");
								IOTC_Session_Channel_ON(i, 1); // 20211221
							}
							break;
						}
					}
					if (strcmp(temp_str, TimeFormat()) != 0)
					{
						strcpy(temp_str, TimeFormat());
						if (DEBUG_PRINTF)
							printf("IOT_LogVedio(%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x)(ret=%d)(port:%d)(%s)\n", Buf_Data_Transfer_LogVedio[0], Buf_Data_Transfer_LogVedio[1], Buf_Data_Transfer_LogVedio[2], Buf_Data_Transfer_LogVedio[3], Buf_Data_Transfer_LogVedio[4], Buf_Data_Transfer_LogVedio[5], Buf_Data_Transfer_LogVedio[6], Buf_Data_Transfer_LogVedio[7], Buf_Data_Transfer_LogVedio[8], Buf_Data_Transfer_LogVedio[9], Buf_Data_Transfer_LogVedio[10], Buf_Data_Transfer_LogVedio[11], Buf_Data_Transfer_LogVedio[12], Buf_Data_Transfer_LogVedio[13], Buf_Data_Transfer_LogVedio[14], Buf_Data_Transfer_LogVedio[15], Buf_Data_Transfer_LogVedio[16], Buf_Data_Transfer_LogVedio[17], Buf_Data_Transfer_LogVedio[18], Buf_Data_Transfer_LogVedio[19], ret, socketDatainfo->port, TimeFormat());
						for (i = 0; i < MAX_SESSION; i++)
						{
							if (Flag_Trans_LogVedio[i])
							{
								if (DEBUG_PRINTF)
									printf("*******(SID=%d,on)\n", i);
							}
							else
							{
								// printf("(SID=%d,off)******\n",i);
							}
						}
					}
				}
			}
			else
			{
				usleep(10000);
			}
			// usleep(10000);    ///
			/* code */
		}
		close(socketDatainfo->conn);
		close(socketDatainfo->sockfd);
		if (DEBUG_PRINTF)
			printf("wepcam4 port:%d\n", socketDatainfo->port);
	}
	pthread_exit(0);
}

void *Pidof_thread_work()
{
	// char ss_adjust_str[50];
	bool ss_flag_run_once = true;
	FILE *ss_fp;
	// int ss_Rows=0;
	char ss_buf[DATA_SIZE];
	char ss_kill_string[50];
	int ss_pid = 0;
	char buf[512];

	char temp[50];
	// char write_sys_log_buf[256];
	bool every_time_once = false;
	bool upload_once = false;
	struct timeval tv_now;
	struct timezone tz_now;

	int ss_mem_init_size = 0;
	int ss_mem_max_size = 0;
	int count = 0;
	struct stat buffer;
	int exist;
	int res_check;

	// char temp_str[10];

	// bool Flag_first_time_run_upload=true;

	sleep(5);
	system("killall Heartbeat_8198c");

	sleep(5);
	system(HEARTBEATSTRING_EXEC);
	system("pidof Heartbeat_8198c");
	if (DEBUG_PRINTF)
		printf("\nCHECK_SS_TIME=%d\n", CHECK_SS_TIME);

	system("ps > /var/www/ps.log");
	ss_mem_init_size = check_ps_log("/var/www/ps.log", SOCKETSERVER_FILE_NAME, count);

	if (DEBUG_PRINTF)
		printf("%s mem init size=%d)\n", SOCKETSERVER_FILE_NAME, ss_mem_init_size);

	if (ss_mem_init_size == 0)
	{
		ss_mem_init_size = 9000;
	}

	if (ss_mem_init_size > 10000)
	{
		ss_mem_max_size = ss_mem_init_size * 1.2;
	}
	else
	{
		ss_mem_max_size = ss_mem_init_size * 1.5;
	}
	if (DEBUG_PRINTF)
		printf("%s mem max size=%d,init size=%d)\n", SOCKETSERVER_FILE_NAME, MAX_SOCKERT_SEV_MEM_SIZE, ss_mem_init_size);
	system("rm /var/www/ps.log");

	while (true)
	{
		usleep(500);
		gettimeofday(&tv_now, &tz_now);
		// if (socketDatainfo.sys_backup_count>40){
		// 	sprintf(temp_str,"cp /var/www/sks.log /tmp/usb/sda1/log/%s_sks.log",TimeFormatDay());
		// 	system(temp_str);
		// 	socketDatainfo.sys_backup_count=0;

		// 	printf("upload sks.log\n");
		// 	system("upload_8198 &");
		// 	// if (DEBUG_PRINTF)
		// 	// 	printf("\nsocket server do sys.log backup\n");
		// 	// if (DEBUG_PRINTF)
		// 	// 	printf("remove befor last year log\n");
		// 	// sprintf(temp_str,"rm /tem/usb/sda1/log/*.log");
		// 	// if (DEBUG_PRINTF)
		// 	// 	printf("exec (%s)\n",temp_str);
		// 	// system(temp_str);
		// }

		if ((tv_now.tv_sec % (24 * 60 * 60)) == (21.5 - 8) * 60 * 60 + socketDatainfo.upload_sec)
		{
			// upload_once
			if (upload_once)
			{
				upload_once = false;
				Flag_upload_log = 1;
				Flag_del_log = 1;
				sprintf(buf, "%s - aw.time_upload_log! \n", TimeFormat());
				echo_file(buf);
			}
		}
		else
		{
			upload_once = true;
		}

		if (Flag_download_config)
		{
			Flag_download_config = 0;
			memset(buf, 0, sizeof(buf));
			sprintf(buf, "%s - aw.APP_download_config_run! \n", TimeFormat());
			if (DEBUG_PRINTF)
				printf("%s", buf);
			WriteToFileSys(FILE_PATH, buf); // write heartbeat message to file;
			system("cp /var/www/sys.log /var/www/sys.logtmp");
			if (flash_write(FLASH_UISLOG, 4096 * 2, "/var/www/sys.logtmp"))
			{
				if (DEBUG_PRINTF)
					printf("%s: %s (%d)flash_write fail\n", __FILE__, __func__, __LINE__);
			}
			else
			{
				if (DEBUG_PRINTF)
					printf("%s: %s (%d)flash_write OK\n", __FILE__, __func__, __LINE__);
			}
			system("rm /var/www/sys.logtmp");
			sync();
			echo_file(buf);
			system("upload_8198 usbdownload&");
		}

		if (Flag_upload_config)
		{
			Flag_upload_config = 0;
			memset(buf, 0, sizeof(buf));
			sprintf(buf, "%s - aw.APP_upload_config_run! \n", TimeFormat());
			if (DEBUG_PRINTF)
				printf("%s", buf);
			WriteToFileSys(FILE_PATH, buf); // write heartbeat message to file;
			system("cp /var/www/sys.log /var/www/sys.logtmp");
			if (flash_write(FLASH_UISLOG, 4096 * 2, "/var/www/sys.logtmp"))
			{
				if (DEBUG_PRINTF)
					printf("%s: %s (%d)flash_write fail\n", __FILE__, __func__, __LINE__);
			}
			else
			{
				if (DEBUG_PRINTF)
					printf("%s: %s (%d)flash_write OK\n", __FILE__, __func__, __LINE__);
			}
			system("rm /var/www/sys.logtmp");
			sync();
			echo_file(buf);
			system("upload_8198 usbupload&");
		}

		if (Flag_upload_log)
		{
			Flag_upload_log = 0;
			// printf("upload file %s\n",HourFormat());
			// system("upload_8198 &");
			// sleep(10);
			res_check = check_command_response_string(CHECK_USB_DISK_COMMAND, USB_DISK_PATH); // USB DISK check

			if (res_check)
			{
				// usb disk non found
				printf("usb disk non found and reset usb\n");
				system("Daemon -usbreset");

				memset(buf, 0, sizeof(buf));
				sprintf(buf, "%s - aw.USB_disk_non_found_and_reset_usb(res=%d)! \n", TimeFormat(), res_check);
				if (DEBUG_PRINTF)
					printf("%s", buf);
				WriteToFileSys(FILE_PATH, buf); // write heartbeat message to file;
				system("cp /var/www/sys.log /var/www/sys.logtmp");
				if (flash_write(FLASH_UISLOG, 4096 * 2, "/var/www/sys.logtmp"))
				{
					if (DEBUG_PRINTF)
						printf("%s: %s (%d)flash_write fail\n", __FILE__, __func__, __LINE__);
				}
				else
				{
					if (DEBUG_PRINTF)
						printf("%s: %s (%d)flash_write OK\n", __FILE__, __func__, __LINE__);
				}
				system("rm /var/www/sys.logtmp");
				sync();
				echo_file(buf);
			}
			else
			{
				exist = stat(SKSLOG_FILE, &buffer);
				if (exist == 0)
				{
					if (DEBUG_PRINTF)
						printf("%s exit!\n", SKSLOG_FILE);
					sync();
					memset(buf, 0, sizeof(buf));
					sprintf(buf, "%s - aw.Upload_run! \n", TimeFormat());
					if (DEBUG_PRINTF)
						printf("%s", buf);
					echo_file(buf);
					printf("upload file %s\n", HourFormat());
					if (Flag_del_log)
					{
						system("upload_8198 del_log &");
						Flag_del_log = 0;
					}
					else
					{
						system("upload_8198 keep_log &");
					}
				}
				else
				{
					// if (DEBUG_PRINTF)
					printf("%s non_exist!\n", SKSLOG_FILE);
					memset(buf, 0, sizeof(buf));
					sprintf(buf, "%s - aw.USB_upload_sks_log_FILE_non_exist! \n", TimeFormat());
					if (DEBUG_PRINTF)
						printf("%s", buf);
					WriteToFileSys(FILE_PATH, buf); // write heartbeat message to file;
					system("cp /var/www/sys.log /var/www/sys.logtmp");
					if (flash_write(FLASH_UISLOG, 4096 * 2, "/var/www/sys.logtmp"))
					{
						if (DEBUG_PRINTF)
							printf("%s: %s (%d)flash_write fail\n", __FILE__, __func__, __LINE__);
					}
					else
					{
						if (DEBUG_PRINTF)
							printf("%s: %s (%d)flash_write OK\n", __FILE__, __func__, __LINE__);
					}
					system("rm /var/www/sys.logtmp");
					sync();
					echo_file(buf);
				}
			}
		}

		if ((tv_now.tv_sec % HB_RESTART_SEC) == HB_RESTART_DELAY_SEC + socketDatainfo.hb_restart_sec)
		{
			if (every_time_once)
			{
				every_time_once = false;
				Flag_restart_HB = 1;
			}
		}
		else
		{
			every_time_once = true;
		}
		/////////////////////////////////
		if (Flag_restart_HB)
		{
			if (DEBUG_PRINTF)
				printf("(now time=%s)restart_HB\n\n", HourFormat());

			system("killall Heartbeat_8198c");
			usleep(10000);
			system("./Heartbeat_8198c &");
			Flag_restart_HB = 0;
		}
		//////////////////////////////////////////////
		gettimeofday(&tv_now, &tz_now);
		// printf("tv_now_sec:%ld\n",tv_now.tv_sec);

		if ((tv_now.tv_sec % 60) == CHECK_SS_TIME)
		{
			system("ps > /var/www/ps.log");
			if (ss_flag_run_once)
			{
				ss_flag_run_once = false;
				count++;
				if (check_ps_log("/var/www/ps.log", HEARTBEAT_FILE_NAME, count) == 0)
				{
					// check without heartbeat
					system("killall Heartbeat_8198c");
					usleep(10000);
					system("./Heartbeat_8198c &");
					sprintf(ss_buf, "%s - aw.%s! \n", TimeFormat(), "AGENT SEND_ERROR-404-HBRBT");
					if (DEBUG_PRINTF)
						printf("%s", ss_buf);

					echo_file(ss_buf);

					// socketDatainfo.sys_backup_count++;
					// WriteToFileSys(SKS_FILE_PATH,ss_buf);  //SKS_FILE_PATH
					// sync();
					// system("cp /var/www/sks.log /var/www/sks.logtmp");
					// if(flash_write(FLASH_SKSLOG_START, 4096*2, "/var/www/sks.logtmp")){
					// 	if (DEBUG_PRINTF)
					// 		printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
					// }
					// else{
					// 	if (DEBUG_PRINTF)
					// 		printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
					// }
					// system("rm /var/www/sks.logtmp");
					// sync();
				}
				if (DEBUG_PRINTF)
					printf("check %s mem max size=%d,init size=%d )\n", SOCKETSERVER_FILE_NAME, MAX_SOCKERT_SEV_MEM_SIZE, ss_mem_init_size);
				if (check_ps_log("/var/www/ps.log", SOCKETSERVER_FILE_NAME, count) > MAX_SOCKERT_SEV_MEM_SIZE)
				{
					sprintf(ss_buf, "%s - aw.%s_%s_MEM_SIZE(%d)! \n", TimeFormat(), "mem_issue_reset", SOCKETSERVER_FILE_NAME, MAX_SOCKERT_SEV_MEM_SIZE);
					if (DEBUG_PRINTF)
						printf("%s", ss_buf);
					echo_file(ss_buf);

					sprintf(ss_buf, "pidof %s > %s", SOCKETSERVER_FILE_NAME, SOCKETSERVER_TXT_DATA);
					system(ss_buf);
					usleep(10000);
					sprintf(ss_buf, "cat %s", SOCKETSERVER_TXT_DATA);
					system(ss_buf);
					memset(ss_buf, 0, DATA_SIZE);
					// if (Debug_Socket_server_webapi_String) printf("filename=%s\n", "ii.txt");
					if (NULL == (ss_fp = fopen(SOCKETSERVER_TXT_DATA, "r")))
					{
						if (DEBUG_PRINTF)
							printf("no file,no data %s\n", SOCKETSERVER_TXT_DATA);
						// fp = fopen(filename, "w");
					}
					else
					{
						// iot_Rows = 0;
						// printf("uu\n");
						memset(ss_buf, 0, DATA_SIZE);
						if (fgets(ss_buf, sizeof(ss_buf), ss_fp) != NULL)
						{
							// printf("%d)%s",ss_Rows,ss_webapibuf);
							ss_pid = atoi(ss_buf);
							// printf("pid=%d\n",ss_pid);
							sprintf(ss_kill_string, "kill %d", ss_pid);
							system(ss_kill_string);
						}
						else
						{
							if (DEBUG_PRINTF)
								printf("NULL\n");
						}
						sprintf(temp, "rm %s", SOCKETSERVER_TXT_DATA);
						if (DEBUG_PRINTF)
							printf(temp);
						system(temp);
						fclose(ss_fp);
					}
				}
				if (count >= MEM_CHECK_MIN)
					count = 0;
			}
			system("rm /var/www/ps.log");
		}
		else
		{
			ss_flag_run_once = true;
		}
	}
}

void *ServerSocket_thread_info(void *arg)
{
	while (true)
	{
		// SocketData *socketDatainfo=(SocketData *)arg;
		socketDatainfo.sockfd = socket(AF_INET, SOCK_STREAM, 0); /*IPV4流式协议即TCP协议*/
		if (DEBUG_PRINTF)
			printf("\n=====thread_ServerSocket_info port:%d=======\n", socketDatainfo.port);
		if (socketDatainfo.sockfd < 0)
			error_print("socket\n");
		int flags = fcntl(socketDatainfo.sockfd, F_GETFL, 0);
		fcntl(socketDatainfo.sockfd, F_SETFL, flags | O_NONBLOCK);
		struct timeval timeout; //={0,100};//3s
		timeout.tv_sec = 3;
		timeout.tv_usec = 0;
		setsockopt(socketDatainfo.sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
		setsockopt(socketDatainfo.sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

		struct sockaddr_in servaddr;
		// socklen_t servaddr_len = sizeof(servaddr);
		bzero(&servaddr, sizeof(servaddr));
		// socklen_t servaddr_len = sizeof(servaddr);
		servaddr.sin_family = AF_INET; /*IPV4*/

		servaddr.sin_port = socketDatainfo.port;
		servaddr.sin_addr.s_addr = inet_addr(SOCKET_SERVER_IP); /*使用本地环回地址做测试*/
		/*inet_aton("127.0.0.1",&servaddr.sin_addr);//与inet_addr函数作用相同*/
		// pthread_t Thread_ServerSocket_write;

		int on = 1;
		if (setsockopt(socketDatainfo.sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
			error_print("setsockopt");

		/*绑定本地Socket地址*/
		if (bind(socketDatainfo.sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
			error_print("bind");
		/*监听连接*/
		if (listen(socketDatainfo.sockfd, SOMAXCONN) < 0)
			error_print("listen");
		struct sockaddr_in peeraddr; /*存储连接成功的客户端Socket信息*/
		socklen_t peerlen = sizeof(peeraddr);
		/*接收监听队列第一个完成连接的请求*/
		char temp_str[30] = {0};
		strcpy(temp_str, TimeFormat());

		int j = 0;

		while (1)
		{
			socketDatainfo.conn = accept(socketDatainfo.sockfd, (struct sockaddr *)&peeraddr, &peerlen);
			if (socketDatainfo.conn == -1 && errno == EAGAIN)
			{
				if (strcmp(temp_str, TimeFormat()) != 0)
				{
					strcpy(temp_str, TimeFormat());
					fprintf(stderr, "j=%d)conn=%d,no client connections yet\n", ++j, socketDatainfo.conn);
				}
				continue;
			}
			else if (socketDatainfo.conn == -1)
			{
				if (DEBUG_PRINTF)
					printf("j=%d)conn=%d", ++j, socketDatainfo.conn);
				perror("accept failed");

				// return 1;
			}
			else
			{
				if (DEBUG_PRINTF)
					printf("j=%d)conn=%d", ++j, socketDatainfo.conn);
				break;
			}
		}

		if (DEBUG_PRINTF)
			printf("\nserver loop1 port:%d!!\n\n", socketDatainfo.port);
		system("ntpclient -s -h pool.ntp.org -i 5");

		while (true)
		{
		}
		close(socketDatainfo.conn);
		close(socketDatainfo.sockfd);
	}
	pthread_exit(0);
}

/////////////////////////////
void *thread_ForSessionRead(void *arg)
{
	TargetChannel *pTargetChl = (TargetChannel *)arg;
	unsigned char ChID = pTargetChl->ChID;
	int SID = pTargetChl->nSID;
	int i, j = 0;
	int k = 0, l = 0;
	struct st_SInfoEx Sinfo;
	int ret = 0;
	bool flag_transfer_data = false;
	bool flag_70_skin = false;
	char write_sys_log_buf[128];

	bool app_potocal_logout = false;

	int ret_webcam_logout_exit = 0;
	Flag_SID_Login_Run_IOT[SID] = false;

	int data_size;

	char webcam_exit_data_send[20];
	char webcam_exit_data_recive[20];
	webcam_exit_data_send[0] = 0x06;
	webcam_exit_data_send[1] = 0x00;
	webcam_exit_data_send[2] = 0x00;
	webcam_exit_data_send[3] = 0x00;
	webcam_exit_data_send[4] = 0x02;
	webcam_exit_data_send[5] = 0x00;
	webcam_exit_data_send[6] = 0x00;
	webcam_exit_data_send[7] = 0x00;
	webcam_exit_data_send[8] = 0x00;
	webcam_exit_data_send[9] = 0x00;
	webcam_exit_data_send[10] = 0x01;
	webcam_exit_data_send[11] = 0x00;
	webcam_exit_data_send[12] = 0x03;
	webcam_exit_data_send[13] = 0x00;
	webcam_exit_data_send[14] = 0x00;
	webcam_exit_data_send[15] = 0x00;
	webcam_exit_data_send[16] = 0x00;
	webcam_exit_data_send[17] = 0x00;

	struct timeval tv_set, tv_now;
	struct timezone tz_set, tz_now;
	if (DEBUG_PRINTF)
		printf("[thread_ForSessionRead] Thread started...[SID=%d,ChID=%d]\n", SID, ChID);

	Sinfo.size = sizeof(struct st_SInfoEx);
	IOTC_Session_Check_Ex(SID, &Sinfo);
	if (Sinfo.Mode == 0)
	{
		if (DEBUG_PRINTF)
			printf("Client is from %s:%d[%s] Mode=P2P(mode:%d)\n", Sinfo.RemoteIP, Sinfo.RemotePort, Sinfo.UID, Sinfo.Mode);
	}
	else if (Sinfo.Mode == 1)
	{
		if (DEBUG_PRINTF)
			printf("Client is from %s:%d[%s] Mode=RLY(mode:%d)\n", Sinfo.RemoteIP, Sinfo.RemotePort, Sinfo.UID, Sinfo.Mode);
	}
	else if (Sinfo.Mode == 2)
	{
		if (DEBUG_PRINTF)
			printf("Client is from %s:%d[%s] Mode=LAN(mode:%d)\n", Sinfo.RemoteIP, Sinfo.RemotePort, Sinfo.UID, Sinfo.Mode);
	}
	if (DEBUG_PRINTF)
		printf("VPG [VID=%d:PID=%d:GID=%d] Remote NAT Type[%d]\n", Sinfo.VID, Sinfo.PID, Sinfo.GID, Sinfo.RemoteNatType);
	IOTC_Session_Channel_ON(SID, ChID);
	IOTC_Session_Channel_ON(SID, 1);
	bool data_run = false;
	char temp_st[30] = {0};
	strcpy(temp_st, TimeFormat());
	bool flag_51_coooamd = false; // remote alarm zone set
	while (true)
	{
		/////shm data
		while ((pTargetChl->nRead = IOTC_Session_Read(SID, (char *)pTargetChl->Buffer_TUTK_data, IOTC_MAX_PACKET_SIZE, 6000, ChID)) > -1) //? 這裡暫時設定六秒接收資料(其實應該是兩個執行序,輪流接收資料)
		{
			gettimeofday(&tv_set, &tz_set);
			// printf("!!!!!!!SID=%d),tv_set_sec:%ld\n\n",SID,tv_set.tv_sec);
			// printf("tv_set_usec:%ld\n",tv_set.tv_usec);

			if (pTargetChl->Buffer_TUTK_data[0] == 0x01)
			{
				Flag_SID_Login_Run_IOT[SID] = true;
			}
			// if(ShowIot_read_write_data){

			// printf("\nIot data write===>\n");
			// if (DEBUG_PRINTF)
			if (IOT_COMMAMD_AND_ITME)
			{
				// if (((pTargetChl->Buffer_TUTK_data[0]==3)&(pTargetChl->Buffer_TUTK_data[3]==3))|((pTargetChl->Buffer_TUTK_data[0]==5)&(pTargetChl->Buffer_TUTK_data[3]==6))){
				if ((pTargetChl->Buffer_TUTK_data[0] == 0x03) & (pTargetChl->Buffer_TUTK_data[3] == 0x14))
				{
					printf("\n\nAPP to AGENT read time===>%s)\n", TimeFormat());
					printf("(0x%02x,0x%02x) APP to AGENT read size=>%d,SID=>%d,ChID=>%d\n", pTargetChl->Buffer_TUTK_data[0], pTargetChl->Buffer_TUTK_data[3], pTargetChl->nRead, SID, ChID);

					// if (pTargetChl->nRead>16){
					if (pTargetChl->nRead > 32)
					{
						data_size = 32;
					}
					else
					{
						data_size = pTargetChl->nRead;
					}

					for (i = 0; i < (data_size); i++)
					{
						printf("0x%02x,", pTargetChl->Buffer_TUTK_data[i]);
						if ((i + 1) == 16)
						{
							printf("\n");
						}
					}
					printf("\n");
					// }
				}
				// }
			}

			if ((pTargetChl->Buffer_TUTK_data[0] == 0x3) & (pTargetChl->Buffer_TUTK_data[3] == 0x20))
			{
				// printf("==(0x3,0x20)always skip\n");
				data_run = false;
			}
			else
			{ // gat data show
				if (pTargetChl->Buffer_TUTK_data[0] == 0x0a)
				{
				}
				else
				{
					if ((pTargetChl->Buffer_TUTK_data[0] == 5) & (pTargetChl->Buffer_TUTK_data[3] == 1))
					{
						flag_51_coooamd = true;
						if (Debuf_51command_data_show)
						{
							if (DEBUG_PRINTF)
								printf("\n recive (5,1) rework data****\n");
							for (l = 0; l < pTargetChl->nRead; l++)
							{
								if (DEBUG_PRINTF)
									printf("0x%02x,", pTargetChl->Buffer_TUTK_data[l]);
								if (DEBUG_PRINTF)
									if (((l + 1) % 16) == 0)
										printf("\n");
							}
							if (DEBUG_PRINTF)
								printf("\n*****End****\n");
						}

						pTargetChl->Buffer_TUTK_data[3] = 6;
						// //step2
						// pTargetChl->Buffer_TUTK_data[10]=0x78;
						// pTargetChl->Buffer_TUTK_data[11]=0x03;
						// pTargetChl->Buffer_TUTK_data[12]=0x03;
						// step3
						pTargetChl->Buffer_TUTK_data[4] = 2;
						pTargetChl->Buffer_TUTK_data[17] = 1;
						pTargetChl->nRead = 18;

						if (Debuf_51command_data_show)
						{
							if (DEBUG_PRINTF)
								printf("\n (5,1)-->(5,6) rework data ok****\n");
							for (l = 0; l < pTargetChl->nRead; l++)
							{
								if (DEBUG_PRINTF)
									printf("0x%02x,", pTargetChl->Buffer_TUTK_data[l]);
								if (DEBUG_PRINTF)
									if (((l + 1) % 16) == 0)
										printf("\n");
							}
							if (DEBUG_PRINTF)
								printf("\n*****End****\n");
						}
					}
					if (Debug_WebCam_PlayBack_AppVoice)
					{
						if ((pTargetChl->Buffer_TUTK_data[0] == 0x04) | (pTargetChl->Buffer_TUTK_data[0] == 0x05) | (pTargetChl->Buffer_TUTK_data[0] == 0x06) | (pTargetChl->Buffer_TUTK_data[0] == 0x07) | (pTargetChl->Buffer_TUTK_data[0] == 0x0a))
						{
							// printf("== (0x%x,0x%x)",pTargetChl->Buffer_TUTK_data[0],pTargetChl->Buffer_TUTK_data[3]);
						}
					}
					else
					{
						if (DEBUG_PRINTF)
						{
							if (Debug_Tutk_Show56_Data)
							{
								if ((pTargetChl->Buffer_TUTK_data[0] == 5))
								{
									printf("== (0x%x,0x%x) run", pTargetChl->Buffer_TUTK_data[0], pTargetChl->Buffer_TUTK_data[3]);
									if (ShowIotDetialData)
									{
										printf("\n2All data:\n");
										for (j = 0; j < pTargetChl->nRead; j++)
										{
											printf("%02x,", pTargetChl->Buffer_TUTK_data[j]);
											if (((j + 1) % 16) == 0)
											{
												printf("\n");
											}
										}
										printf("\n\n");
									}
								}
							}
							else
							{
								printf("== (0x%x,0x%x) run", pTargetChl->Buffer_TUTK_data[0], pTargetChl->Buffer_TUTK_data[3]);
								if (ShowIotDetialData)
								{
									printf("\n3All data:\n");
									for (j = 0; j < pTargetChl->nRead; j++)
									{
										printf("%02x,", pTargetChl->Buffer_TUTK_data[j]);
										if (((j + 1) % 16) == 0)
										{
											printf("\n");
										}
									}
									printf("\n\n********\n");
								}
							}
						}
					}
				}
				if (Flag_SID_Login_Run_IOT[SID])
				{
					data_run = true;
				}
				else
				{
					data_run = false;
				}
			}

			// show data
			if (pTargetChl->Buffer_TUTK_data[0] == 0x0a)
			{
				if (strcmp(temp_st, TimeFormat()) != 0)
				{
					strcpy(temp_st, TimeFormat());
					if (DEBUG_PRINTF)
						printf("sound run tutk== (0x%x,0x%x,0x%x,0x%x,size=%d,SID=%d,%s)\n", pTargetChl->Buffer_TUTK_data[0], pTargetChl->Buffer_TUTK_data[1], pTargetChl->Buffer_TUTK_data[2], pTargetChl->Buffer_TUTK_data[3], pTargetChl->nRead, SID, TimeFormat());
				}
			}
			else
			{
				if (Debug_WebCam_PlayBack_AppVoice)
				{
					if ((pTargetChl->Buffer_TUTK_data[0] == 0x04) | (pTargetChl->Buffer_TUTK_data[0] == 0x05) | (pTargetChl->Buffer_TUTK_data[0] == 0x06) | (pTargetChl->Buffer_TUTK_data[0] == 0x07) | (pTargetChl->Buffer_TUTK_data[0] == 0x0a))
					{
						if (DEBUG_PRINTF)
							printf("(0x%02x,0x%02x)!!p2p data (size=%d) (SID=%d)get(%s)\n", pTargetChl->Buffer_TUTK_data[0], pTargetChl->Buffer_TUTK_data[3], pTargetChl->nRead, SID, TimeFormat());
						// printf("All data=\n");
						// for (j=0;j<pTargetChl->nRead;j++){
						// 	printf("0x%02x,",pTargetChl->Buffer_TUTK_data[j]);
						// }
						// printf("\n");
					}
				}
				else
				{
					if (Debug_Tutk_Show56_Data)
					{
						if ((pTargetChl->Buffer_TUTK_data[0] == 5))
						{
							if (DEBUG_PRINTF)
								printf("(0x%02x,0x%02x)!!p2p data (size=%d) (SID=%d)get(%s)\n", pTargetChl->Buffer_TUTK_data[0], pTargetChl->Buffer_TUTK_data[3], pTargetChl->nRead, SID, TimeFormat());
						}
					}
					else
					{
						if (DEBUG_PRINTF)
							printf("(0x%02x,0x%02x)!!p2p data (size=%d) (SID=%d)get(%s)\n", pTargetChl->Buffer_TUTK_data[0], pTargetChl->Buffer_TUTK_data[3], pTargetChl->nRead, SID, TimeFormat());
					}
				}
				// printf("(0x%02x,0x%02x)!!p2p data (size=%d) (SID=%d)get(%s)\n",pTargetChl->Buffer_TUTK_data[0],pTargetChl->Buffer_TUTK_data[3],pTargetChl->nRead,SID,TimeFormat());
			}

			if (data_run)
			{
				////////////////////////////////////////////////////////////////////////////////////////
				// IOTC_Session_Write(SID, buf, IOTC_MAX_PACKET_SIZE, ChID);
				// # TODO:這裡接收到的命令要做相對應的動作

				// pTargetChl->Buffer_TUTK_data[pTargetChl->nRead] = '\0';
				if (DEBUG_PRINTF)
					if (ShowDataMassage) // show data before run
						printf("[IOTC_Session_Read] SID=%d, ChID = %d, Size=(%d);ArrayData=(\n", SID, ChID, pTargetChl->nRead);
				pTargetChl->read_data_size = 0;
				for (i = 0; i < pTargetChl->nRead; i++)
				{
					if (i == 4)
						pTargetChl->read_data_size = pTargetChl->read_data_size + pTargetChl->Buffer_TUTK_data[i];
					if (i == 5)
						pTargetChl->read_data_size = pTargetChl->read_data_size + pTargetChl->Buffer_TUTK_data[i] * 256;
					if ((i > 15) && (i < (16 + pTargetChl->read_data_size)))
					{
						pTargetChl->read_data_array[i - 16] = pTargetChl->Buffer_TUTK_data[i];
					}
				}
				pTargetChl->read_data_array[pTargetChl->read_data_size] = 0;
				if (DEBUG_PRINTF)
					if (ShowDataMassage)
						printf("******data=(%s),gw=(%s)*****\n", pTargetChl->read_data_array, pTargetChl->gw);
				pTargetChl->Flag_TUTK_read = 1;

				Buffer_Transfer_Data_Size = pTargetChl->nRead;
				memcpy(Buffer_Data_Transfer, pTargetChl->Buffer_TUTK_data, Buffer_Transfer_Data_Size);
				// if ((Buffer_Data_Transfer[0]==6)|(Buffer_Data_Transfer[0]==7)|(Buffer_Data_Transfer[3]==7))//////////

				if (Buffer_Data_Transfer[0] == 0x0a)
				{
					if (strcmp(temp_st, TimeFormat()) != 0)
					{
						strcpy(temp_st, TimeFormat());
						if (DEBUG_PRINTF)
							printf("sound RUN !!tutk p2p data get (0x%x,0x%x,0x%x,0x%x,size=%d,SID=%d,%s)\n", Buffer_Data_Transfer[0], Buffer_Data_Transfer[1], Buffer_Data_Transfer[2], Buffer_Data_Transfer[3], pTargetChl->nRead, SID, TimeFormat());
					}
				}
				else
				{
					if (Debug_WebCam_PlayBack_AppVoice)
					{
						if ((Buffer_Data_Transfer[0] == 0x06) | (Buffer_Data_Transfer[0] == 0x07) | (Buffer_Data_Transfer[0] == 0x0a))
							if (DEBUG_PRINTF)
								printf("RUN !!(0x%02x,0x%02x)p2p data (size=%d) (SID=%d)get(%s)\n", Buffer_Data_Transfer[0], Buffer_Data_Transfer[3], Buffer_Transfer_Data_Size, SID, TimeFormat());
					}
					else
					{
						if (Debug_Tutk_Show56_Data)
						{
							if ((pTargetChl->Buffer_TUTK_data[0] == 5))
							{
								if (DEBUG_PRINTF)
									printf("RUN !!(0x%02x,0x%02x)p2p data (size=%d) (SID=%d)get(%s)\n", Buffer_Data_Transfer[0], Buffer_Data_Transfer[3], Buffer_Transfer_Data_Size, SID, TimeFormat());
							}
						}
						else
						{
							if (DEBUG_PRINTF)
								printf("RUN !!(0x%02x,0x%02x)p2p data (size=%d) (SID=%d)get(%s)\n", Buffer_Data_Transfer[0], Buffer_Data_Transfer[3], Buffer_Transfer_Data_Size, SID, TimeFormat());
						}
					}
					// printf("RUN !!(0x%02x,0x%02x)p2p data (size=%d) (SID=%d)get(%s)\n",Buffer_Data_Transfer[0],Buffer_Data_Transfer[3],Buffer_Transfer_Data_Size,SID,TimeFormat());
				}

				if ((pTargetChl->Buffer_TUTK_data[0] == 1) | (pTargetChl->Buffer_TUTK_data[0] == 2) | (pTargetChl->Buffer_TUTK_data[0] == 0xe1))
				{
					if (DEBUG_PRINTF)
						if (ShowIot_login_out_Data)
						{
							printf("\n(0x%02x,0x%02x)data(size:%d):\n", pTargetChl->Buffer_TUTK_data[0], pTargetChl->Buffer_TUTK_data[3], pTargetChl->nRead);
							for (j = 0; j < pTargetChl->nRead; j++)
							{
								printf("%02x,", pTargetChl->Buffer_TUTK_data[j]);
								if (((j + 1) % 16) == 0)
								{
									printf("\n");
								}
							}
							printf("\n\n");
						}

					pTargetChl->IOT_response[4] = 0x00;
					pTargetChl->IOT_response[5] = 0x00;
					memcpy(pTargetChl->IOT_response, pTargetChl->Buffer_TUTK_data, Buffer_Transfer_Data_Size);
					if (pTargetChl->Buffer_TUTK_data[0] == 1)
					{
						Flag_SID_Login_Run_IOT[SID] = true;
						pTargetChl->IOT_response[4] = 0x03;
						pTargetChl->IOT_response[5] = 0x00;
						pTargetChl->IOT_response[16] = 0x31;
						pTargetChl->IOT_response[17] = 0x31;
						pTargetChl->IOT_response[18] = 0x07;
						Buffer_Recive_Data_Size = 16 + pTargetChl->IOT_response[4] + pTargetChl->IOT_response[5] * 256;

						// printf("(%02d,%02d)T1 O1 data process(size=%d)(SID:%d)!!!!!\n",pTargetChl->IOT_response[0],pTargetChl->IOT_response[3],Buffer_Recive_Data_Size,SID);
					}
					else if (pTargetChl->Buffer_TUTK_data[0] == 2)
					{

						memcpy(pTargetChl->IOT_response, pTargetChl->Buffer_TUTK_data, Buffer_Transfer_Data_Size);
						Buffer_Recive_Data_Size = Buffer_Transfer_Data_Size;

						ret_webcam_logout_exit = write(socketDatainfo.conn, webcam_exit_data_send, 18);
						if (DEBUG_PRINTF)
							printf("(6,0)socket write before potocol logout!\n");
						usleep(10000);
						ret_webcam_logout_exit = read(socketDatainfo.conn, webcam_exit_data_recive, 20);
						if (DEBUG_PRINTF)
							printf("(6,0)socket read before potocol logout!\n");
						Logvedio_UDP_port_status = 0;
						Webcam_UDP_port_status = 0;
						usleep(10000);
						if (ret_webcam_logout_exit == -1 && errno == EAGAIN)
						{
							printf("ret_webcam_logout_exit=-1&&errno==EAGAIN (6,0)socket read (potocol logout)timeout ret_webcam_logout_exit=%d,sid=%d(send data %02x,%02x)\n", ret_webcam_logout_exit, SID, webcam_exit_data_send[0], webcam_exit_data_send[3]);
						}
						else if (ret_webcam_logout_exit < 0)
						{
							if (DEBUG_PRINTF)
								printf("ret_webcam_logout_exit < 0 (6,0)socket read (potocol logout)timeout ret_webcam_logout_exit=%d,sid=%d(send data %02x,%02x\n", ret_webcam_logout_exit, SID, webcam_exit_data_send[0], webcam_exit_data_send[3]);
							error_print("read");
						}
						Flag_SID_Login_Run_IOT[SID] = false;
						IOTC_Session_Close(SID);
						sprintf(write_sys_log_buf, "%s - aw.App_potocol_logout_SID=%d(%d)! \n", TimeFormat(), SID, pTargetChl->nRead);

						echo_file(write_sys_log_buf);

						// socketDatainfo.sys_backup_count++;
						// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf);  //SKS_FILE_PATH
						// sync();
						// system("cp /var/www/sks.log /var/www/sks.logtmp");
						// if(flash_write(FLASH_SKSLOG_START, 4096*2, "/var/www/sks.logtmp")){
						// 	if (DEBUG_PRINTF)
						// 		printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
						// }
						// else{
						// 	if (DEBUG_PRINTF)
						// 		printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
						// }
						// system("rm /var/www/sks.logtmp");
						// sync();
						app_potocal_logout = true;

						if (DEBUG_PRINTF)
							printf("%s\n", write_sys_log_buf);
						// break;
						break;
					}
					else if ((pTargetChl->Buffer_TUTK_data[0] == 0xe1) & (pTargetChl->Buffer_TUTK_data[3] == 0xf2))
					{ // HB RESTART "SksRst3579"

						if ((pTargetChl->Buffer_TUTK_data[16] == 0x53) & (pTargetChl->Buffer_TUTK_data[17] == 0x6b))
						{ // 0x53=>S 0x6b=>k
							if (DEBUG_PRINTF)
							{
								printf("\n(0x%02x,0x%02x)data(size:%d):\n", pTargetChl->Buffer_TUTK_data[0], pTargetChl->Buffer_TUTK_data[3], pTargetChl->nRead);
								for (j = 0; j < pTargetChl->nRead; j++)
								{
									printf("%02x,", pTargetChl->Buffer_TUTK_data[j]);
									if (((j + 1) % 16) == 0)
									{
										printf("\n");
									}
								}
							}
							memcpy(pTargetChl->IOT_response, pTargetChl->Buffer_TUTK_data, 16);

							// printf("\n\n");
							Flag_restart_HB = 1;
							pTargetChl->IOT_response[4] = 0x00;
							pTargetChl->IOT_response[5] = 0x00;
							Buffer_Recive_Data_Size = 16;
							sprintf(write_sys_log_buf, "%s - aw.App_potocol_reHB_SID=%d(%d)! \n", TimeFormat(), SID, pTargetChl->nRead);
							echo_file(write_sys_log_buf);
						}
					}
					else if ((pTargetChl->Buffer_TUTK_data[0] == 0xe1) & (pTargetChl->Buffer_TUTK_data[3] == 0xf1))
					{ // Reboot Gateway system "SksRst3579"

						if ((pTargetChl->Buffer_TUTK_data[16] == 0x53) & (pTargetChl->Buffer_TUTK_data[17] == 0x6b))
						{ // Sk
							if (DEBUG_PRINTF)
							{
								printf("\n(0x%02x,0x%02x)data(size:%d):\n", pTargetChl->Buffer_TUTK_data[0], pTargetChl->Buffer_TUTK_data[3], pTargetChl->nRead);
								for (j = 0; j < pTargetChl->nRead; j++)
								{
									printf("%02x,", pTargetChl->Buffer_TUTK_data[j]);
									if (((j + 1) % 16) == 0)
									{
										printf("\n");
									}
								}
							}

							IOTC_Session_Close(SID);
							sprintf(write_sys_log_buf, "%s - aw.App_potocol_reboot_SID=%d(%d)! \n", TimeFormat(), SID, pTargetChl->nRead);
							echo_file(write_sys_log_buf);

							usleep(100);
							// printf("\n\n");
							system(REBOOT_STRING);
						}
					}
					else if ((pTargetChl->Buffer_TUTK_data[0] == 0xe1) & (pTargetChl->Buffer_TUTK_data[3] == 0xf4))
					{ // Execute Command
						printf("%02x,", pTargetChl->Buffer_TUTK_data[16]);
						printf("%02x,", pTargetChl->Buffer_TUTK_data[17]);
						printf("\n(0x%02x,0x%02x)data(size:%d):\n", pTargetChl->Buffer_TUTK_data[0], pTargetChl->Buffer_TUTK_data[3], pTargetChl->nRead);
						memset(write_sys_log_buf2, 0, sizeof(write_sys_log_buf2));
						// int i;
						int BufferSize = pTargetChl->IOT_response[4] + pTargetChl->IOT_response[5] * 256;
						for (i = 0; i < BufferSize; i++)
						{
							write_sys_log_buf2[i] = pTargetChl->Buffer_TUTK_data[16 + i];
						}
						for (j = 0; j < pTargetChl->nRead; j++)
						{
							printf("%02x,", pTargetChl->Buffer_TUTK_data[j]);
							if (((j + 1) % 16) == 0)
							{
								printf("\n");
							}
						}
						printf("\n %s \n", write_sys_log_buf2);
						system(write_sys_log_buf2);
					}

					else if ((pTargetChl->Buffer_TUTK_data[0] == 0xe1) & (pTargetChl->Buffer_TUTK_data[3] == 0xf3))
					{ // log upload "SksRst3579"

						if ((pTargetChl->Buffer_TUTK_data[16] == 0x53) & (pTargetChl->Buffer_TUTK_data[17] == 0x6b))
						{ // Sk
							if (DEBUG_PRINTF)
							{
								printf("\n(0x%02x,0x%02x)data(size:%d):\n", pTargetChl->Buffer_TUTK_data[0], pTargetChl->Buffer_TUTK_data[3], pTargetChl->nRead);
								for (j = 0; j < pTargetChl->nRead; j++)
								{
									printf("%02x,", pTargetChl->Buffer_TUTK_data[j]);
									if (((j + 1) % 16) == 0)
									{
										printf("\n");
									}
								}
							}
							memcpy(pTargetChl->IOT_response, pTargetChl->Buffer_TUTK_data, 16);

							// printf("\n\n");
							Flag_upload_log = 1;

							pTargetChl->IOT_response[4] = 0x00;
							pTargetChl->IOT_response[5] = 0x00;
							Buffer_Recive_Data_Size = 16;
							sprintf(write_sys_log_buf, "%s - aw.App_upload_log_SID=%d(%d)! \n", TimeFormat(), SID, pTargetChl->nRead);
							echo_file(write_sys_log_buf);
							usleep(100);
							// printf("\n\n");
							// system(REBOOT_STRING);
						}
					}

					else if ((pTargetChl->Buffer_TUTK_data[0] == 0xe1) & (pTargetChl->Buffer_TUTK_data[3] == 0xfd))
					{ // ConfigUpload "Config3579"
						if ((pTargetChl->Buffer_TUTK_data[16] == 0x43) & (pTargetChl->Buffer_TUTK_data[17] == 0x6F))
						{
							if (DEBUG_PRINTF)
								printf("\n(0x%02x,0x%02x)data(size:%d):\n", pTargetChl->Buffer_TUTK_data[0], pTargetChl->Buffer_TUTK_data[3], pTargetChl->nRead);
							for (j = 0; j < pTargetChl->nRead; j++)
							{
								printf("%02x,", pTargetChl->Buffer_TUTK_data[j]);
								if (((j + 1) % 16) == 0)
								{
									printf("\n");
								}
							}
							printf("upload config data!\n");
							pTargetChl->IOT_response[4] = 0x00;
							pTargetChl->IOT_response[5] = 0x00;
							Buffer_Recive_Data_Size = 16;
							Flag_upload_config = 1;
						}
					}
					else if ((pTargetChl->Buffer_TUTK_data[0] == 0xe1) & (pTargetChl->Buffer_TUTK_data[3] == 0xfe))
					{ // ConfigDownload "Config3579"
						if ((pTargetChl->Buffer_TUTK_data[16] == 0x43) & (pTargetChl->Buffer_TUTK_data[17] == 0x6F))
						{
							if (DEBUG_PRINTF)
								printf("\n(0x%02x,0x%02x)data(size:%d):\n", pTargetChl->Buffer_TUTK_data[0], pTargetChl->Buffer_TUTK_data[3], pTargetChl->nRead);
							for (j = 0; j < pTargetChl->nRead; j++)
							{
								printf("%02x,", pTargetChl->Buffer_TUTK_data[j]);
								if (((j + 1) % 16) == 0)
								{
									printf("\n");
								}
							}
							printf("Download config data!\n");
							pTargetChl->IOT_response[4] = 0x00;
							pTargetChl->IOT_response[5] = 0x00;
							Buffer_Recive_Data_Size = 16;
							Flag_download_config = 1;
						}
					}

					else
					{
						memcpy(pTargetChl->IOT_response, pTargetChl->Buffer_TUTK_data, Buffer_Transfer_Data_Size);
						Buffer_Recive_Data_Size = Buffer_Transfer_Data_Size;
						// printf("(%02d,%02d)T1,T2 other data process(size=%d)(SID:%d)!!!!!\n",pTargetChl->Buffer_TUTK_data[0],pTargetChl->Buffer_TUTK_data[3],Buffer_Recive_Data_Size,SID);
					}
					//////////////////////

					if (pTargetChl->Buffer_TUTK_data[0] == 1)
					{
						ret = IOTC_Session_Write(SID, (const char *)pTargetChl->IOT_response, Buffer_Recive_Data_Size, ChID);
						if (DEBUG_PRINTF)
							if (ShowIot_login_out_Data)
							{
								printf("\n(0x%02x,0x%02x)data(size:%d):\n", pTargetChl->IOT_response[0], pTargetChl->IOT_response[3], Buffer_Recive_Data_Size);
								for (j = 0; j < Buffer_Recive_Data_Size; j++)
								{
									printf("%02x,", pTargetChl->IOT_response[j]);
									if (((j + 1) % 16) == 0)
									{
										printf("\n");
									}
								}
								printf("\n\n********\n");
							}

						// printf("(%02x,%02x) p2p write (size=%d)(SID:%d)(%s)!!!!!\n",pTargetChl->IOT_response[0],pTargetChl->IOT_response[3],Buffer_Recive_Data_Size,SID,TimeFormat());
						if (DEBUG_PRINTF)
							if (ShowDataMassage)
								printf("p2p send size=(%d)\n", Buffer_Recive_Data_Size);
						if (ret == IOTC_ER_CH_NOT_ON)
						{
							printf("IOTC_ER_CH_NOT_ON!!!!!!!!!!\n");
							IOTC_Session_Channel_ON(SID, ChID);
							IOTC_Session_Channel_ON(SID, 1);
						}
						else if (ret < 0)
						{ // other error < 0 means this session cant use anymore
							if (DEBUG_PRINTF)
								printf("SWLIO_IOTC_Session_Write error, code[%d]\n", ret);

							ret_webcam_logout_exit = write(socketDatainfo.conn, webcam_exit_data_send, 18);
							if (DEBUG_PRINTF)
								printf("(6,0)socket write before SWLIO_IOTC logout!\n");
							usleep(10000);
							ret_webcam_logout_exit = read(socketDatainfo.conn, webcam_exit_data_recive, 20);
							if (DEBUG_PRINTF)
								printf("(6,0)socket read before SWLIO_IOTC logout!\n");
							Logvedio_UDP_port_status = 0;
							Webcam_UDP_port_status = 0;
							usleep(10000);
							if (ret_webcam_logout_exit == -1 && errno == EAGAIN)
							{
								printf("ret_webcam_logout_exit=-1&&errno==EAGAIN (6,0)socket read (SWLIO_IOTC logout)timeout ret_webcam_logout_exit=%d,sid=%d(send data %02x,%02x)\n", ret_webcam_logout_exit, SID, webcam_exit_data_send[0], webcam_exit_data_send[3]);
							}
							else if (ret_webcam_logout_exit < 0)
							{
								printf("ret_webcam_logout_exit < 0 (6,0)socket read (SWLIO_IOTC logout)timeout ret_webcam_logout_exit=%d,sid=%d(send data %02x,%02x\n", ret_webcam_logout_exit, SID, webcam_exit_data_send[0], webcam_exit_data_send[3]);
								error_print("read");
							}
							Flag_SID_Login_Run_IOT[SID] = false;
							IOTC_Session_Close(SID);
							sprintf(write_sys_log_buf, "%s - aw.SWLIO_logout_SID(WLIO)=%d(%d)! \n", TimeFormat(), SID, ret);

							echo_file(write_sys_log_buf);

							// socketDatainfo.sys_backup_count++;
							// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf);  //SKS_FILE_PATH
							// sync();
							// system("cp /var/www/sks.log /var/www/sks.logtmp");
							// if(flash_write(FLASH_SKSLOG_START, 4096*2, "/var/www/sks.logtmp")){
							// 	if (DEBUG_PRINTF)
							// 		printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
							// }
							// else{
							// 	if (DEBUG_PRINTF)
							// 		printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
							// }
							// system("rm /var/www/sks.logtmp");
							// sync();
							pthread_exit(0);
						}
						else
						{
							pTargetChl->Flag_TUTK_read = 0;
							if (ShowDataMassage)
								printf("TUTK write over!!\n\n");
						}
						if (ShowDataMassage)
							printf("\n\n");
					}
					/////////////////////
				}
				else
				{
					// printf("other type,order\n");
					if ((pTargetChl->Buffer_TUTK_data[0] == 0x06) & (pTargetChl->Buffer_TUTK_data[3] == 0x01))
					{
						if (pTargetChl->Buffer_TUTK_data[17] == 0)
						{
							if (DEBUG_PRINTF)
								printf("webcam1 off!\n");
						}
						else
						{

							Flag_WebCam1SID[SID] = true;
							SID_WebCam_Mask[SID] = 0x01;
							Webcam_UDP_port_status = Webcam_UDP_port_status | 0x01;
							if (DEBUG_PRINTF)
								printf("\n\n****Webcam_UDP_port_status=%02x)))***\n\n", Webcam_UDP_port_status);
						}
						Buffer_Data_Transfer[13] = Webcam_UDP_port_status;
						if (DEBUG_PRINTF)
							printf("webcam1 socket udp on(SID:%d)[13,16,17](%02x,%02x,%02x)!\n", SID, Buffer_Data_Transfer[13], Buffer_Data_Transfer[16], Buffer_Data_Transfer[17]);
					}
					if ((pTargetChl->Buffer_TUTK_data[0] == 0x06) & (pTargetChl->Buffer_TUTK_data[3] == 0x02))
					{
						if (pTargetChl->Buffer_TUTK_data[17] == 0)
						{
							if (DEBUG_PRINTF)
								printf("webcam2 off!\n");
						}
						else
						{

							Flag_WebCam2SID[SID] = true;
							SID_WebCam_Mask[SID] = 0x02;

							Webcam_UDP_port_status = Webcam_UDP_port_status | 0x02;
							if (DEBUG_PRINTF)
								printf("\n\n02****Webcam_UDP_port_status=%02x)))***\n\n", Webcam_UDP_port_status);
							// WebCam2Qulity=Buffer_Data_Transfer[16];
						}
						Buffer_Data_Transfer[13] = Webcam_UDP_port_status;
						if (DEBUG_PRINTF)
							printf("webcam2 socket udp on(SID:%d)[13,16,17](%02x,%02x,%02x)!\n", SID, Buffer_Data_Transfer[13], Buffer_Data_Transfer[16], Buffer_Data_Transfer[17]);
					}

					if ((pTargetChl->Buffer_TUTK_data[0] == 0x06) & (pTargetChl->Buffer_TUTK_data[3] == 0x03))
					{
						if (pTargetChl->Buffer_TUTK_data[17] == 0)
						{
							if (DEBUG_PRINTF)
								printf("webcam3 off!\n");
						}
						else
						{

							Flag_WebCam3SID[SID] = true;
							SID_WebCam_Mask[SID] = 0x04;
							Webcam_UDP_port_status = Webcam_UDP_port_status | 0x04;
							if (DEBUG_PRINTF)
								printf("\n\n04****Webcam_UDP_port_status=%02x)))***\n\n", Webcam_UDP_port_status);
							// WebCam3Qulity=Buffer_Data_Transfer[16];
						}
						Buffer_Data_Transfer[13] = Webcam_UDP_port_status;
						if (DEBUG_PRINTF)
							printf("webcam3 socket udp on(SID:%d)[13,16,17](%02x,%02x,%02x)!\n", SID, Buffer_Data_Transfer[13], Buffer_Data_Transfer[16], Buffer_Data_Transfer[17]);
					}

					if ((pTargetChl->Buffer_TUTK_data[0] == 0x06) & (pTargetChl->Buffer_TUTK_data[3] == 0x04))
					{
						if (pTargetChl->Buffer_TUTK_data[17] == 0)
						{
							if (DEBUG_PRINTF)
								printf("webcam4 off!\n");
						}
						else
						{

							Flag_WebCam4SID[SID] = true;
							SID_WebCam_Mask[SID] = 0x08;
							Webcam_UDP_port_status = Webcam_UDP_port_status | 0x08;
							if (DEBUG_PRINTF)
								printf("\n\n08****Webcam_UDP_port_status=%02x)))***\n\n", Webcam_UDP_port_status);
							// WebCam4Qulity=Buffer_Data_Transfer[16];
						}
						Buffer_Data_Transfer[13] = Webcam_UDP_port_status;
						if (DEBUG_PRINTF)
							printf("webcam4 socket udp on(SID:%d)[13,16,17](%02x,%02x,%02x)!\n", SID, Buffer_Data_Transfer[13], Buffer_Data_Transfer[16], Buffer_Data_Transfer[17]);
					}

					if ((pTargetChl->Buffer_TUTK_data[0] == 0x06) & (pTargetChl->Buffer_TUTK_data[3] == 0x00))
					{
						if (DEBUG_PRINTF)
							printf("webcam1,2,3,4 off,SID:%d)[13,16,17](%02x,%02x,%02x)!\n", SID, Buffer_Data_Transfer[13], Buffer_Data_Transfer[16], Buffer_Data_Transfer[17]);

						Flag_WebCam1SID[SID] = false;
						Flag_WebCam2SID[SID] = false;
						Flag_WebCam3SID[SID] = false;
						Flag_WebCam4SID[SID] = false;

						if (SID_WebCam_Mask[SID] & 0x01)
						{
							Webcam_UDP_port_status = Webcam_UDP_port_status & (~SID_WebCam_Mask[SID]);
							SID_WebCam_Mask[SID] = 0;
							Buffer_Data_Transfer[13] = 1;
						}

						if (SID_WebCam_Mask[SID] & 0x02)
						{
							Webcam_UDP_port_status = Webcam_UDP_port_status & (~SID_WebCam_Mask[SID]);
							SID_WebCam_Mask[SID] = 0;
							Buffer_Data_Transfer[13] = 2;
						}

						if (SID_WebCam_Mask[SID] & 0x04)
						{
							Webcam_UDP_port_status = Webcam_UDP_port_status & (~SID_WebCam_Mask[SID]);
							SID_WebCam_Mask[SID] = 0;
							Buffer_Data_Transfer[13] = 3;
						}

						if (SID_WebCam_Mask[SID] & 0x08)
						{
							Webcam_UDP_port_status = Webcam_UDP_port_status & (~SID_WebCam_Mask[SID]);
							SID_WebCam_Mask[SID] = 0;
							Buffer_Data_Transfer[13] = 4;
						}
						if (DEBUG_PRINTF)
							printf("webcam1,2,3,4 off,SID:%d)[13,16,17](%02x,%02x,%02x)(port status=%02x)!\n", SID, Buffer_Data_Transfer[13], Buffer_Data_Transfer[16], Buffer_Data_Transfer[17], Webcam_UDP_port_status);
						// Webcam_UDP_port_status=0;
					}
					if ((pTargetChl->Buffer_TUTK_data[0] == 0x07) & (pTargetChl->Buffer_TUTK_data[3] == 0x01))
					{

						if ((pTargetChl->Buffer_TUTK_data[pTargetChl->nRead - 3] == 0x61) & (pTargetChl->Buffer_TUTK_data[pTargetChl->nRead - 2] == 0x76) & (pTargetChl->Buffer_TUTK_data[pTargetChl->nRead - 1] == 0x69))
						{
							if (DEBUG_PRINTF)
								printf("log AVI vedio file!\n");
							unsigned char temp_status = Webcam_UDP_port_status | Logvedio_UDP_port_status;
							unsigned char mark = 1;
							for (i = 0; i < 4; i++)
							{
								if (temp_status)
								{
									temp_status = temp_status >> 1;
								}
								else
								{
									break;
								}
							}
							Trans_LogVedio_mark[SID] = mark << i;

							Flag_Trans_LogVedio[SID] = true;
							Logvedio_UDP_port_status = Logvedio_UDP_port_status | Trans_LogVedio_mark[SID];
							Buffer_Data_Transfer[13] = Logvedio_UDP_port_status;
							if (DEBUG_PRINTF)
								printf("log vedio on,socket port (SID%d)(status=%02x,mark=%02x)!\n", SID, Logvedio_UDP_port_status, Trans_LogVedio_mark[SID]);
						}
						else
						{

							if (DEBUG_PRINTF)
								printf("log ipg pic file!\n");
						}
					}
					if ((pTargetChl->Buffer_TUTK_data[0] == 0x07) & (pTargetChl->Buffer_TUTK_data[3] == 0x00))
					{

						if (Logvedio_UDP_port_status)
						{
							for (i = 0; i < 4; i++)
							{
								if (Flag_Trans_LogVedio[i] == true)
								{
									Flag_Trans_LogVedio[i] = false;
									Logvedio_UDP_port_status = Logvedio_UDP_port_status & (~Trans_LogVedio_mark[i]);
									if (DEBUG_PRINTF)
										printf("log vedio off loop(SID=%d)(status=%02x,mark=%02x)!\n", SID, Logvedio_UDP_port_status, Trans_LogVedio_mark[i]);
									Trans_LogVedio_mark[i] = 0;
								}
							}

							Logvedio_UDP_port_status = 0;
							Buffer_Data_Transfer[13] = Logvedio_UDP_port_status;
							if (DEBUG_PRINTF)
								printf("log vedio off,socket (SID=%d)(status=%02x)!\n", SID, Logvedio_UDP_port_status);
						}
						else
						{
							if (DEBUG_PRINTF)
								printf("second (%x,%x) socket tcp skin and IOT response(SID=%d)\n", Buffer_Data_Transfer[0], Buffer_Data_Transfer[3], SID);
							flag_70_skin = true;
							pTargetChl->IOT_response[4] = 0x00;
							pTargetChl->IOT_response[5] = 0x00;
							Buffer_Recive_Data_Size = Buffer_Transfer_Data_Size;
							memcpy(pTargetChl->IOT_response, pTargetChl->Buffer_TUTK_data, Buffer_Transfer_Data_Size);
						}
					}

					if ((Buffer_Data_Transfer[0] == 0x0a))
					{
						int voice_mask = 0;
						for (i = 0; i < 4; i++)
						{
							if (Flag_WebCam1SID[SID] == true)
							{
								voice_mask = 0b00000001;
								// voice_mask=voice_mask<<i;
							}
							else if (Flag_WebCam2SID[i] == true)
							{
								voice_mask = 0b00000010;
								// voice_mask=voice_mask<<i;
							}
							else if (Flag_WebCam3SID[i] == true)
							{
								voice_mask = 0b00000100;
							}
							else if (Flag_WebCam4SID[i] == true)
							{
								voice_mask = 0b00001000;
							}
						}
						Buffer_Data_Transfer[13] = voice_mask;
					}
					k++;

					if (Buffer_Data_Transfer[0] == 0x0a)
					{
						if (strcmp(temp_st, TimeFormat()) != 0)
						{
							strcpy(temp_st, TimeFormat());
							if (DEBUG_PRINTF)
								printf("###(((((%d)sound socket (0x%x,0x%x,0x%x,0x%x,%s)\n", k, Buffer_Data_Transfer[0], Buffer_Data_Transfer[1], Buffer_Data_Transfer[2], Buffer_Data_Transfer[3], TimeFormat());
						}
					}

					if (flag_70_skin == false)
					{

						ret = write(socketDatainfo.conn, Buffer_Data_Transfer, Buffer_Transfer_Data_Size);

						usleep(10000);
					}
					// printf("(%02X,%02x) socket write(size=%d)\n",Buffer_Data_Transfer[0],Buffer_Data_Transfer[3],Buffer_Transfer_Data_Size);
					if ((Buffer_Data_Transfer[0] == 0x0a) | (flag_70_skin == true))
					{
						// usleep(100000);

						if (Buffer_Data_Transfer[0] == 0x0a)
						{
							if (strcmp(temp_st, TimeFormat()) != 0)
							{
								strcpy(temp_st, TimeFormat());
								if (DEBUG_PRINTF)
									printf("no socket response doesn't copy to iot(0x%x,0x%x,0x%x,0x%x,ret=%d,SID=%d)\n\n", Buffer_Data_Transfer[0], pTargetChl->Buffer_TUTK_data[1], pTargetChl->Buffer_TUTK_data[2], pTargetChl->Buffer_TUTK_data[3], ret, SID);
							}
						}
						else
						{
							if (DEBUG_PRINTF)
								printf("(%02X,%02x)size=%d) no socket response doesn't copy to iot(SID%d)\n\n", Buffer_Data_Transfer[0], Buffer_Data_Transfer[3], ret, SID);
						}

						flag_70_skin = false;
					}
					else
					{

						flag_transfer_data = true;
						while (flag_transfer_data)
						{
							// usleep(10);
							// printf("beforee zero Buffer_Data_Recive!\n");
							bzero(Buffer_Data_Recive, 1388);
							// printf("beforee readdd!\n");
							Buffer_Recive_Data_Size = read(socketDatainfo.conn, Buffer_Data_Recive, 1016);

							// printf("afterrr readdd!\n");
							if (Buffer_Recive_Data_Size == 1016)
							{
								flag_transfer_data = true;
							}
							else
							{
								flag_transfer_data = false;
							}
							// if ((Buffer_Data_Transfer[0]==6)|(Buffer_Data_Transfer[0]==7)|(Buffer_Data_Transfer[3]==7))
							if (flag_51_coooamd)
								if ((Buffer_Data_Recive[0] == 5) & (Buffer_Data_Recive[3] == 6))
								{
									sprintf(write_sys_log_buf, "%s - aw.SKS_remote_control_SID%d(%d)! \n", TimeFormat(), SID, ret);
									echo_file(write_sys_log_buf);
									// socketDatainfo.sys_backup_count++;
									// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf);  //SKS_FILE_PATH
									// sync();
									// system("cp /var/www/sks.log /var/www/sks.logtmp");
									// if(flash_write(FLASH_SKSLOG_START, 4096*2, "/var/www/sks.logtmp")){
									// 	if (DEBUG_PRINTF)
									// 		printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
									// }
									// else{
									// 	if (DEBUG_PRINTF)
									// 		printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
									// }
									// system("rm /var/www/sks.logtmp");
									// sync();
									if (DEBUG_PRINTF)
										if (Debuf_51command_data_show)
										{
											printf("\n (5,1)commmand set back!\n");
											for (l = 0; l < Buffer_Recive_Data_Size; l++)
											{
												printf("0x%02x,", Buffer_Data_Recive[l]);
												if (((l + 1) % 16) == 0)
													printf("\n");
											}
										}
									Buffer_Data_Recive[3] = 1;
									// step2
									//  Buffer_Data_Recive[10]=0x0c;
									//  Buffer_Data_Recive[11]=0x00;
									//  Buffer_Data_Recive[12]=0x06;
									// step3
									if (DEBUG_PRINTF)
										if (Debuf_51command_data_show)
										{
											for (l = 0; l < Buffer_Recive_Data_Size; l++)
											{
												printf("0x%02x,", Buffer_Data_Recive[l]);
												if (((l + 1) % 16) == 0)
													printf("\n");
											}
										}
									flag_51_coooamd = false;
								}

							// if ((Buffer_Data_Recive[0]==7)&(Buffer_Data_Recive[3]==1)){
							// 	Buffer_Data_Recive[4]=0x2a;
							// }

							if (Debug_WebCam_PlayBack_AppVoice)
							{
								if ((Buffer_Data_Recive[0] == 0x06) | (Buffer_Data_Recive[0] == 0x07) | (Buffer_Data_Recive[0] == 0x0a))
									if (DEBUG_PRINTF)
										printf("1socket read Buffer_Recive_Data_Size =%d(%02x,%02x)(%s)\n", Buffer_Recive_Data_Size, Buffer_Data_Recive[0], Buffer_Data_Recive[3], TimeFormat());

								// if (ShowIotDetialData){
								// 	printf("4All data(size=%03d):\n",Buffer_Recive_Data_Size);

								// 	if (((Buffer_Data_Recive[0]==0x03)&(Buffer_Data_Recive[3]==0x01))|((Buffer_Data_Recive[0]==0x03)&(Buffer_Data_Recive[3]==0x02))|((Buffer_Data_Recive[0]==0x03)&(Buffer_Data_Recive[3]==0x03))|((Buffer_Data_Recive[0]==0x03)&(Buffer_Data_Recive[3]==0x14))|((Buffer_Data_Recive[0]==0x03)&(Buffer_Data_Recive[3]==0x05))|((Buffer_Data_Recive[0]==0x03)&(Buffer_Data_Recive[3]==0x04))){
								// 		printf("(0x%02x,0x%02x)==>(size=%03d)\n\n",Buffer_Data_Recive[0],Buffer_Data_Recive[3],Buffer_Recive_Data_Size);
								// 	}
								// 	else{
								// 		for (j=0;j<Buffer_Recive_Data_Size;j++){
								// 			printf("%02x,",Buffer_Data_Recive[j]);
								// 			if (((j+1)%16)==0){
								// 				printf("\n");
								// 			}
								// 		}
								// 		printf("\n\n");
								// 	}
								// }
							}
							else
							{
								if (DEBUG_PRINTF)
								{
									if (Debug_Tutk_Show56_Data)
									{
										if ((pTargetChl->Buffer_TUTK_data[0] == 5))
										{
											printf("2socket read Buffer_Recive_Data_Size =%d(%02x,%02x)(%s)\n", Buffer_Recive_Data_Size, Buffer_Data_Recive[0], Buffer_Data_Recive[3], TimeFormat());
											if (ShowIotDetialData)
											{
												printf("5All data:\n");
												for (j = 0; j < Buffer_Recive_Data_Size; j++)
												{
													printf("%02x,", Buffer_Data_Recive[j]);
													if (((j + 1) % 16) == 0)
													{
														printf("\n");
													}
												}
												printf("\n\n");
											}
										}
									}
									else
									{
										printf("3socket read Buffer_Recive_Data_Size =%d(%02x,%02x)(%s)\n", Buffer_Recive_Data_Size, Buffer_Data_Recive[0], Buffer_Data_Recive[3], TimeFormat());
										if (Buffer_Recive_Data_Size != -1)
											if (ShowIotDetialData)
											{
												printf("1All data:\n");
												for (j = 0; j < Buffer_Recive_Data_Size; j++)
												{
													printf("%02x,", Buffer_Data_Recive[j]);
													if (((j + 1) % 16) == 0)
													{
														printf("\n");
													}
												}
												printf("\n\n");
											}
									}
								}
							}
							// printf("socket read Buffer_Recive_Data_Size =%d(%02x,%02x)(%s)\n",Buffer_Recive_Data_Size,Buffer_Data_Recive[0],Buffer_Data_Recive[3],TimeFormat());
							////////////////////////////////////////////////////////
							if (Buffer_Recive_Data_Size == -1 && errno == EAGAIN)
							{

								if (DEBUG_PRINTF)
								{
									printf("\nRecive timeout size info=%d,send size info=%d,sid=%d!\nhead[%d,%d]\n", Buffer_Recive_Data_Size, pTargetChl->nRead, SID, Buffer_Data_Transfer[0], Buffer_Data_Transfer[3]);
									printf("Iot data read===>\n");
									for (j = 0; j < pTargetChl->nRead; j++)
									{
										printf("%02x,", pTargetChl->Buffer_TUTK_data[j]);
										if (((j + 1) % 16) == 0)
										{
											printf("\n");
										}
									}
									printf("\n\n");
								}
								flag_transfer_data = false;
							}
							else if (Buffer_Recive_Data_Size < 0)
							{
								if (DEBUG_PRINTF)
									printf("%d timeout Buffer_Recive_Data_Size=%d,sid=%d\n", ++i, Buffer_Recive_Data_Size, SID);
								error_print("read");
								flag_transfer_data = false;
							}

							else
							{
								if ((Buffer_Data_Recive[0] == 0x03) & (Buffer_Data_Recive[3] == 0x07) & (Buffer_Recive_Data_Size == 17))
								{
									if (DEBUG_PRINTF)
										printf("(3,7) no data ,and don't send to app!");
								}
								else
								{
									memcpy(pTargetChl->IOT_response, Buffer_Data_Recive, Buffer_Recive_Data_Size);
									ret = IOTC_Session_Write(SID, (const char *)pTargetChl->IOT_response, Buffer_Recive_Data_Size, ChID);
									// if(ShowIot_read_write_data){
									// if (DEBUG_PRINTF)
									if (IOT_COMMAMD_AND_ITME)
									{
										// if (((pTargetChl->IOT_response[0]==3)&(pTargetChl->IOT_response[3]==3))|((pTargetChl->IOT_response[0]==5)&(pTargetChl->IOT_response[3]==6))){
										if ((pTargetChl->Buffer_TUTK_data[0] == 0x03) & (pTargetChl->Buffer_TUTK_data[3] == 0x14))
										{
											printf("\nAGENT TO APP write time===>%s)\n", TimeFormat());
											printf("(0x%02x,0x%02x) AGENT TO APP write size=>%d,SID=>%d,ChID=>%d\n", pTargetChl->IOT_response[0], pTargetChl->IOT_response[3], ret, SID, ChID);
											// if (Buffer_Recive_Data_Size>16){
											if (Buffer_Recive_Data_Size > 32)
											{
												data_size = 32;
											}
											else
											{
												data_size = Buffer_Recive_Data_Size;
											}
											for (i = 0; i < (data_size); i++)
											{
												printf("0x%02x,", pTargetChl->IOT_response[i]);
												if ((i + 1) == 16)
												{
													printf("\n");
												}
											}
											printf("\n");
											// }
										}
										// }
										// printf("Iot data write time===>%s\n",TimeFormat());
										// printf("(0x%02x,0x%02x) write size=>%d,SID=>%d,ChID=>%d\n",pTargetChl->IOT_response[0],pTargetChl->IOT_response[3],ret,SID,ChID);
									}
									// for(j=0;j<pTargetChl->nRead;j++){
									// 	printf("%02x,",pTargetChl->Buffer_TUTK_data[j]);
									// 	if (((j+1)%16)==0){
									// 		printf("\n");
									// 	}
									// }
									// printf("\n\n");
									// }

									// if ((Buffer_Data_Transfer[0]==6)|(Buffer_Data_Transfer[0]==7)|(Buffer_Data_Transfer[3]==7))
									if (Debug_WebCam_PlayBack_AppVoice)
									{
										if ((pTargetChl->Buffer_TUTK_data[0] == 0x06) | (pTargetChl->Buffer_TUTK_data[0] == 0x07) | (pTargetChl->Buffer_TUTK_data[0] == 0x0a))
											if (DEBUG_PRINTF)
												printf("(%02x,%02x)2 p2p write (size=%d)(SID=%d)(%s)!!!!!\n", pTargetChl->IOT_response[0], pTargetChl->IOT_response[3], ret, SID, TimeFormat());
									}
									else
									{
										if (Debug_Tutk_Show56_Data)
										{
											if ((pTargetChl->Buffer_TUTK_data[0] == 5))
											{
												if (DEBUG_PRINTF)
													printf("(%02x,%02x)3 p2p write (size=%d)(SID=%d)(%s)!!!!!\n", pTargetChl->IOT_response[0], pTargetChl->IOT_response[3], ret, SID, TimeFormat());
												// printf("All data:");
												// for (j=0;j<Buffer_Recive_Data_Size;j++){

												// 	printf("0x%02x,",Buffer_Data_Recive[j]);
												// }
												// printf("\n\n");
											}
										}
										else
										{
											if (DEBUG_PRINTF)
												printf("(%02x,%02x)4 p2p write (size=%d)(SID=%d)(%s)!!!!!\n", pTargetChl->IOT_response[0], pTargetChl->IOT_response[3], ret, SID, TimeFormat());
											// printf("4socket read Buffer_Recive_Data_Size =%d(%02x,%02x)(%s)\n",Buffer_Recive_Data_Size,Buffer_Data_Recive[0],Buffer_Data_Recive[3],TimeFormat());
										}
									}
									// printf("(%02x,%02x) p2p write (size=%d)(SID=%d)(%s)!!!!!\n",pTargetChl->IOT_response[0],pTargetChl->IOT_response[3],ret,SID,TimeFormat());
									if (ret == IOTC_ER_CH_NOT_ON)
									{
										if (DEBUG_PRINTF)
											printf("IOTC_ER_CH_NOT_ON!!!!!!!!!!\n");
										IOTC_Session_Channel_ON(SID, ChID);
										IOTC_Session_Channel_ON(SID, 1);
									}
									else if (ret < 0)
									{ // other error < 0 means this session cant use anymore
										if (DEBUG_PRINTF)
											printf("normal_IOTC_Session_Write error, code[%d]\n", ret);

										ret_webcam_logout_exit = write(socketDatainfo.conn, webcam_exit_data_send, 18);
										if (DEBUG_PRINTF)
											printf("(6,0)socket write before normal logout!\n");
										usleep(10000);
										ret_webcam_logout_exit = read(socketDatainfo.conn, webcam_exit_data_recive, 20);
										if (DEBUG_PRINTF)
											printf("(6,0)socket read before normal logout!\n");
										Logvedio_UDP_port_status = 0;
										Webcam_UDP_port_status = 0;
										usleep(10000);
										if (ret_webcam_logout_exit == -1 && errno == EAGAIN)
										{
											if (DEBUG_PRINTF)
												printf("ret_webcam_logout_exit=-1&&errno==EAGAIN (6,0)socket read (normal logout)timeout ret_webcam_logout_exit=%d,sid=%d(send data %02x,%02x)\n", ret_webcam_logout_exit, SID, webcam_exit_data_send[0], webcam_exit_data_send[3]);
										}
										else if (ret_webcam_logout_exit < 0)
										{
											if (DEBUG_PRINTF)
												printf("ret_webcam_logout_exit < 0 (6,0)socket read (normal logout)timeout ret_webcam_logout_exit=%d,sid=%d(send data %02x,%02x\n", ret_webcam_logout_exit, SID, webcam_exit_data_send[0], webcam_exit_data_send[3]);
											error_print("read");
										}

										Flag_SID_Login_Run_IOT[SID] = false;
										IOTC_Session_Close(SID);
										sprintf(write_sys_log_buf, "%s - aw.SN_Write_fail_SID(WR)=%d(%d)! \n", TimeFormat(), SID, ret);
										echo_file(write_sys_log_buf);
										// socketDatainfo.sys_backup_count++;
										// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf);  //SKS_FILE_PATH
										// sync();
										// system("cp /var/www/sks.log /var/www/sks.logtmp");
										// if(flash_write(FLASH_SKSLOG_START, 4096*2, "/var/www/sks.logtmp")){
										// 	if (DEBUG_PRINTF)
										// 		printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
										// }
										// else{
										// 	if (DEBUG_PRINTF)
										// 		printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
										// }
										// system("rm /var/www/sks.logtmp");
										// sync();
										if (DEBUG_PRINTF)
											printf("%s_SN_Write_fail sleep 50 nsec exit thread\n", write_sys_log_buf);
										usleep(50);
										pthread_exit(0);
									}
									else
									{
										pTargetChl->Flag_TUTK_read = 0;

										if (ShowDataMassage)
											printf("TUTK write over!!\n\n");
									}
								}
								if (Debug_WebCam_PlayBack_AppVoice)
								{
									if ((pTargetChl->Buffer_TUTK_data[0] == 0x06) | (pTargetChl->Buffer_TUTK_data[0] == 0x07) | (pTargetChl->Buffer_TUTK_data[0] == 0x0a))
										printf("\n\n");
								}
								else
									printf("\n\n");
							}
						}
					}
				}
				data_run = false;
			}
		}
		if (app_potocal_logout)
		{
			if (DEBUG_PRINTF)
				printf("\nApp_potocol_logout sleep 50 usec!\n");
			usleep(50);
			break;
		}
		if (pTargetChl->nRead == IOTC_ER_SESSION_CLOSE_BY_REMOTE)
		{
			if (DEBUG_PRINTF)
				printf("[thread_ForSessionRead] (CID:%d) remote site close this session, (SID:%d)(ret=%d)\n", ChID, SID, pTargetChl->nRead);

			ret_webcam_logout_exit = write(socketDatainfo.conn, webcam_exit_data_send, 18);
			if (DEBUG_PRINTF)
				printf("AIl(6,0)socket write before APP IOT logout!\n");
			usleep(10000);
			ret_webcam_logout_exit = read(socketDatainfo.conn, webcam_exit_data_recive, 20);
			if (DEBUG_PRINTF)
				printf("AIl(6,0)socket read before APP IOT logout!\n");
			Logvedio_UDP_port_status = 0;
			Webcam_UDP_port_status = 0;
			usleep(10000);
			if (ret_webcam_logout_exit == -1 && errno == EAGAIN)
			{
				if (DEBUG_PRINTF)
					printf("ret_webcam_logout_exit=-1&&errno==EAGAIN (6,0)socket read (APP IOT logout)timeout ret_webcam_logout_exit=%d,sid=%d(send data %02x,%02x)\n", ret_webcam_logout_exit, SID, webcam_exit_data_send[0], webcam_exit_data_send[3]);
			}
			else if (ret_webcam_logout_exit < 0)
			{
				if (DEBUG_PRINTF)
					printf("ret_webcam_logout_exit < 0 (6,0)socket read (APP IOT logout)timeout ret_webcam_logout_exit=%d,sid=%d(send data %02x,%02x\n", ret_webcam_logout_exit, SID, webcam_exit_data_send[0], webcam_exit_data_send[3]);
				error_print("read");
			}
			Flag_SID_Login_Run_IOT[SID] = false;
			IOTC_Session_Close(SID);
			sprintf(write_sys_log_buf, "%s - aw.App_IOT_logout_SID=%d(%d)! \n", TimeFormat(), SID, pTargetChl->nRead);
			echo_file(write_sys_log_buf);

			// socketDatainfo.sys_backup_count++;
			// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf);  //SKS_FILE_PATH
			// sync();
			// system("cp /var/www/sks.log /var/www/sks.logtmp");
			// if(flash_write(FLASH_SKSLOG_START, 4096*2, "/var/www/sks.logtmp")){
			// 	if (DEBUG_PRINTF)
			// 		printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
			// }
			// else{
			// 	if (DEBUG_PRINTF)
			// 		printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
			// }
			// system("rm /var/www/sks.logtmp");
			// sync();
			if (DEBUG_PRINTF)
				printf("\n%slogout sleep 50 usec!\n", write_sys_log_buf);
			usleep(50);

			break;
		}
		else if (pTargetChl->nRead == IOTC_ER_REMOTE_TIMEOUT_DISCONNECT)
		{
			if (DEBUG_PRINTF)
				printf("[thread_ForSessionRead] (CID:%d) disconnected due to remote site has no any response after a 'timeout' period of time., (SID:%d)(ret=%d)\n", ChID, SID, pTargetChl->nRead);

			ret_webcam_logout_exit = write(socketDatainfo.conn, webcam_exit_data_send, 18);
			if (DEBUG_PRINTF)
				printf("AIt(6,0)socket write before APP IOT timeout logout!\n");
			usleep(10000);
			ret_webcam_logout_exit = read(socketDatainfo.conn, webcam_exit_data_recive, 20);
			if (DEBUG_PRINTF)
				printf("AIt(6,0)socket read before APP IOT timeout logout!\n");
			Logvedio_UDP_port_status = 0;
			Webcam_UDP_port_status = 0;
			usleep(10000);
			if (ret_webcam_logout_exit == -1 && errno == EAGAIN)
			{
				if (DEBUG_PRINTF)
					printf("ret_webcam_logout_exit=-1&&errno==EAGAIN (6,0)socket read (APP IOT timeout logout)timeout ret_webcam_logout_exit=%d,sid=%d(send data %02x,%02x)\n", ret_webcam_logout_exit, SID, webcam_exit_data_send[0], webcam_exit_data_send[3]);
			}
			else if (ret_webcam_logout_exit < 0)
			{
				if (DEBUG_PRINTF)
					printf("ret_webcam_logout_exit < 0 (6,0)socket read (APP IOT timeout logout)timeout ret_webcam_logout_exit=%d,sid=%d(send data %02x,%02x\n", ret_webcam_logout_exit, SID, webcam_exit_data_send[0], webcam_exit_data_send[3]);
				error_print("read");
			}
			Flag_SID_Login_Run_IOT[SID] = false;
			IOTC_Session_Close(SID);

			sprintf(write_sys_log_buf, "%s - aw.Response_Timeout_Exit_SID=%d(%d)! \n", TimeFormat(), SID, pTargetChl->nRead);
			echo_file(write_sys_log_buf);
			// socketDatainfo.sys_backup_count++;
			// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf);  //SKS_FILE_PATH
			// sync();
			// system("cp /var/www/sks.log /var/www/sks.logtmp");
			// if(flash_write(FLASH_SKSLOG_START, 4096*2, "/var/www/sks.logtmp")){
			// 	if (DEBUG_PRINTF)
			// 		printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
			// }
			// else{
			// 	if (DEBUG_PRINTF)
			// 		printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
			// }
			// system("rm /var/www/sks.logtmp");
			// sync();
			CountMaxSession = 0;
			if (DEBUG_PRINTF)
				printf("\nResponse_Timeout_Exit sleep 50 usec!\n");
			usleep(50);

			break;
		}
		else if (pTargetChl->nRead == IOTC_ER_TIMEOUT)
		{
			if (DEBUG_PRINTF)
				printf("[thread_ForSessionRead] (CID:%d) IOTC_ER_TIMEOUT(SID:%d_%d)\n", ChID, SID, pTargetChl->nRead);
			// printf("read over!\n\n");
			// printf("[thread_ForSessionReadWrite] %d, nRead = %d, SID = %d\n", ChID, nRead, SID);
			// IOTC_Session_Close(SID);
			ret_webcam_logout_exit = write(socketDatainfo.conn, webcam_exit_data_send, 18);
			if (DEBUG_PRINTF)
				printf("IER(6,0)socket write before IOTC_ER_TIMEOUT logout!\n");
			usleep(10000);
			ret_webcam_logout_exit = read(socketDatainfo.conn, webcam_exit_data_recive, 20);
			if (DEBUG_PRINTF)
				printf("IER(6,0)socket read before IOTC_ER_TIMEOUT logout!\n");
			Logvedio_UDP_port_status = 0;
			Webcam_UDP_port_status = 0;
			usleep(10000);
			if (ret_webcam_logout_exit == -1 && errno == EAGAIN)
			{
				if (DEBUG_PRINTF)
					printf("ret_webcam_logout_exit=-1&&errno==EAGAIN (6,0)socket read (IOTC_ER_TIMEOUT)timeout ret_webcam_logout_exit=%d,sid=%d(send data %02x,%02x)\n", ret_webcam_logout_exit, SID, webcam_exit_data_send[0], webcam_exit_data_send[3]);
			}
			else if (ret_webcam_logout_exit < 0)
			{
				if (DEBUG_PRINTF)
					printf("ret_webcam_logout_exit < 0 (6,0)socket read (IOTC_ER_TIMEOUT)timeout ret_webcam_logout_exit=%d,sid=%d(send data %02x,%02x\n", ret_webcam_logout_exit, SID, webcam_exit_data_send[0], webcam_exit_data_send[3]);
				error_print("read");
			}
			Flag_SID_Login_Run_IOT[SID] = false;
			IOTC_Session_Close(SID);
			if (DEBUG_PRINTF)
				printf("IOTC_Session_Channel_ON SID=%d,CID=%d\n", SID, ChID);
			sprintf(write_sys_log_buf, "%s - aw.IOTC_ER_TIMEOUT_logout(SID=%d_%d)! \n", TimeFormat(), SID, pTargetChl->nRead);
			echo_file(write_sys_log_buf);
			// socketDatainfo.sys_backup_count++;
			// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf);  //SKS_FILE_PATH
			// sync();
			// system("cp /var/www/sks.log /var/www/sks.logtmp");
			// if(flash_write(FLASH_SKSLOG_START, 4096*2, "/var/www/sks.logtmp")){
			// 	if (DEBUG_PRINTF)
			// 		printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
			// }
			// else{
			// 	if (DEBUG_PRINTF)
			// 		printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
			// }
			// system("rm /var/www/sks.logtmp");
			// sync();
			CountMaxSession = 0;
			if (DEBUG_PRINTF)
				printf("\n%s sleep 50 usec!\n", write_sys_log_buf);
			usleep(50);
			break;
		}
		else
		{
			if (DEBUG_PRINTF)
				printf("[thread_ForSessionRead] (CID:%d) other issue_(SID:%d,%d)\n", ChID, SID, pTargetChl->nRead);
			// printf("read over!\n\n");
			// printf("[thread_ForSessionReadWrite] %d, nRead = %d, SID = %d\n", ChID, nRead, SID);

			ret_webcam_logout_exit = write(socketDatainfo.conn, webcam_exit_data_send, 18);
			if (DEBUG_PRINTF)
				printf("boi(6,0)socket write before other issue logout!\n");
			usleep(10000);
			ret_webcam_logout_exit = read(socketDatainfo.conn, webcam_exit_data_recive, 20);
			if (DEBUG_PRINTF)
				printf("boi(6,0)socket read before other issue logout!\n");
			Logvedio_UDP_port_status = 0;
			Webcam_UDP_port_status = 0;
			usleep(10000);
			if (ret_webcam_logout_exit == -1 && errno == EAGAIN)
			{
				if (DEBUG_PRINTF)
					printf("ret_webcam_logout_exit=-1&&errno==EAGAIN (6,0)socket read (other issue logout)timeout ret_webcam_logout_exit=%d,sid=%d(send data %02x,%02x)\n", ret_webcam_logout_exit, SID, webcam_exit_data_send[0], webcam_exit_data_send[3]);
			}
			else if (ret_webcam_logout_exit < 0)
			{
				if (DEBUG_PRINTF)
					printf("ret_webcam_logout_exit < 0 (6,0)socket read (other issue logout)timeout ret_webcam_logout_exit=%d,sid=%d(send data %02x,%02x\n", ret_webcam_logout_exit, SID, webcam_exit_data_send[0], webcam_exit_data_send[3]);
				error_print("read");
			}
			Flag_SID_Login_Run_IOT[SID] = false;
			IOTC_Session_Close(SID);

			sprintf(write_sys_log_buf, "%s - aw.Other_Issuse(SID=%d_%d)! \n", TimeFormat(), SID, pTargetChl->nRead);

			printf("write_sys_log_buf==>(%s)\n", write_sys_log_buf);
			echo_file(write_sys_log_buf);
			// socketDatainfo.sys_backup_count++;
			// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf);  //SKS_FILE_PATH
			// sync();
			// system("cp /var/www/sks.log /var/www/sks.logtmp");
			// if(flash_write(FLASH_SKSLOG_START, 4096*2, "/var/www/sks.logtmp")){
			// 	if (DEBUG_PRINTF)
			// 		printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
			// }
			// else{
			// 	if (DEBUG_PRINTF)
			// 		printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
			// }
			// system("rm /var/www/sks.logtmp");
			// sync();
			if (DEBUG_PRINTF)
				printf("\nOther_Issuse logout sleep 50 usec!\n\n");
			usleep(50);
			break;
		}
		gettimeofday(&tv_now, &tz_now);
		printf("tv_now_sec:%ld\n", tv_now.tv_sec);
		// printf("tv_now_usec:%ld\n",tv_now.tv_usec);

		if ((tv_now.tv_sec - tv_set.tv_sec) > SESSION_CHANNEL_IDLE_SECOND)
		{
			if (DEBUG_PRINTF)
				printf("Idel over %d sec\n", SESSION_CHANNEL_IDLE_SECOND);
			ret_webcam_logout_exit = write(socketDatainfo.conn, webcam_exit_data_send, 18);
			if (DEBUG_PRINTF)
				printf("Iol(6,0)socket write before Idel over logout!\n");
			usleep(10000);
			ret_webcam_logout_exit = read(socketDatainfo.conn, webcam_exit_data_recive, 20);
			if (DEBUG_PRINTF)
				printf("Iol(6,0)socket read before Idel over logout!\n");
			Logvedio_UDP_port_status = 0;
			Webcam_UDP_port_status = 0;
			usleep(10000);
			if (ret_webcam_logout_exit == -1 && errno == EAGAIN)
			{
				if (DEBUG_PRINTF)
					printf("ret_webcam_logout_exit=-1&&errno==EAGAIN (6,0)socket read (Idel over logout)timeout ret_webcam_logout_exit=%d,sid=%d(send data %02x,%02x)\n", ret_webcam_logout_exit, SID, webcam_exit_data_send[0], webcam_exit_data_send[3]);
			}
			else if (ret_webcam_logout_exit < 0)
			{
				if (DEBUG_PRINTF)
					printf("ret_webcam_logout_exit < 0 (6,0)socket read (Idel over logout)timeout ret_webcam_logout_exit=%d,sid=%d(send data %02x,%02x\n", ret_webcam_logout_exit, SID, webcam_exit_data_send[0], webcam_exit_data_send[3]);
				error_print("read");
			}
			Flag_SID_Login_Run_IOT[SID] = false;
			IOTC_Session_Close(SID);
			sprintf(write_sys_log_buf, "%s - aw.Session_idel_over_%dsec_Exit_SID=%d(%d)! \n", TimeFormat(), SESSION_CHANNEL_IDLE_SECOND, SID, pTargetChl->nRead);
			echo_file(write_sys_log_buf);
			// socketDatainfo.sys_backup_count++;
			// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf);  //SKS_FILE_PATH
			// sync();
			// system("cp /var/www/sks.log /var/www/sks.logtmp");
			// if(flash_write(FLASH_SKSLOG_START, 4096*2, "/var/www/sks.logtmp")){
			// 	if (DEBUG_PRINTF)
			// 		printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
			// }
			// else{
			// 	if (DEBUG_PRINTF)
			// 		printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
			// }
			// system("rm /var/www/sks.logtmp");
			// sync();

			CountMaxSession = 0;

			if (Flag_WebCam1SID[SID])
			{
				Webcam_UDP_port_status = (Webcam_UDP_port_status & (~SID_WebCam_Mask[SID]));
				SID_WebCam_Mask[SID] = 0;
			}
			if (Flag_WebCam2SID[SID])
			{
				Webcam_UDP_port_status = (Webcam_UDP_port_status & (~SID_WebCam_Mask[SID]));
				SID_WebCam_Mask[SID] = 0;
			}
			if (Flag_WebCam3SID[SID])
			{
				Webcam_UDP_port_status = (Webcam_UDP_port_status & (~SID_WebCam_Mask[SID]));
				SID_WebCam_Mask[SID] = 0;
			}
			if (Flag_WebCam4SID[SID])
			{
				Webcam_UDP_port_status = (Webcam_UDP_port_status & (~SID_WebCam_Mask[SID]));
				SID_WebCam_Mask[SID] = 0;
			}
			if (DEBUG_PRINTF)
				printf("\nSession_idel_over 30 Sec exit sleep 100 usec!\n\n");
			usleep(50);
			break;
		}
	}
	if (DEBUG_PRINTF)
		printf("[thread_ForSessionRead](SID:%d) Thread exit...[CID:%d]\n\n\n", SID, ChID);
	free(pTargetChl);
	pthread_exit(0);
}
DeviceLoginInput auth_option;
int use_key = 0;
void *_thread_Login(void *arg)
{
	int ret = 0;
	char *UID = (char *)arg;

	while (true)
	{
		if (DEBUG_PRINTF)
			printf("key =%d\n", use_key); ////
		auth_option.cb = sizeof(DeviceLoginInput);
		auth_option.authentication_type = AUTHENTICATE_BY_KEY;
		// Authkey是一組最多支援8位的英數字
		memset(auth_option.auth_key, '0', 8);
		////
		if (use_key)
		{												  ////
			ret = IOTC_Device_LoginEx(UID, &auth_option); ////
			if (DEBUG_PRINTF)
				printf("Calling IOTC_Device_LoginEX() ret = %d\n", ret); ////
		} ////
		else
		{
			TUTK_SDK_Set_Region(REGION_ASIA);
			ret = IOTC_Device_Login(UID, NULL, NULL);
			if (DEBUG_PRINTF)
				printf("Calling IOTC_Device_Login() ret = %d\n", ret);
			// pthread_exit(0);   ///////
		}
		if (ret == IOTC_ER_NoERROR)
		{
			if (DEBUG_PRINTF)
				printf("*******Exit IOTC_Device_Login(),NoERROR ret = %d\n", ret);
			break;
		}
		else
		{
			sleep(1);
			system("ntpclient -s -h pool.ntp.org -i 5");
		}
	}
	pthread_exit(0);
}

time_t *UtcTime()
{ // 時間處理程式
	static time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep); /*取得當地時間*/
	if (DEBUG_PRINTF)
		printf("timep=time() : %ld \n", timep);
	return &timep;
}
// int WriteToDayRunFile(char *NewEvent, char *filename)
// {

// 	FILE *fp_WriteToDayRunFile;
// 	// char dayname_WriteToDayRunFile[50];
// 	// //WriteToDayRunFile()

//     // memset(dayname_WriteToDayRunFile,0,50);
// 	// sprintf(dayname_WriteToDayRunFile,"%s",DAYRUN_FILE);
// 	// strcat(dayname_WriteToDayRunFile,"DayRun");
// 	if (DEBUG_PRINTF)
// 		if (Debug_file_data_show)
// 			printf("write (%s) to %s file \n\n",NewEvent,filename);

// 	fp_WriteToDayRunFile = fopen(filename, "a");
// 	fwrite(NewEvent,strlen(NewEvent),1,fp_WriteToDayRunFile);
// 	fclose(fp_WriteToDayRunFile);
// 	sync();
//     return 0;
// }
// msdk-linux-gcc -I /home/kk/8198c_toolchain/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/include -g -Wall -O2 IOTCAPIs_Device2.c libapmib.so cJSON.c -lIOTCAPIs -lNebulaAPIs -lTUTKGlobalAPIs -lm -lcurl -lpthread -L /home/kk/8198c_toolchain/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/lib -Wl,-rpath=/home/kk/8198c_toolchain/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/lib -lpthread -o IOTCAPIs_Device2_8198c_h
// msdk-linux-gcc -I /home/kk/8198c_toolchain/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/include -g -Wall -O2 IOTCAPIs_Device2.c libapmib.so cJSON.c -lIOTCAPIs -lNebulaAPIs -lTUTKGlobalAPIs -lm -lcurl -lpthread -L /home/kk/8198c_toolchain/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/lib -Wl,-rpath=/home/kk/8198c_toolchain/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/lib -lpthread -o IOTCAPIs_Device2_8198c_h
// msdk-linux-gcc -I /home/kk/8198c_toolchain/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/include -g -Wall -O2 IOTCAPIs_Device2.c libapmib.so cJSON.c -lIOTCAPIsT -lNebulaAPIsT -lTUTKGlobalAPIsT -lm -lcurl -lpthread -L /home/kk/8198c_toolchain/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/lib -Wl,-rpath=/home/kk/8198c_toolchain/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/lib -lpthread -o IOTCAPIs_Device2_8198c_h  記錄LOG
int main(int argc, char *argv[])
{

	int ret;
	const unsigned char ChID0 = 0; //, ChID1 = 1;
	pthread_t threadID_Login;
	char time_temp[30];
	// *********************************
	Apmib *Ampib_data = (Apmib *)malloc(sizeof(Apmib));
	char *UID;
	time_t *t;
	char write_sys_log_buf[128];
	char buf[256];
	int k = 0;
	// char power_on_data[256];
	Flag_restart_HB = 0;
	if (THAI_ADDR_ENABLE)
	{
		sprintf(FIRMWARE_VER2, "%s%s", NUM_VER, THAI);
		if (DEBUG_PRINTF)
			printf("VER==>%s", FIRMWARE_VER2);
	}
	else
	{
		sprintf(FIRMWARE_VER2, "%s%s", NUM_VER, SKS);
		if (DEBUG_PRINTF)
			printf("TVER==>%s", FIRMWARE_VER2);
	}
	if (DEBUG_PRINTF)
		printf("\n\n////IOTCAPI%s,%s/////\n\n", DATE_TIME, FIRMWARE_VER2);

	sprintf(buf, "%s - aw.Iotcapi_on_%s_%s! \n", TimeFormat(), DATE_TIME, FIRMWARE_VER2);
	if (DEBUG_PRINTF)
		printf("%s", buf);
	echo_file(buf);

	CountMaxSession = 0; // init
	if (argc < 2)
	{
// printf("No UID input!!!\n");
#ifndef DEBUG

		if (DEFAULT_APMIB)
		{
			strcpy(Ampib_data->apmib_gw, DEFAULT_GW);
			strcpy(Ampib_data->apmib_mno, DEFAULT_MNO);
			strcpy(Ampib_data->apmib_UID, DEFAULT_UID);
			UID = (char *)Ampib_data->apmib_UID;
			printf("UID(read from parm.) = %s\n", Ampib_data->apmib_UID);
		}
		else
		{
			apmib_init();
			apmib_get(MIB_USER_PASSWORD, (void *)Ampib_data->apmib_gw);
			apmib_get(MIB_SNMP_RW_COMMUNITY, (void *)Ampib_data->apmib_mno);
			apmib_get(MIB_PPP_PASSWORD4, (void *)Ampib_data->apmib_UID);
			UID = (char *)Ampib_data->apmib_UID;
			if (DEBUG_PRINTF)
				printf("UID(read from board) = %s\n", Ampib_data->apmib_UID);
		}
#endif
	}
	else
	{
		strcpy(Ampib_data->apmib_gw, DEFAULT_GW);
		strcpy(Ampib_data->apmib_mno, DEFAULT_MNO);
		UID = (char *)argv[1];
		strcpy(Ampib_data->apmib_UID, UID);
		// printf("else UID = %s\n",UID);
	}

	ret = TUTK_SDK_Set_License_Key(LICENSE_KEY);

	LogAttr logAttr;
	memset(&logAttr, 0, sizeof(logAttr));
	logAttr.log_level = LEVEL_VERBOSE;
	logAttr.path = "./<your path>/log_global.txt";
	TUTK_Set_Log_Attr(logAttr);
	logAttr.path = "./<your path>/log_iotc.txt";
	IOTC_Set_Log_Attr(logAttr);

	if (ret != TUTK_ER_NoERROR)
	{
		if (DEBUG_PRINTF)
			printf("TUTK_SDK_Set_License_Key()=%d ,exit...!!\n", ret);
		return 0;
	}
	// // Must be first call before IOTC_Initialize2(), but not a must
	IOTC_Set_Max_Session_Number(MAX_SESSION);
	if (DEBUG_PRINTF)
		printf("IOTCAPIs_Device start...\n");
	ret = IOTC_Initialize2(0);
	if (DEBUG_PRINTF)
		printf("Step 1: IOTC_Initialize2() ret = %d\n", ret);
	if (ret != IOTC_ER_NoERROR)
	{
		if (DEBUG_PRINTF)
			printf("IOTC_Initialize2 error, IOTCAPIs_Device exit...!!\n");
		return 0;
	}

	auth_option.cb = sizeof(DeviceLoginInput);
	auth_option.authentication_type = AUTHENTICATE_BY_KEY;
	memset(auth_option.auth_key, '0', 8);

	WebApiData *WebApiinfo = (WebApiData *)malloc(sizeof(WebApiData));
	strcpy(WebApiinfo->apmib_gw, Ampib_data->apmib_gw);
	strcpy(WebApiinfo->apmib_mno, Ampib_data->apmib_mno);
	strcpy(WebApiinfo->apmib_UID, Ampib_data->apmib_UID);
	strcpy(socketDatainfo.apmib_gw, Ampib_data->apmib_gw);
	strcpy(socketDatainfo.apmib_mno, Ampib_data->apmib_mno);
	strcpy(socketDatainfo.apmib_UID, Ampib_data->apmib_UID);
	socketDatainfo.port = 1235;

	socketDatainfo.hb_restart_sec = atoi(Ampib_data->apmib_mno) % HB_RESTART_MOD_SEC;
	socketDatainfo.upload_sec = atoi(Ampib_data->apmib_mno) % (2 * 60 * 60);

	SocketData *socketWebcam1Data = (SocketData *)malloc(sizeof(SocketData));
	strcpy(socketWebcam1Data->apmib_gw, Ampib_data->apmib_gw);
	strcpy(socketWebcam1Data->apmib_mno, Ampib_data->apmib_mno);
	strcpy(socketWebcam1Data->apmib_UID, Ampib_data->apmib_UID);
	socketWebcam1Data->port = 1236;
	socketWebcam1Data->flag_run_once = true;

	SocketData *socketWebcam2Data = (SocketData *)malloc(sizeof(SocketData));
	strcpy(socketWebcam2Data->apmib_gw, Ampib_data->apmib_gw);
	strcpy(socketWebcam2Data->apmib_mno, Ampib_data->apmib_mno);
	strcpy(socketWebcam2Data->apmib_UID, Ampib_data->apmib_UID);
	socketWebcam2Data->port = 1237;
	socketWebcam2Data->flag_run_once = true;

	SocketData *socketWebcam3Data = (SocketData *)malloc(sizeof(SocketData));
	strcpy(socketWebcam3Data->apmib_gw, Ampib_data->apmib_gw);
	strcpy(socketWebcam3Data->apmib_mno, Ampib_data->apmib_mno);
	strcpy(socketWebcam3Data->apmib_UID, Ampib_data->apmib_UID);
	socketWebcam3Data->port = 1238;
	socketWebcam3Data->flag_run_once = true;

	SocketData *socketWebcam4Data = (SocketData *)malloc(sizeof(SocketData));
	strcpy(socketWebcam4Data->apmib_gw, Ampib_data->apmib_gw);
	strcpy(socketWebcam4Data->apmib_mno, Ampib_data->apmib_mno);
	strcpy(socketWebcam4Data->apmib_UID, Ampib_data->apmib_UID);
	socketWebcam4Data->port = 1239;
	socketWebcam4Data->flag_run_once = true;

	pthread_t Thread_ServerSocket;
	pthread_create(&Thread_ServerSocket, NULL, &ServerSocket_thread_info, NULL);
	pthread_detach(Thread_ServerSocket);

	pthread_t Thread_ServerWebcam1;

	pthread_t Thread_ServerWebcam2;

	pthread_t Thread_ServerWebcam3;

	pthread_t Thread_ServerWebcam4;

	pthread_t Thread_pidof;
	pthread_create(&Thread_pidof, NULL, &Pidof_thread_work, NULL);
	pthread_detach(Thread_pidof);
	if (DEBUG_PRINTF)
	{
		printf("ip address=(%s)\n", WebApiinfo->CurrIP);
		printf("ip6 address=(%s)\n", WebApiinfo->CurrIP6);

		printf("UID=%s\n", UID);
	}

	pthread_create(&threadID_Login, NULL, &_thread_Login, (void *)UID);
	pthread_detach(threadID_Login);

	t = UtcTime();
	if (DEBUG_PRINTF)
	{
		printf("UTC=>%ld\n", *t);
		printf("timestring=(%s)\n", TimeFormat());

		printf("apmib_gw = %s\n", Ampib_data->apmib_gw);
		printf("apmib_mno = %s\n", Ampib_data->apmib_mno);
		printf("apmib_UID = %s\n", UID);
	}

	// sleep(2);
	strcpy(time_temp, TimeFormat());
	socketDatainfo.sys_backup_count = 0;

	// power_on_data
	// sprintf(power_on_data,"curl -m 30 -i -X POST -H \"Content-Type:application/json\" -d \'{\"sno\":\"001\",\"sigtime\":\"%s \",\"catg\":\"3\",\"mno\":\"%s\",\"uid\":\"%s\",\"power\":\"A\",\"lan\":\"1\",\"scode\":\"ON022000000,\"}\' http://wlsecurity069.sks.com.tw/uisgw/api/receive\n",TimeFormat(),Ampib_data->apmib_mno,UID);
	// WriteToDayRunFile(power_on_data,DAYRUN_FILE);

	// if (DEBUG_PRINTF) printf("\nread usb start time=>%s\n\n",TimeFormat());
	// // printf(time_tmp);
	// system("cp /tmp/usb/sda1/dayrun/DayRun /var/www/DayRun");
	// sync();
	// /tmp/usb/sda1/dayrun
	// if(flash_read(FLASH_DayRun_START, 4096*4, "/var/www/DayRuntmp.tar") == 0){
	// 	if (DEBUG_PRINTF)
	// 		printf("%s: %s (%d)flash read ok\n",__FILE__,__func__,__LINE__);
	// }
	// else{
	// 	if (DEBUG_PRINTF)
	// 		printf("%s: %s (%d)flash read fail\n",__FILE__,__func__,__LINE__);
	// }
	// system("tar xvf /var/www/DayRuntmp.tar");
	// system("cat /var/www/DayRuntmp");
	// system("cp /var/www/DayRuntmp /var/www/DayRun");
	// system("rm /var/www/DayRuntmp");
	// system("rm /var/www/DayRuntmp.tar");

	if (flash_read(FLASH_SKSLOG_START, 4096 * 2, "/var/www/sks.log") == 0)
	{
		if (DEBUG_PRINTF)
			printf("%s: %s (%d)flash read ok\n", __FILE__, __func__, __LINE__);
	}
	else
	{
		if (DEBUG_PRINTF)
			printf("%s: %s (%d)flash read fail\n", __FILE__, __func__, __LINE__);
	}
	sync();

	while (1)
	{

		if (socketWebcam1Data->flag_run_once)
		{
			socketWebcam1Data->flag_run_once = false;
			pthread_create(&Thread_ServerWebcam1, NULL, &ServerSocket_thread_webcam1, (void *)socketWebcam1Data);
			pthread_detach(Thread_ServerWebcam1);
		}

		if (socketWebcam2Data->flag_run_once)
		{
			socketWebcam2Data->flag_run_once = false;
			pthread_create(&Thread_ServerWebcam2, NULL, &ServerSocket_thread_webcam2, (void *)socketWebcam2Data);
			pthread_detach(Thread_ServerWebcam2);
		}

		if (socketWebcam3Data->flag_run_once)
		{
			socketWebcam3Data->flag_run_once = false;
			pthread_create(&Thread_ServerWebcam3, NULL, &ServerSocket_thread_webcam3, (void *)socketWebcam3Data);
			pthread_detach(Thread_ServerWebcam3);
		}

		if (socketWebcam4Data->flag_run_once)
		{
			socketWebcam4Data->flag_run_once = false;
			pthread_create(&Thread_ServerWebcam4, NULL, &ServerSocket_thread_webcam4, (void *)socketWebcam4Data);
			pthread_detach(Thread_ServerWebcam4);
		}

		pthread_t Thread_ID1;
		// pthread_t Thread_ID2;
		if (DEBUG_PRINTF)
			printf("waiting IOTC_Listen().........\n");

		int nSID = IOTC_Listen(0);
		if (DEBUG_PRINTF)
			printf("Step 3: IOTC_Listen() nSID = %d \n", nSID);

		if (nSID > -1)
		{
			if (DEBUG_PRINTF)
				printf("Step 4: Create a Thread to handle data read / write from this session\n");
			//================= Read ==============================================
			TargetChannel *pTargetChl0 = (TargetChannel *)malloc(sizeof(TargetChannel));
			// Flag_IOT_SID[nSID]=true;
			pTargetChl0->nSID = nSID;
			pTargetChl0->ChID = ChID0;
			pTargetChl0->Flag_TUTK_read = 0;

			sprintf(write_sys_log_buf, "%s - aw.APP_login_SID=%d! \n", TimeFormat(), nSID);
			echo_file(write_sys_log_buf);
			// socketDatainfo.sys_backup_count++;
			// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf);  //SKS_FILE_PATH
			// sync();
			// system("cp /var/www/sks.log /var/www/sks.logtmp");
			// if(flash_write(FLASH_SKSLOG_START, 4096*2, "/var/www/sks.logtmp")){
			// 	if (DEBUG_PRINTF)
			// 		printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
			// }
			// else{
			// 	if (DEBUG_PRINTF)
			// 		printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
			// }
			// system("rm /var/www/sks.logtmp");
			// sync();
			if (DEBUG_PRINTF)
				printf("%s\n\n", write_sys_log_buf);

			strcpy(pTargetChl0->gw, Ampib_data->apmib_gw);
			strcpy(pTargetChl0->mno, Ampib_data->apmib_mno);
			strcpy(pTargetChl0->UID, UID);
			pthread_create(&Thread_ID1, NULL, &thread_ForSessionRead, (void *)pTargetChl0);
			pthread_detach(Thread_ID1);
		}
		else if (nSID == IOTC_ER_EXCEED_MAX_SESSION)
		{
			// printf("\n**********IOTC_ER_EXCEED_MAX_SESSION*************\n\n");

			sprintf(write_sys_log_buf, "%s - aw.Error_exceed_MAX_sessin(SID=%d)! \n", TimeFormat(), nSID);
			echo_file(write_sys_log_buf);
			// socketDatainfo.sys_backup_count++;
			// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf);  //SKS_FILE_PATH
			// sync();
			// system("cp /var/www/sks.log /var/www/sks.logtmp");
			// if(flash_write(FLASH_SKSLOG_START, 4096*2, "/var/www/sks.logtmp")){
			// 	if (DEBUG_PRINTF)
			// 		printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
			// }
			// else{
			// 	if (DEBUG_PRINTF)
			// 		printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
			// }
			// system("rm /var/www/sks.logtmp");
			// sync();

			sleep(1);
			CountMaxSession++;

			if (CountMaxSession == 4)
			{
				CountMaxSession = 0;
				for (k = 0; k < MAX_SESSION; k++)
				{

					IOTC_Session_Close(k);
					sprintf(write_sys_log_buf, "%s - aw.Close_All_Session_SID=%d! \n", TimeFormat(), nSID);

					echo_file(write_sys_log_buf);

					// socketDatainfo.sys_backup_count++;
					// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf);  //SKS_FILE_PATH
					// sync();
					// system("cp /var/www/sks.log /var/www/sks.logtmp");
					// if(flash_write(FLASH_SKSLOG_START, 4096*2, "/var/www/sks.logtmp")){
					// 	if (DEBUG_PRINTF)
					// 		printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
					// }
					// else{
					// 	if (DEBUG_PRINTF)
					// 		printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
					// }
					// system("rm /var/www/sks.logtmp");
					// sync();
				}
				// system("echo 4 > /var/www/mode.ini");
			}
		}
		else
		{
			// printf("IOTC_Listen--Error[%d]\n", nSID);
			sprintf(write_sys_log_buf, "%s - aw.Error_IOTC_Listen_fail(SID=%d)! \n", TimeFormat(), nSID);
			echo_file(write_sys_log_buf);
			// socketDatainfo.sys_backup_count++;
			// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf);  //SKS_FILE_PATH
			// sync();
			// system("cp /var/www/sks.log /var/www/sks.logtmp");
			// if(flash_write(FLASH_SKSLOG_START, 4096*2, "/var/www/sks.logtmp")){
			// 	if (DEBUG_PRINTF)
			// 		printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
			// }
			// else{
			// 	if (DEBUG_PRINTF)
			// 		printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
			// }
			// system("rm /var/www/sks.logtmp");
			// sync();

			if (DEBUG_PRINTF)
				printf("IOTCAPIs_Device exit...!!\n\n");
			break;
		}
	}
	// # TODO:需要注意程式執行到這裡就結束了,正常情況下是不會執行到這裡
	if (DEBUG_PRINTF)
		printf("finish...\n");
	IOTC_DeInitialize();
	return 0;
}

// 參數檔數檔設置 SksAgentUtility.h
// app login, logout SID資料顥示並寫入UIS日誌
// 技研要求 UIS日誌不要寫入 app login, logout SID 通道相關資料顥示,改寫入/var/www/othersys.log

// 所有 APP P2P 命令 都回覆
// 修正AGENT接收漢唐socket資料timeout錯誤訊息重覆顯示死回圈問題!
//  max session 偵測時間45秒 並寫入UIS
//  檢查IOT 其他讀取錯誤並顯示 並寫入UIS
//  當IOT SESSION 閒置超過50秒釋放SESSION通道並不寫入UIS
// 修正WEBCAM2 會顯示停止

// 收到(2,1)通道登出.TUTK 寫入有問題通道登出 IOT資料不寫出
//  每次通道登出都送(6,0) WEBCAM停止

// 伺服器逆控轉指令(5,1)--->(5,6) 原(5,6)不變化

// 判定歷史影像或圖片

// 修正逆控　帶入hearder 參數

// 將IOT LOGIN LOGOUT 寫入UIS日誌

// 將IOT LOGIN LOGOUT 移出UIS日誌
//  定時重置程式

// 561C　　
// IOT 有收到login 指令才會送接收其他指令
// IOT TUTK IOT區域設定

// 584C
// aPP_IOT_LOGOUT_SID移除
// ５９７Ｃ
// IOT 遠端重開機揩令
// IOT 遠端重啟ＨＢ　每12分+19+0~9重啟ＨＢ
// TUTK 離開時延時可避免跳脫

// 未
// APP WEBCAP issue