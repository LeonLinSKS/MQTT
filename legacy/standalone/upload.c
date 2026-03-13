#include "SksAgentUtility.h"
#define BK_COUNT 10
#define RETRY 3
// #include <stdio.h>
// #include <string.h>
// #include <stdlib.h>
// #include <pthread.h>
// #include <unistd.h>
// #include <mqtt.h>
// #include "IOTCAPIs.h"   ///

// # include<sys/socket.h>
// # include<sys/file.h>
// # include<arpa/inet.h>
// # include<netinet/in.h>
// # include<signal.h>
// //# =====cJSON========
// #include <sys/types.h> 
// #include <sys/stat.h> 
// #include <curl/curl.h>
// #include <signal.h>
// // #include <unistd.h>
// #include <sys/time.h>
// #include <time.h>
// #include <termios.h>
// #include <fcntl.h> 
// #include <sys/statfs.h>
// #include <dirent.h>
// #include <net/if.h>
// #include <ifaddrs.h>
// #include <errno.h>
// #include <sys/ipc.h>
// #include <sys/shm.h>

// #include <sys/select.h>
// #include <netdb.h>


// msdk-linux-gcc -I/home/kk/8198c_toolchain/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/include -g -Wall -O2 -L/home/kk/8198c_toolchain/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/lib -Wl,-rpath=/home/kk/8198c_toolchain/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/lib upload.c libapmib.so cJSON.c -lIOTCAPIs -lNebulaAPIs -lTUTKGlobalAPIs -lm -lcurl -lpthread -o upload_8198
// gcc -g -Wall -O2  upload.c  cJSON.c -lm -lcurl -lpthread -o upload
struct timeval tv_now; 
struct timezone tz_now;
Apmib *Ampib_data;
WebApiUploadData WebApiUploadinfo;
WebApiUSBData WebApiUSBinfo;

int echo_file(char *data );

struct memory {
	char *response;
	size_t size;
};

const char* GetInterface(){	

	struct stat st1_GetInterface;
	int size_GetInterface = 0;
	//GetInterface()

    if(stat("/tmp/3g_chk",&st1_GetInterface) ==0){
    	size_GetInterface = st1_GetInterface.st_size;
        if(size_GetInterface){ // has pppoe interface now distinguish 3G or ADSL                		
	      	return "ppp0";    	 
 		}
 		else{
 			return "eth1";	
 		}	
    }        
    else{
		return "eth1";
  	}
}

static size_t cb(void *data, size_t size, size_t nmemb, void *userp)
{
	size_t realsize_cb;
//cb()	
	realsize_cb = size * nmemb;
	struct memory *mem = (struct memory *)userp; 
	char *ptr = realloc(mem->response, mem->size + realsize_cb + 1);
	if(ptr == NULL)
		return 0;  /* out of memory! */ 
	mem->response = ptr;
	memcpy(&(mem->response[mem->size]), data, realsize_cb);
	mem->size += realsize_cb;
	mem->response[mem->size] = 0;
	return realsize_cb;
}
int SksWebApiUSB_download(){
	CURL        *curl_SKSWebApi;
	CURLcode    res_SKSWebApi;
	char webapi_response_message_SKSWebApi[128];
	// char uis_temp_data[512];
	struct memory chunk_SKSWebApi;// = {0};	
	// if (Debug_Socket_server_webapi_String) 
	// struct curl_slist *headers_SKSWebApi = NULL;
	// int i_SKSWebApi;
	long response_code1_SKSWebApi;
	long response_code2_SKSWebApi;
	int res1_SKSWebApi; 
	
	chunk_SKSWebApi.response = NULL;
	chunk_SKSWebApi.size =0 ;
	int index=0;
	int return_data;

	while(true){
		index++;
		printf("USBdata upload (%d) time!\n",index);

		curl_global_init(CURL_GLOBAL_ALL);
		curl_SKSWebApi        = curl_easy_init();
			// printf("****************SKSWebApi urlstr=\n(%s)\n",WebApiinfo->urlstr);
		if (curl_SKSWebApi){			
			// struct curl_httppost *post = NULL;
			// struct curl_httppost *last = NULL;
			// 添加一个文本字段

			// printf("1.mno=>%s,file=>%s,url=>%s\n",WebApiUSBinfo.mno,WebApiUSBinfo.postfile,WebApiUSBinfo.urlstr);
			// curl_formadd(&post, &last,
            //          CURLFORM_COPYNAME, "mno",
            //          CURLFORM_COPYCONTENTS, WebApiUSBinfo.mno,
            //          CURLFORM_END);

			// // 添加一个文件
			// curl_formadd(&post, &last,
			// 			CURLFORM_COPYNAME, "fileUpload",
			// 			CURLFORM_FILE, WebApiUSBinfo.postfile,
			// 			CURLFORM_END);
			// curl_easy_setopt(curl_SKSWebApi, CURLOPT_CUSTOMREQUEST, WebApiUploadinfo.methodstr);		

			// 设置 URL 和其他选项
			curl_easy_setopt(curl_SKSWebApi, CURLOPT_CUSTOMREQUEST, "GET");

			// if (WebApiUSBinfo.ssl_bool==false){
			curl_easy_setopt(curl_SKSWebApi, CURLOPT_USERNAME, "2272dab338444400966b8f1fdd18fefe");
			curl_easy_setopt(curl_SKSWebApi, CURLOPT_PASSWORD, "YUaRpI1N6hJOKiouUx3eW50cWsSi58akTnwqnjkEPm22t3b2RK4SSsO9tZPbUqjm");

			// }
			

			curl_easy_setopt(curl_SKSWebApi, CURLOPT_URL, WebApiUSBinfo.urlstr);        //URL parameter
			

			// // 設置超時時間為 60 秒
			// curl_easy_setopt(curl_SKSWebApi, CURLOPT_TIMEOUT, 60L);
			// 設置失敗時不返回 HTTP 錯誤響應
			curl_easy_setopt(curl_SKSWebApi, CURLOPT_FAILONERROR, 1L);
			
			// curl_easy_setopt(curl_SKSWebApi, CURLOPT_HTTPHEADER, headers_SKSWebApi);
			
			curl_easy_setopt(curl_SKSWebApi, CURLOPT_FOLLOWLOCATION, 1L); // redirection
			curl_easy_setopt(curl_SKSWebApi, CURLOPT_WRITEFUNCTION, cb);	/* Function Pointer "writer" manages the required buffer size */
			curl_easy_setopt(curl_SKSWebApi, CURLOPT_WRITEDATA, (void *)&chunk_SKSWebApi); /* Data Pointer &buffer stores downloaded web content */
			if (WebApiUSBinfo.ssl_bool){
				curl_easy_setopt(curl_SKSWebApi, CURLOPT_SSL_VERIFYHOST, 0);  //  SSL peer certificate
				curl_easy_setopt(curl_SKSWebApi, CURLOPT_SSL_VERIFYPEER, 0);
			}
			
			res_SKSWebApi = curl_easy_perform(curl_SKSWebApi);
				
			if (chunk_SKSWebApi.size){
				strcpy(WebApiUploadinfo.outputstr,chunk_SKSWebApi.response);
				if (DEBUG_PRINTF)
					printf("Response output data==>\n%s\n",WebApiUploadinfo.outputstr);  //******************
			} 
			else{
				if (DEBUG_PRINTF)
					printf("No output!\n");
			}

			usleep(50);
			
			if (res_SKSWebApi!=CURLE_OK){//webapi send fail
						
				sprintf(webapi_response_message_SKSWebApi,"%s_(res=%d)",curl_easy_strerror(res_SKSWebApi),res_SKSWebApi);
				strcpy(WebApiUploadinfo.response_message,webapi_response_message_SKSWebApi);

				
				fprintf(stderr, "%s(res=%d) !\n",curl_easy_strerror(res_SKSWebApi),res_SKSWebApi);
				if (DEBUG_PRINTF)
					printf("WebApiDataInfo==>%s\n",WebApiUploadinfo.response_message);
			}
			else{		
				curl_easy_getinfo(curl_SKSWebApi, CURLINFO_RESPONSE_CODE, &response_code1_SKSWebApi);
				WebApiUploadinfo.response_code = response_code1_SKSWebApi;
				if (DEBUG_PRINTF)
					printf("***********************SKSWebApi ==>CURLINFO_RESPONSE_CODE status code (%03ld)\n",WebApiUploadinfo.response_code);
				if (WebApiUploadinfo.response_code==200){
					sprintf(webapi_response_message_SKSWebApi,"%s%ld","At.",WebApiUploadinfo.response_code);				
				}else if (WebApiUploadinfo.response_code==201){
					sprintf(webapi_response_message_SKSWebApi,"%s%ld","Cted.",WebApiUploadinfo.response_code);
				}else if (WebApiUploadinfo.response_code==204){
					sprintf(webapi_response_message_SKSWebApi,"%s%ld","No_content",WebApiUploadinfo.response_code);
				}else if (WebApiUploadinfo.response_code==400){
					sprintf(webapi_response_message_SKSWebApi,"%s%ld","Bad_Request",WebApiUploadinfo.response_code);
				}else if (WebApiUploadinfo.response_code==404){
					sprintf(webapi_response_message_SKSWebApi,"%s%ld","Not_Fount",WebApiUploadinfo.response_code);
				}else if (WebApiUploadinfo.response_code==403){
					sprintf(webapi_response_message_SKSWebApi,"%s%ld","Forbidden",WebApiUploadinfo.response_code);
				}else if (WebApiUploadinfo.response_code==409){
					sprintf(webapi_response_message_SKSWebApi,"%s%ld","Conflict",WebApiUploadinfo.response_code);
				}else if (WebApiUploadinfo.response_code==500){
					sprintf(webapi_response_message_SKSWebApi,"%s%ld","Server_Error",WebApiUploadinfo.response_code);
				}else{
					sprintf(webapi_response_message_SKSWebApi,"%s%ld","Other_Condition",WebApiUploadinfo.response_code);
				}

				strcpy(WebApiUploadinfo.response_message,webapi_response_message_SKSWebApi);
				// if (Debug_Socket_server_webapi_String) 
				if (DEBUG_PRINTF)
					if (Debug_Socket_server_webapi_String|Debug_file_data_show) 
						printf("***********************SKSWebApi CURLE_OK_resprone=>%s\n",WebApiUploadinfo.response_message);				
				
				res1_SKSWebApi = curl_easy_getinfo(curl_SKSWebApi, CURLINFO_HTTP_CONNECTCODE, &response_code2_SKSWebApi);
				
				if ((!res1_SKSWebApi)&response_code2_SKSWebApi)
					if (DEBUG_PRINTF) 
						printf("*************8CURLINFO_HTTP_CONNECTCODE status code******************** ==>%03ld,ret==>%3d\n\n",response_code2_SKSWebApi,res1_SKSWebApi);            
				
			}
			// curl_formfree(post);
			// curl_slist_free_all(headers_SKSWebApi);			
			curl_easy_cleanup(curl_SKSWebApi);
			curl_global_cleanup();	

			if (res_SKSWebApi==CURLE_OK){
				return_data= res_SKSWebApi;
				printf("curl ok!\n");
				break;
			}else{
				if (index ==2){
					return_data= res_SKSWebApi;
					printf("curl retry fail!\n");
					break;
				}
			}
						
		}else{
			if (DEBUG_PRINTF)
				printf("curl open fial!\n");
				return_data= -1;
				break;
						
			// return -1;
		}

	}
	return return_data;

}

int SKSWebApiUpload(){

	CURL        *curl_SKSWebApi;
	CURLcode    res_SKSWebApi;
	char webapi_response_message_SKSWebApi[128];
	// char uis_temp_data[512];
	struct memory chunk_SKSWebApi;// = {0};	
	// if (Debug_Socket_server_webapi_String) 
	struct curl_slist *headers_SKSWebApi = NULL;
	// int i_SKSWebApi;
	long response_code1_SKSWebApi;
	long response_code2_SKSWebApi;
	int res1_SKSWebApi; 
	
	chunk_SKSWebApi.response = NULL;
	chunk_SKSWebApi.size =0 ;
	int index=0;
	int return_data;

	while(true){
		index++;
		printf("upload run (%d) time!\n",index);

		curl_global_init(CURL_GLOBAL_ALL);
		curl_SKSWebApi        = curl_easy_init();
			// printf("****************SKSWebApi urlstr=\n(%s)\n",WebApiinfo->urlstr);
		if (curl_SKSWebApi){			
			struct curl_httppost *post = NULL;
			struct curl_httppost *last = NULL;

			//ADD A FILE
			curl_formadd(&post, &last,
						CURLFORM_COPYNAME,"file",
						CURLFORM_FILE,WebApiUploadinfo.postfile,
						CURLFORM_END);
			// curl_easy_setopt(curl_SKSWebApi, CURLOPT_CUSTOMREQUEST, WebApiUploadinfo.methodstr);			
			curl_easy_setopt(curl_SKSWebApi, CURLOPT_URL, WebApiUploadinfo.urlstr);        //URL parameter
			curl_easy_setopt(curl_SKSWebApi,CURLOPT_HTTPPOST,post);	

			// // 設置超時時間為 60 秒
			// curl_easy_setopt(curl_SKSWebApi, CURLOPT_TIMEOUT, 60L);
			// 設置失敗時不返回 HTTP 錯誤響應
			curl_easy_setopt(curl_SKSWebApi, CURLOPT_FAILONERROR, 1L);
			// headers_SKSWebApi = NULL;
			// headers_SKSWebApi = curl_slist_append(headers_SKSWebApi, "postman-token: 59388f84-9771-32de-ae0f-f5234a0ea690");
			// headers_SKSWebApi = curl_slist_append(headers_SKSWebApi, "cache-control: no-cache");
			// headers_SKSWebApi = curl_slist_append(headers_SKSWebApi, "content-type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
			// if (strcmp(WebApiUploadinfo.methodstr,"GET")){
			// 	headers_SKSWebApi = curl_slist_append(headers_SKSWebApi, "content-type: application/json");
			// 	if (DEBUG_PRINTF)
			// 		printf("%s Method\n",WebApiUploadinfo.methodstr);
			// } else{
			// 	if (DEBUG_PRINTF)
			// 		printf("GET Method\n");
			// } 
			curl_easy_setopt(curl_SKSWebApi, CURLOPT_HTTPHEADER, headers_SKSWebApi);
			// printf("\nWebApiUploadinfo.postfile=(%s)\n",WebApiUploadinfo.postfile);
			// curl_easy_setopt(curl_SKSWebApi, CURLOPT_POSTFIELDS, WebApiUploadinfo.postfile);
			curl_easy_setopt(curl_SKSWebApi, CURLOPT_FOLLOWLOCATION, 1L); // redirection
			curl_easy_setopt(curl_SKSWebApi, CURLOPT_WRITEFUNCTION, cb);	/* Function Pointer "writer" manages the required buffer size */
			curl_easy_setopt(curl_SKSWebApi, CURLOPT_WRITEDATA, (void *)&chunk_SKSWebApi); /* Data Pointer &buffer stores downloaded web content */
			if (WebApiUploadinfo.ssl_bool){
				curl_easy_setopt(curl_SKSWebApi, CURLOPT_SSL_VERIFYHOST, 0);  //  SSL peer certificate
				curl_easy_setopt(curl_SKSWebApi, CURLOPT_SSL_VERIFYPEER, 0);
			}
			// curl_easy_setopt(curl, CURLOPT_POSTFIELDS, WebApiDataInfo.string_json_data);
			// curl_easy_setopt(curl_SKSWebApi, CURLOPT_POSTFIELDS, WebApiUploadinfo.jsonstr);
			// if (DEBUG_PRINTF)
			// 	if (Debug_Socket_server_webapi_String|Debug_file_data_show)
			// 		printf("**********SKSWebApi input json data WebApiinfo->jsonstr==>\n(%s)\n",WebApiUploadinfo.jsonstr);
			// // printf( WebApiinfo->inputstr);
			
			
			// for (i_SKSWebApi=0;i_SKSWebApi<RETRY;i_SKSWebApi++){
				// if (DEBUG_PRINTF)
				// 	if (Debug_Socket_server_webapi_String|Debug_file_data_show) 
				// 		printf("\n\n***********************SKSWebApi curl_easy_perform() exec before input json data WebApiinfo->jsonstr==>\n(%s)\n",WebApiUploadinfo.jsonstr);
			res_SKSWebApi = curl_easy_perform(curl_SKSWebApi);
				// if (DEBUG_PRINTF)                      // default get
				// 	if (Debug_Socket_server_webapi_String|Debug_file_data_show) 
				// 		printf("***********************SKSWebApi curl_easy_perform() exec after input json data WebApiinfo->jsonstr==>\n(%s)\n",WebApiUploadinfo.jsonstr);
				
			if (chunk_SKSWebApi.size){
				strcpy(WebApiUploadinfo.outputstr,chunk_SKSWebApi.response);
				if (DEBUG_PRINTF)
					printf("Response output data==>\n%s\n",WebApiUploadinfo.outputstr);  //******************
			} 
			else{
				if (DEBUG_PRINTF)
					printf("No output!\n");
			}

				// if(res_SKSWebApi != CURLE_OK){
				// 	if (DEBUG_PRINTF)
				// 		printf("CURL RETRY index=%d,curl_easy_perform continue(res=%d)!\n",i_SKSWebApi,res_SKSWebApi);
				// 	continue;
				// }
				// else{
				// 	if (DEBUG_PRINTF)
				// 		printf("CURLE_OK index=%d,curl_easy_perform break(res=%d)!\n",i_SKSWebApi,res_SKSWebApi);
				// 	break;
				// }	
				usleep(50);
			// }
			if (res_SKSWebApi!=CURLE_OK){//webapi send fail
				// if (DEBUG_PRINTF)
				// 	printf("Webapi retry %d time fail! curl=(%d)\n",i_SKSWebApi,res_SKSWebApi);			
				sprintf(webapi_response_message_SKSWebApi,"%s_(res=%d)",curl_easy_strerror(res_SKSWebApi),res_SKSWebApi);
				strcpy(WebApiUploadinfo.response_message,webapi_response_message_SKSWebApi);

				
				fprintf(stderr, "%s(res=%d) !\n",curl_easy_strerror(res_SKSWebApi),res_SKSWebApi);
				if (DEBUG_PRINTF)
					printf("WebApiDataInfo==>%s\n",WebApiUploadinfo.response_message);
				// WebApiUploadinfo.webapi_send_fail=true;
				// return res_SKSWebApi;		

			}
			else{		
				
				curl_easy_getinfo(curl_SKSWebApi, CURLINFO_RESPONSE_CODE, &response_code1_SKSWebApi);
				WebApiUploadinfo.response_code = response_code1_SKSWebApi;
				if (DEBUG_PRINTF)
					printf("***********************SKSWebApi ==>CURLINFO_RESPONSE_CODE status code (%03ld)\n",WebApiUploadinfo.response_code);
				if (WebApiUploadinfo.response_code==200){
					sprintf(webapi_response_message_SKSWebApi,"%s%ld","At.",WebApiUploadinfo.response_code);				
				}else if (WebApiUploadinfo.response_code==201){
					sprintf(webapi_response_message_SKSWebApi,"%s%ld","Cted.",WebApiUploadinfo.response_code);
				}else if (WebApiUploadinfo.response_code==204){
					sprintf(webapi_response_message_SKSWebApi,"%s%ld","No_content",WebApiUploadinfo.response_code);
				}else if (WebApiUploadinfo.response_code==400){
					sprintf(webapi_response_message_SKSWebApi,"%s%ld","Bad_Request",WebApiUploadinfo.response_code);
				}else if (WebApiUploadinfo.response_code==404){
					sprintf(webapi_response_message_SKSWebApi,"%s%ld","Not_Fount",WebApiUploadinfo.response_code);
				}else if (WebApiUploadinfo.response_code==403){
					sprintf(webapi_response_message_SKSWebApi,"%s%ld","Forbidden",WebApiUploadinfo.response_code);
				}else if (WebApiUploadinfo.response_code==409){
					sprintf(webapi_response_message_SKSWebApi,"%s%ld","Conflict",WebApiUploadinfo.response_code);
				}else if (WebApiUploadinfo.response_code==500){
					sprintf(webapi_response_message_SKSWebApi,"%s%ld","Server_Error",WebApiUploadinfo.response_code);
				}else{
					sprintf(webapi_response_message_SKSWebApi,"%s%ld","Other_Condition",WebApiUploadinfo.response_code);
				}

				strcpy(WebApiUploadinfo.response_message,webapi_response_message_SKSWebApi);
				// if (Debug_Socket_server_webapi_String) 
				if (DEBUG_PRINTF)
					if (Debug_Socket_server_webapi_String|Debug_file_data_show) 
						printf("***********************SKSWebApi CURLE_OK_resprone=>%s\n",WebApiUploadinfo.response_message);				
				
				res1_SKSWebApi = curl_easy_getinfo(curl_SKSWebApi, CURLINFO_HTTP_CONNECTCODE, &response_code2_SKSWebApi);
				
				if ((!res1_SKSWebApi)&response_code2_SKSWebApi)
					if (DEBUG_PRINTF) 
						printf("*************8CURLINFO_HTTP_CONNECTCODE status code******************** ==>%03ld,ret==>%3d\n\n",response_code2_SKSWebApi,res1_SKSWebApi);            
				
			}

			curl_slist_free_all(headers_SKSWebApi);			
			curl_easy_cleanup(curl_SKSWebApi);
			curl_global_cleanup();	
			// WebApiinfo->Curl_exeic=false;
			// free(chunk_SKSWebApi.response);
			if (res_SKSWebApi==CURLE_OK){
				return_data= res_SKSWebApi;
				printf("curl ok!\n");
				break;
			}else{
				if (index ==2){
					return_data= res_SKSWebApi;
					printf("curl retry fail!\n");
					break;
				}
			}


						
		}else{
			if (DEBUG_PRINTF)
				printf("curl open fial!\n");
				return_data= -1;
				break;
						
			// return -1;
		}

	}
	return return_data;
    
	
	
}
int SksWebApiUSB_upload(){
	CURL        *curl_SKSWebApi;
	CURLcode    res_SKSWebApi;
	char webapi_response_message_SKSWebApi[128];
	// char uis_temp_data[512];
	struct memory chunk_SKSWebApi;// = {0};	
	// if (Debug_Socket_server_webapi_String) 
	struct curl_slist *headers_SKSWebApi = NULL;
	// int i_SKSWebApi;
	long response_code1_SKSWebApi;
	long response_code2_SKSWebApi;
	int res1_SKSWebApi; 
	
	chunk_SKSWebApi.response = NULL;
	chunk_SKSWebApi.size =0 ;
	int index=0;
	int return_data;

	while(true){
		index++;
		printf("USBdata upload (%d) time!\n",index);

		curl_global_init(CURL_GLOBAL_ALL);
		curl_SKSWebApi        = curl_easy_init();
			// printf("****************SKSWebApi urlstr=\n(%s)\n",WebApiinfo->urlstr);
		if (curl_SKSWebApi){			
			struct curl_httppost *post = NULL;
			struct curl_httppost *last = NULL;
			// 添加一个文本字段

			printf("1.mno=>%s,file=>%s,url=>%s\n",WebApiUSBinfo.mno,WebApiUSBinfo.postfile,WebApiUSBinfo.urlstr);
			curl_formadd(&post, &last,
                     CURLFORM_COPYNAME, "mno",
                     CURLFORM_COPYCONTENTS, WebApiUSBinfo.mno,
                     CURLFORM_END);

			// 添加一个文件
			curl_formadd(&post, &last,
						CURLFORM_COPYNAME, "fileUpload",
						CURLFORM_FILE, WebApiUSBinfo.postfile,
						CURLFORM_END);
			// curl_easy_setopt(curl_SKSWebApi, CURLOPT_CUSTOMREQUEST, WebApiUploadinfo.methodstr);		

			// 设置 URL 和其他选项
			curl_easy_setopt(curl_SKSWebApi, CURLOPT_USERNAME, "2272dab338444400966b8f1fdd18fefe");
			curl_easy_setopt(curl_SKSWebApi, CURLOPT_PASSWORD, "YUaRpI1N6hJOKiouUx3eW50cWsSi58akTnwqnjkEPm22t3b2RK4SSsO9tZPbUqjm");

			curl_easy_setopt(curl_SKSWebApi, CURLOPT_URL, WebApiUSBinfo.urlstr);        //URL parameter
			curl_easy_setopt(curl_SKSWebApi,CURLOPT_HTTPPOST,post);	

			// // 設置超時時間為 60 秒
			// curl_easy_setopt(curl_SKSWebApi, CURLOPT_TIMEOUT, 60L);
			// 設置失敗時不返回 HTTP 錯誤響應
			curl_easy_setopt(curl_SKSWebApi, CURLOPT_FAILONERROR, 1L);
			
			// curl_easy_setopt(curl_SKSWebApi, CURLOPT_HTTPHEADER, headers_SKSWebApi);
			
			curl_easy_setopt(curl_SKSWebApi, CURLOPT_FOLLOWLOCATION, 1L); // redirection
			curl_easy_setopt(curl_SKSWebApi, CURLOPT_WRITEFUNCTION, cb);	/* Function Pointer "writer" manages the required buffer size */
			curl_easy_setopt(curl_SKSWebApi, CURLOPT_WRITEDATA, (void *)&chunk_SKSWebApi); /* Data Pointer &buffer stores downloaded web content */
			// if (WebApiUSBinfo.ssl_bool){
			// 	curl_easy_setopt(curl_SKSWebApi, CURLOPT_SSL_VERIFYHOST, 0);  //  SSL peer certificate
			// 	curl_easy_setopt(curl_SKSWebApi, CURLOPT_SSL_VERIFYPEER, 0);
			// }
			
			res_SKSWebApi = curl_easy_perform(curl_SKSWebApi);
				
			if (chunk_SKSWebApi.size){
				strcpy(WebApiUploadinfo.outputstr,chunk_SKSWebApi.response);
				if (DEBUG_PRINTF)
					printf("Response output data==>\n%s\n",WebApiUploadinfo.outputstr);  //******************
			} 
			else{
				if (DEBUG_PRINTF)
					printf("No output!\n");
			}

			usleep(50);
			
			if (res_SKSWebApi!=CURLE_OK){//webapi send fail
						
				sprintf(webapi_response_message_SKSWebApi,"%s_(res=%d)",curl_easy_strerror(res_SKSWebApi),res_SKSWebApi);
				strcpy(WebApiUploadinfo.response_message,webapi_response_message_SKSWebApi);

				
				fprintf(stderr, "%s(res=%d) !\n",curl_easy_strerror(res_SKSWebApi),res_SKSWebApi);
				if (DEBUG_PRINTF)
					printf("WebApiDataInfo==>%s\n",WebApiUploadinfo.response_message);
			}
			else{		
				curl_easy_getinfo(curl_SKSWebApi, CURLINFO_RESPONSE_CODE, &response_code1_SKSWebApi);
				WebApiUploadinfo.response_code = response_code1_SKSWebApi;
				if (DEBUG_PRINTF)
					printf("***********************SKSWebApi ==>CURLINFO_RESPONSE_CODE status code (%03ld)\n",WebApiUploadinfo.response_code);
				if (WebApiUploadinfo.response_code==200){
					sprintf(webapi_response_message_SKSWebApi,"%s%ld","At.",WebApiUploadinfo.response_code);				
				}else if (WebApiUploadinfo.response_code==201){
					sprintf(webapi_response_message_SKSWebApi,"%s%ld","Cted.",WebApiUploadinfo.response_code);
				}else if (WebApiUploadinfo.response_code==204){
					sprintf(webapi_response_message_SKSWebApi,"%s%ld","No_content",WebApiUploadinfo.response_code);
				}else if (WebApiUploadinfo.response_code==400){
					sprintf(webapi_response_message_SKSWebApi,"%s%ld","Bad_Request",WebApiUploadinfo.response_code);
				}else if (WebApiUploadinfo.response_code==404){
					sprintf(webapi_response_message_SKSWebApi,"%s%ld","Not_Fount",WebApiUploadinfo.response_code);
				}else if (WebApiUploadinfo.response_code==403){
					sprintf(webapi_response_message_SKSWebApi,"%s%ld","Forbidden",WebApiUploadinfo.response_code);
				}else if (WebApiUploadinfo.response_code==409){
					sprintf(webapi_response_message_SKSWebApi,"%s%ld","Conflict",WebApiUploadinfo.response_code);
				}else if (WebApiUploadinfo.response_code==500){
					sprintf(webapi_response_message_SKSWebApi,"%s%ld","Server_Error",WebApiUploadinfo.response_code);
				}else{
					sprintf(webapi_response_message_SKSWebApi,"%s%ld","Other_Condition",WebApiUploadinfo.response_code);
				}

				strcpy(WebApiUploadinfo.response_message,webapi_response_message_SKSWebApi);
				// if (Debug_Socket_server_webapi_String) 
				if (DEBUG_PRINTF)
					if (Debug_Socket_server_webapi_String|Debug_file_data_show) 
						printf("***********************SKSWebApi CURLE_OK_resprone=>%s\n",WebApiUploadinfo.response_message);				
				
				res1_SKSWebApi = curl_easy_getinfo(curl_SKSWebApi, CURLINFO_HTTP_CONNECTCODE, &response_code2_SKSWebApi);
				
				if ((!res1_SKSWebApi)&response_code2_SKSWebApi)
					if (DEBUG_PRINTF) 
						printf("*************8CURLINFO_HTTP_CONNECTCODE status code******************** ==>%03ld,ret==>%3d\n\n",response_code2_SKSWebApi,res1_SKSWebApi);            
				
			}
			curl_formfree(post);
			curl_slist_free_all(headers_SKSWebApi);			
			curl_easy_cleanup(curl_SKSWebApi);
			curl_global_cleanup();	

			if (res_SKSWebApi==CURLE_OK){
				return_data= res_SKSWebApi;
				printf("curl ok!\n");
				break;
			}else{
				if (index ==2){
					return_data= res_SKSWebApi;
					printf("curl retry fail!\n");
					break;
				}
			}
						
		}else{
			if (DEBUG_PRINTF)
				printf("curl open fial!\n");
				return_data= -1;
				break;
						
			// return -1;
		}

	}
	return return_data;

}



int main(int argc, char *argv[]){

    FILE *fp_WriteToFileSys;
    int Rows_WriteToFileSys;
    char webapibuf[BK_COUNT][64];
	int len_tmp=0;
	int same_count=0;
	long UTC_timesta;
	// char *mmn="0692983";
	char sks[128];
	char uis[128];
	// char buf[512];
	char temp[128];
	// char curl_command[1024];
	int	res;
	bool flag_del_sks=false;
	bool flag_del_uis=false;
	char write_sys_log_buf[512];
	cJSON *root = NULL;
    cJSON *payload = NULL;
    cJSON *url = NULL;

	Ampib_data = (Apmib *)malloc(sizeof(Apmib));

	apmib_init();
	apmib_get(MIB_USER_PASSWORD, (void *)Ampib_data->apmib_gw);
	apmib_get(MIB_SNMP_RW_COMMUNITY, (void *)Ampib_data->apmib_mno);
	apmib_get(MIB_PPP_PASSWORD4, (void*)Ampib_data->apmib_UID);

	WebApiUploadinfo.ssl_bool = FLAG_SSL_BOOL;
	// strcpy(WebApiUploadinfo.methodstr,"POST"); 

	if (THAI_ADDR_ENABLE){
		sprintf(FIRMWARE_VER2,"%s%s",NUM_VER,THAI);
		if (DEBUG_PRINTF)
			printf("VER==>%s",FIRMWARE_VER2);
	}else{
		sprintf(FIRMWARE_VER2,"%s%s",NUM_VER,SKS);
		if (DEBUG_PRINTF)
			printf("TVER==>%s",FIRMWARE_VER2);
	}
	if (DEBUG_PRINTF)
		printf("\n\n***UPLOAD%s,%s***\n\n",DATE_TIME,FIRMWARE_VER2);

	if (argc==1){
		printf("No parameter and exit!\n");
		return 0;
	}
	else if ((strncmp(argv[1],"usbdownload",11)==0)){
		printf("%s,005\n",argv[1]);
		// sprintf(WebApiUSBinfo.urlstr,"http://api.sksiot.com/iot/sks/downloadConfig?mno=%s",Ampib_data->apmib_mno);
		sprintf(WebApiUSBinfo.urlstr,"http://api.sksiot.com/iot/sks/downloadConfig?mno=%s","0690004");
		WebApiUSBinfo.ssl_bool=false;
		res=SksWebApiUSB_download();
		if (res==-1){
			printf("SKSWebApiUpload() curl function open fail!\n");

		}
		else if(res==CURLE_OK){
			printf("usbdownload SKSWebApiUpload() curl OK, response code=%ld\n",WebApiUploadinfo.response_code);
			if (WebApiUploadinfo.response_code==200) {				
				printf("output info=>%s\n",WebApiUploadinfo.outputstr);
				if (chdir("/tmp/usb/sda1") == 0) {
					printf("Directory changed to %s\n", "/tmp/usb/sda1");
					echo_file("Directory changed to /tmp/usb/sda1");
				} else {
					perror("chdir() failed");
					echo_file("Directory changed failed for /tmp/usb/sda1");
					return 1;
				}
				root=cJSON_Parse(WebApiUploadinfo.outputstr);    
				payload=cJSON_GetObjectItem(root,"payload");
				printf("payload=>%s\n",cJSON_Print(payload));
				url=cJSON_GetObjectItem(payload,"url");
				// printf("url=>%s\n",cJSON_Print(img));
				printf("url=>%s\n",cJSON_GetStringValue(url));
				memset(temp,0,sizeof(temp));
				sprintf(temp,"curl -O %s",cJSON_GetStringValue(url));
				printf("system=>%s\n",temp);
				system(temp);
				system("tar xvf usbbackup.tar");
				printf("untar usbbackup.tar");
				echo_file("untar usbbackup.tar");

				memset(write_sys_log_buf,0,sizeof(write_sys_log_buf));
				// memset(temp,0,sizeof(temp));
				// sprintf(temp,"%s","config upload OK!");
				if (strcmp(GetInterface(),"ppp0")==0){
					sprintf(write_sys_log_buf,"%s - aw.config download OK_4G! \n",TimeFormat());
				}else{
					sprintf(write_sys_log_buf,"%s - aw.config download OK_lan! \n",TimeFormat());
				}						
				printf("***%s**\n",write_sys_log_buf);
				
				sync();
			}
			else
			{
				printf("no_download_data_information!");
				memset(write_sys_log_buf,0,sizeof(write_sys_log_buf));
				// memset(temp,0,sizeof(temp));
				// sprintf(temp,"%s","config upload OK!");
				if (strcmp(GetInterface(),"ppp0")==0){
					sprintf(write_sys_log_buf,"%s - aw.no_download_data(res=%ld)_%s_4G! \n",TimeFormat(),WebApiUploadinfo.response_code,WebApiUploadinfo.response_message);
				}else{
					sprintf(write_sys_log_buf,"%s - aw.no_download_data(res=%ld)_%s_lan! \n",TimeFormat(),WebApiUploadinfo.response_code,WebApiUploadinfo.response_message);
				}						
				printf("*****%s**\n",write_sys_log_buf);
			}			
		}
		else
		{
			printf("no_download_data curl_non_ok !");
			// printf("curl non ok upload file fail_2!\n");						
			memset(write_sys_log_buf,0,sizeof(write_sys_log_buf));
			// memset(temp,0,sizeof(temp));
			// sprintf(temp,"%s","config upload OK!");
			if (strcmp(GetInterface(),"ppp0")==0){
				sprintf(write_sys_log_buf,"%s - aw.download_curl_non_ok_%s_4G! \n",TimeFormat(),WebApiUploadinfo.response_message);
			}else{
				sprintf(write_sys_log_buf,"%s - aw.download_curl_non_ok_%s_lan! \n",TimeFormat(),WebApiUploadinfo.response_message);
			}						
			printf("*****%s**\n",write_sys_log_buf);
		}
		echo_file(write_sys_log_buf);						
		WriteToFileSys(FILE_PATH,write_sys_log_buf);//write heartbeat message to file;
		system("cp /var/www/sys.log /var/www/sys.logtmp");
		if(flash_write(FLASH_UISLOG, 4096*2, "/var/www/sys.logtmp")){
			if (DEBUG_PRINTF)
				printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
		}							
		else{
			if (DEBUG_PRINTF)
				printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
		}													
		system("rm /var/www/sys.logtmp");
		system("rm /tmp/usb/sda1/usbbackup.tar");
		sync();

	}
	else if ((strncmp(argv[1],"usbupload",9)==0)){

		printf("%s,004\n",argv[1]);

		if (chdir("/tmp/usb/sda1") == 0) {
			printf("Directory changed to %s\n", "/tmp/usb/sda1");
			echo_file("Directory changed to /tmp/usb/sda1");
		} else {
			perror("chdir() failed");
			echo_file("Directory changed failed for /tmp/usb/sda1");
			return 1;
		}

		system("rm sys*.log");
		system("tar cvf usbbackup.tar *");
		sync();

		// strcpy(WebApiUSBinfo.mno,Ampib_data->apmib_mno);
		strcpy(WebApiUSBinfo.mno,"0690004");
		strcpy(WebApiUSBinfo.urlstr,"http://api.sksiot.com/iot/sks/uploadConfig");
		strcpy(WebApiUSBinfo.postfile,"usbbackup.tar");
		// WebApiUSBinfo.ssl_bool=false;

		res=SksWebApiUSB_upload();
		if (res==-1){
			printf("SKSWebApiUpload() curl function open fail!\n");

		}
		else if(res==CURLE_OK){
			printf("SKSWebApiUpload() curl OK, response code=%ld\n",WebApiUploadinfo.response_code);
			if (WebApiUploadinfo.response_code==200) {
				
				printf("output info=>%s\n",WebApiUploadinfo.outputstr);						
				memset(write_sys_log_buf,0,sizeof(write_sys_log_buf));
				// memset(temp,0,sizeof(temp));
				// sprintf(temp,"%s","config upload OK!");
				if (strcmp(GetInterface(),"ppp0")==0){
					sprintf(write_sys_log_buf,"%s - aw.config upload OK_4G! \n",TimeFormat());
				}else{
					sprintf(write_sys_log_buf,"%s - aw.config upload OK_lan! \n",TimeFormat());
				}						
				printf("*****%s**\n",write_sys_log_buf);
				
			}
			else{
				printf("upload file fail_1!\n");						
				memset(write_sys_log_buf,0,sizeof(write_sys_log_buf));
				// memset(temp,0,sizeof(temp));
				// sprintf(temp,"%s","config upload OK!");
				if (strcmp(GetInterface(),"ppp0")==0){
					sprintf(write_sys_log_buf,"%s - aw.upload_error(res=%ld)_%s_4G! \n",TimeFormat(),WebApiUploadinfo.response_code,WebApiUploadinfo.response_message);
				}else{
					sprintf(write_sys_log_buf,"%s - aw.upload_error(res=%ld)_%s_lan! \n",TimeFormat(),WebApiUploadinfo.response_code,WebApiUploadinfo.response_message);
				}						
				printf("*****%s**\n",write_sys_log_buf);			
				
			}
		}
		else{
			printf("curl non ok upload file fail_2!\n");						
			memset(write_sys_log_buf,0,sizeof(write_sys_log_buf));
			// memset(temp,0,sizeof(temp));
			// sprintf(temp,"%s","config upload OK!");
			if (strcmp(GetInterface(),"ppp0")==0){
				sprintf(write_sys_log_buf,"%s - aw.upload_curl_non_ok_%s_4G! \n",TimeFormat(),WebApiUploadinfo.response_message);
			}else{
				sprintf(write_sys_log_buf,"%s - aw.upload_curl_non_ok_%s_lan! \n",TimeFormat(),WebApiUploadinfo.response_message);
			}						
			printf("*****%s**\n",write_sys_log_buf);

		}

		echo_file(write_sys_log_buf);						
		WriteToFileSys(FILE_PATH,write_sys_log_buf);//write heartbeat message to file;
		system("cp /var/www/sys.log /var/www/sys.logtmp");
		if(flash_write(FLASH_UISLOG, 4096*2, "/var/www/sys.logtmp")){
			if (DEBUG_PRINTF)
				printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
		}							
		else{
			if (DEBUG_PRINTF)
				printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
		}													
		system("rm /var/www/sys.logtmp");
		system("rm /tmp/usb/sda1/usbbackup.tar");
		sync();

	}
	else if (((strcmp(argv[1],"keep_log")==0))|((strcmp(argv[1],"del_log")==0))){
		system("ls /tmp/usb/sda1/log/ > /var/www/ls.log");//"ls /var/www/log/ > /var/www/ls.log"	
		system("cat /var/www/ls.log");
		if (NULL == (fp_WriteToFileSys = fopen("/var/www/ls.log", "r"))){
			if (1)
				printf("no file,Create New file %s\n","/var/www/ls.log");
			// fp_WriteToFileSys = fopen(filename, "w");       
		}
		else{
			Rows_WriteToFileSys = 0;
			while (fgets(webapibuf[Rows_WriteToFileSys], sizeof(webapibuf[Rows_WriteToFileSys]), fp_WriteToFileSys) != NULL){
				len_tmp=strlen(webapibuf[Rows_WriteToFileSys]);
				webapibuf[Rows_WriteToFileSys][len_tmp-1]=webapibuf[Rows_WriteToFileSys][len_tmp];
				if (strstr(webapibuf[Rows_WriteToFileSys],"sks")){
					gettimeofday (&tv_now , &tz_now);
					UTC_timesta=tv_now.tv_sec;
					printf("tv_now_sec:%ld\n",UTC_timesta);
					sprintf(sks,"https://wirelessdev.azurewebsites.net/api/log/%s/%s/%ld/agent/log",Ampib_data->apmib_mno,Ampib_data->apmib_mno,UTC_timesta);
					// printf("sks=>(%s)\n",sks);
					strcpy(WebApiUploadinfo.urlstr,sks);
					
					printf("%d)(%s)\n",Rows_WriteToFileSys,webapibuf[Rows_WriteToFileSys]); 
					sprintf(WebApiUploadinfo.postfile,"/tmp/usb/sda1/log/%s",webapibuf[Rows_WriteToFileSys]);
					// sprintf(WebApiUploadinfo.postfile,"------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: form-data; name=\"file\"; filename=\"%s\"\r\nContent-Type: text/plain\r\n\r\n\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW--",temp);				
					
					// sprintf(curl_command,"curl -X POST -F \"file=@%s\" %s",temp,sks);
					// printf(curl_command);
					// system(curl_command);
					res=SKSWebApiUpload();
					if (res==-1){
						printf("SKSWebApiUpload() curl function open fail!\n");

					}
					else if(res==CURLE_OK){
						printf("SKSWebApiUpload() curl OK, response code=%ld\n",WebApiUploadinfo.response_code);
						if (WebApiUploadinfo.response_code==200) {							
							if (strcmp(argv[1],"del_log")==0){
								flag_del_sks=true;
							}							
							else if (strcmp(argv[1],"keep_log")==0){							
								flag_del_sks=false;
							}						
							memset(write_sys_log_buf,0,sizeof(write_sys_log_buf));
							if (strcmp(GetInterface(),"ppp0")==0){
								sprintf(write_sys_log_buf,"%s - %s_4G! \n",TimeFormat(),WebApiUploadinfo.outputstr);

							}else{
								sprintf(write_sys_log_buf,"%s - %s_lan! \n",TimeFormat(),WebApiUploadinfo.outputstr);
							}						
							printf("*****%s**\n",write_sys_log_buf);						
							WriteToFileSys(FILE_PATH,write_sys_log_buf);//write heartbeat message to file;
							system("cp /var/www/sys.log /var/www/sys.logtmp");
							if(flash_write(FLASH_UISLOG, 4096*2, "/var/www/sys.logtmp")){
								if (DEBUG_PRINTF)
									printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
							}							
							else{
								if (DEBUG_PRINTF)
									printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
							}													
							system("rm /var/www/sys.logtmp");
							sync();
						}
					}
					else{
						printf("SKSWebApiUpload() non curl OK, rese=%d\n",res);
					}				
					sync();				
				}
				if (strstr(webapibuf[Rows_WriteToFileSys],"uis")){
					gettimeofday (&tv_now , &tz_now);
					UTC_timesta=tv_now.tv_sec;
					printf("tv_now_sec:%ld\n",UTC_timesta);
					sprintf(uis,"https://wirelessdev.azurewebsites.net/api/log/%s/%s/%ld/uis/log",Ampib_data->apmib_mno,Ampib_data->apmib_mno,UTC_timesta);
					// printf("uis=>(%s)\n",uis);
					strcpy(WebApiUploadinfo.urlstr,uis);
					
					printf("%d)(%s)\n",Rows_WriteToFileSys,webapibuf[Rows_WriteToFileSys]);
					sprintf(WebApiUploadinfo.postfile,"/tmp/usb/sda1/log/%s",webapibuf[Rows_WriteToFileSys]);
					// sprintf(WebApiUploadinfo.postfile,"------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: form-data; name=\"file\"; filename=\"%s\"\r\nContent-Type: text/plain\r\n\r\n\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW--",temp);
					
					// sprintf(curl_command,"curl -X POST -F \"file=@%s\" %s",temp,uis);
					// // printf(curl_command);
					// system(curl_command);

					res=SKSWebApiUpload();
					sync();	
					if (res==-1){
						printf("SKSWebApiUpload() curl function open fail!\n");					
					}
					else if(res==CURLE_OK){
						if (WebApiUploadinfo.response_code==200){
							
							if (strcmp(argv[1],"del_log")==0){
								flag_del_uis=true;
							}
							else if (strcmp(argv[1],"keep_log")==0){							
								flag_del_uis=false;
							}

							
							memset(write_sys_log_buf,0,sizeof(write_sys_log_buf));
							if (strcmp(GetInterface(),"ppp0")==0){
								sprintf(write_sys_log_buf,"%s - %s_4G! \n",TimeFormat(),WebApiUploadinfo.outputstr);
							}else{
								sprintf(write_sys_log_buf,"%s - %s_lan! \n",TimeFormat(),WebApiUploadinfo.outputstr);
							}						
							printf("*****%s**\n",write_sys_log_buf);
							WriteToFileSys(FILE_PATH,write_sys_log_buf);//write heartbeat message to file;
							system("cp /var/www/sys.log /var/www/sys.logtmp");
							if(flash_write(FLASH_UISLOG, 4096*2, "/var/www/sys.logtmp")){
								if (DEBUG_PRINTF)
									printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
							}
							else{
								if (DEBUG_PRINTF)
									printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
							}
							system("rm /var/www/sys.logtmp");
							sync();
						} 
						printf("SKSWebApiUpload() curl OK, response code=%ld\n",WebApiUploadinfo.response_code);
					}
					else{
						printf("SKSWebApiUpload() curl OK, rese=%d\n",res);
					}						
				}	

				Rows_WriteToFileSys++;  
				if (Rows_WriteToFileSys==BK_COUNT){
					break;
				}          
			}
			if (1) printf("\nTotal Rows=%d \n", (Rows_WriteToFileSys-same_count)); //# 總共有幾筆
			fclose(fp_WriteToFileSys);
			if (flag_del_sks){
				system("rm /tmp/usb/sda1/log/*sks.log");
			}
			if (flag_del_uis){
				system("rm /tmp/usb/sda1/log/*uis.log");
			}
			flag_del_uis=false;
			flag_del_sks=false;		
		}
	}
	return 0;
}