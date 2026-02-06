#include "SksAgentUtility.h"

// #define SIZE_OF_SKS_DATA sizeof(SKS_data)

// // WriteToDayFile(
// int sno;

/*处理系统调用中产生的错误*/
void error_print(char * ptr);
/*处理通信结束时回调函数接收到的信号*/
// void quit_tranmission(int sig);
bool run_once=true;

char *TimeFormat();

// typedef struct {
// 		int		fd;
// 		unsigned int	offset;
// 		unsigned int	length;
// } MTDEntry, *PMTDEntry;

struct memory {
	char *response;
	size_t size;
};

struct timeval tv_now; 
struct timezone tz_now;

Apmib *Ampib_data;
WebApiData WebApiinfo;
PushApiData PushApiinfo;

int echo_file(char *data );


//main()

// int setMTDEntry(PMTDEntry ee, int offset, int length){
// 	if(offset < CONFIG_RTL_ROOT_IMAGE_OFFSET) {
// 		ee[0].fd = open("/dev/mtdblock0", O_RDWR);
// 		if(ee[0].fd < 0)
// 			return -1;
// 		ee[0].offset = offset;
// 		if(offset+length > CONFIG_RTL_ROOT_IMAGE_OFFSET) {
// 			ee[0].length = CONFIG_RTL_ROOT_IMAGE_OFFSET - offset;
// 			ee[1].fd = open("/dev/mtdblock1", O_RDONLY);
// 			if(ee[1].fd < 0)
// 				return -1;
// 			ee[1].offset = 0;
// 			ee[1].length = offset+length-CONFIG_RTL_ROOT_IMAGE_OFFSET;
// 		} 
// 		else
// 			ee[0].length = length;
// 	} 
// 	else{
// 		ee[1].fd = open("/dev/mtdblock1", O_RDWR);
// 		if(ee[1].fd < 0)
// 			return -1;
// 		ee[1].offset = offset;
// 		ee[1].length = length;
// 	}
// 	return 0;
// }

// int flash_write(int offset, int length, char *filename){
// 	int usrFd_flash_write=-1, count_flash_write, i_flash_write, ret_flash_write=-1;
// 	char buf_flash_write[0x1000];
// 	MTDEntry eDev_flash_write[2] = {{-1,0,0},{-1,0,0}};
	
// 	do {
// 		if(setMTDEntry(eDev_flash_write, offset, length) < 0)
// 			break;
// 		usrFd_flash_write = open(filename, O_RDONLY, 0644);
// 		if(usrFd_flash_write < 0)
// 			break;

// 		for(i_flash_write=0; i_flash_write<2; i_flash_write++) {
// 			if(eDev_flash_write[i_flash_write].fd >= 0) {
// 				lseek(eDev_flash_write[i_flash_write].fd, eDev_flash_write[i_flash_write].offset, SEEK_SET);
// 				for(count_flash_write=0; count_flash_write<eDev_flash_write[i_flash_write].length; count_flash_write+=sizeof(buf_flash_write)) {
// 					read(usrFd_flash_write, buf_flash_write, sizeof(buf_flash_write));
// 					write(eDev_flash_write[i_flash_write].fd, buf_flash_write, sizeof(buf_flash_write));
// 				}
// 			}
// 		}
// 		ret_flash_write = 0;
// 	} while(0);
// 	if(usrFd_flash_write >= 0)
// 		close(usrFd_flash_write);
// 	if(eDev_flash_write[0].fd >= 0)
// 		close(eDev_flash_write[0].fd);
// 	if(eDev_flash_write[1].fd >= 0)
// 		close(eDev_flash_write[1].fd);
// 	return ret_flash_write;
// }

// int flash_read(int offset, int length, char *filename){

// 	//flash_read()
// 	int usrFd_flash_read=-1, count_flash_read, i_flash_read, ret_flash_read=-1;
// 	char buf_flash_read[0x1000];
// 	MTDEntry eDev_flash_read[2] = {{-1,0,0},{-1,0,0}};
// 	//flash_read()



// 	do {
// 		if(setMTDEntry(eDev_flash_read, offset, length) < 0)
// 			break;

// 		usrFd_flash_read = open(filename, O_WRONLY|O_TRUNC|O_CREAT, 0644);
// 		if(usrFd_flash_read < 0)
// 			break;

// 		for(i_flash_read=0; i_flash_read<2; i_flash_read++) {
			
// 			if(eDev_flash_read[i_flash_read].fd >= 0) {
// 				lseek(eDev_flash_read[i_flash_read].fd, eDev_flash_read[i_flash_read].offset, SEEK_SET);
// 				for(count_flash_read=0; count_flash_read<eDev_flash_read[i_flash_read].length; count_flash_read+=sizeof(buf_flash_read)) {
// 					read(eDev_flash_read[i_flash_read].fd, buf_flash_read, sizeof(buf_flash_read));
// 					write(usrFd_flash_read, buf_flash_read, sizeof(buf_flash_read));
// 				}
// 			}
// 		}
// 		ret_flash_read = 0;
// 	} while(0);

// 	if(usrFd_flash_read >= 0)
// 		close(usrFd_flash_read);
// 	if(eDev_flash_read[0].fd >= 0)
// 		close(eDev_flash_read[0].fd);
// 	if(eDev_flash_read[1].fd >= 0)
// 		close(eDev_flash_read[1].fd);

// 	return ret_flash_read;
// }

// int WriteToFileEvent(char *filename, char *NewEvent)
// {

// 	//WriteToFileEvent()
// 	FILE *fp_WriteToFileEvent;
// 	int Rows_WriteToFileEvent=0;    
// 	char webapibuf_WriteToFileEvent[DATA_COUNT][DATA_SIZE];
// 	int i_WriteToFileEvent;
// 	//WriteToFileEvent()
// 	printf("WriteToFileEvent function\n");
    
//     for (i_WriteToFileEvent=0;i_WriteToFileEvent<DATA_COUNT;i_WriteToFileEvent++){
//         memset(webapibuf_WriteToFileEvent[i_WriteToFileEvent],0,DATA_SIZE);
//     }
//     printf("1filename=%s\n", filename);
//     if (NULL == (fp_WriteToFileEvent = fopen(filename, "r"))){
//         printf("no file,Create New file %s\n",filename);
//         fp_WriteToFileEvent = fopen(filename, "w");       
//     }

//     if (fp_WriteToFileEvent != NULL){
//         Rows_WriteToFileEvent = 0;
//         while (fgets(webapibuf_WriteToFileEvent[Rows_WriteToFileEvent], sizeof(webapibuf_WriteToFileEvent[Rows_WriteToFileEvent]), fp_WriteToFileEvent) != NULL){
//             printf("%d)%s))\n",Rows_WriteToFileEvent,webapibuf_WriteToFileEvent[Rows_WriteToFileEvent]); //#讀出每一行
//             Rows_WriteToFileEvent++;
//             if(Rows_WriteToFileEvent >= DATA_COUNT){          
//                 for (Rows_WriteToFileEvent=0;Rows_WriteToFileEvent<(DATA_COUNT-1);Rows_WriteToFileEvent++){
//                     strcpy(webapibuf_WriteToFileEvent[Rows_WriteToFileEvent],webapibuf_WriteToFileEvent[Rows_WriteToFileEvent+1]);
//                 }
//             }
//         }
//         printf("\n Rows=%d \n", Rows_WriteToFileEvent); //# 總共有幾筆
//     }
//     // fclose(fp);
//     strcpy(webapibuf_WriteToFileEvent[Rows_WriteToFileEvent], NewEvent);
// 	printf("%s",NewEvent);
//     fp_WriteToFileEvent = fopen(filename, "w+"); //# 把檔案重寫
//     for (Rows_WriteToFileEvent=0;Rows_WriteToFileEvent<DATA_COUNT;Rows_WriteToFileEvent++){
//         fwrite(webapibuf_WriteToFileEvent[Rows_WriteToFileEvent],strlen(webapibuf_WriteToFileEvent[Rows_WriteToFileEvent]),1,fp_WriteToFileEvent);
//     }
// 	printf("\n The NEW total Rows=%d \n", Rows_WriteToFileEvent); //# 總共有幾筆
// 	usleep(100);
// 	fclose(fp_WriteToFileEvent);
// 	printf("sks_event.txt file write over\n");
// 	sync();	

//     return 0;    
// }

int WriteToFileSys(char *filename, char *NewEvent);
int check_ps_log(char *filename,char *check_str,int count);

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

// size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata) {
//     // 這是一個回調函數，用於處理下載的數據
//     // 在這個示例中，我們只是將數據輸出到控制台
//     fwrite(ptr, size, nmemb, stdout);
//     return size * nmemb;
// }

int SKSPushApi(){

	CURL        *curl_SKSPushApi;
	CURLcode    res_SKSPushApi;
	char pushapi_response_message_SKSPushApi[128];
	// char uis_temp_data[512];
	struct memory chunk_SKSPushApi;	
	// if (Debug_Socket_server_webapi_String) 
	struct curl_slist *headers_SKSPushApi = NULL;
	int i_SKSPushApi;
	long response_code1_SKSPushApi;
	long response_code2_SKSPushApi;
	int res1_SKSPushApi; 
	char url_string[1024];
	char data_string[1024];
	char data_array[20][128];
	char *p ;
	const char *d = "=&" ;
	int i=0,j;
	char temp[2]={0x31,0};
	chunk_SKSPushApi.response=NULL;
	chunk_SKSPushApi.size=0;
    
	curl_global_init(CURL_GLOBAL_ALL);
    curl_SKSPushApi        = curl_easy_init();

	CURL *curl = curl_easy_init();
		// printf("****************SKSWebApi urlstr=\n(%s)\n",WebApiinfo->urlstr);
    if (curl_SKSPushApi){
		// WebApiinfo->Curl_exeic=true;
		
		curl_easy_setopt(curl_SKSPushApi, CURLOPT_CUSTOMREQUEST, "POST");
		strcpy(data_string,PushApiinfo.datastr);
		p = strtok(data_string, d);
		while (p != NULL) {
			strcpy(data_array[i],p);
			if (DEBUG_PRINTF)
				printf("%d)%d)%s\n",i,(int)strlen(data_array[i]) ,data_array[i]);	
				if (i==7){
					strcpy(PushApiinfo.pushkey,data_array[i]);
				}		
			i++;
			p = strtok(NULL, d);		   
		}
		memset(data_string,0,1024);
		
		strcpy(PushApiinfo.GROUP_STRING,data_array[5]);
		strcpy(PushApiinfo.GW_STRING,data_array[9]);		
		
		if(curl) {
			for (j=0;j<i;j++){
				char *output = curl_easy_escape(curl, data_array[j], strlen(data_array[j]));
				if(output) {
					if (DEBUG_PRINTF)
						printf("%d)Encoded: %s\n",j,output);
					strcat(data_string,output);
					curl_free(output);
				}
				if(j%2==0){
					
					temp[0]='=';
					// printf("%s",temp);
					strcat(data_string,temp);
				
				}else if(j%2==1){
					temp[0]='&';
					// printf("%s",temp);
					strcat(data_string,temp);
				}
			}
			data_string[strlen(data_string)-1]=0;
			printf("data==>%s)",data_string);
			curl_easy_cleanup(curl);
		}
		

		sprintf(url_string,"%s?%s",PushApiinfo.urlstr,data_string);
		if (DEBUG_PRINTF)
			printf("\npush urlstring=>%s)\n",url_string);
        curl_easy_setopt(curl_SKSPushApi, CURLOPT_URL, url_string);        //URL parameter

		headers_SKSPushApi = NULL;
		headers_SKSPushApi = curl_slist_append(headers_SKSPushApi, "postman-token: 59388f84-9771-32de-ae0f-f5234a0ea690");
		headers_SKSPushApi = curl_slist_append(headers_SKSPushApi, "cache-control: no-cache");
		// if (strcmp(WebApiinfo.methodstr,"GET")){
		// 	headers_SKSWebApi = curl_slist_append(headers_SKSWebApi, "content-type: application/json");
		// 	printf("%s Method\n",WebApiinfo.methodstr);
		// } else{
		// 	printf("GET Method\n");
		// }
		
		// curl_easy_setopt(curl_SKSPushApi, CURLOPT_CUSTOMREQUEST, );
		curl_easy_setopt(curl_SKSPushApi, CURLOPT_HTTPHEADER, headers_SKSPushApi);

        curl_easy_setopt(curl_SKSPushApi, CURLOPT_FOLLOWLOCATION, 1L); // redirection
        curl_easy_setopt(curl_SKSPushApi, CURLOPT_WRITEFUNCTION, cb);	/* Function Pointer "writer" manages the required buffer size */
	    curl_easy_setopt(curl_SKSPushApi, CURLOPT_WRITEDATA, (void *)&chunk_SKSPushApi); /* Data Pointer &buffer stores downloaded web content */
				
		// 設置超時時間為 10 秒
        curl_easy_setopt(curl_SKSPushApi, CURLOPT_TIMEOUT, 10L);
        // 設置失敗時不返回 HTTP 錯誤響應
        curl_easy_setopt(curl_SKSPushApi, CURLOPT_FAILONERROR, 1L);
        // 設置數據回調函數
        // curl_easy_setopt(curl_SKSPushApi, CURLOPT_WRITEFUNCTION, write_callback);

        // if (WebApiinfo.ssl_bool){
        //     curl_easy_setopt(curl_SKSWebApi, CURLOPT_SSL_VERIFYHOST, 0);  //  SSL peer certificate
        //     curl_easy_setopt(curl_SKSWebApi, CURLOPT_SSL_VERIFYPEER, 0);
        // }
		// curl_easy_setopt(curl, CURLOPT_POSTFIELDS, WebApiDataInfo.string_json_data);
		// curl_easy_setopt(curl_SKSPushApi, CURLOPT_POSTFIELDS, WebApiinfo.jsonstr);
		// if (Debug_Socket_server_webapi_String|Debug_file_data_show) 
		// 	printf("**********SKSWebApi input json data WebApiinfo->jsonstr==>\n(%s)\n",WebApiinfo.jsonstr);
		// printf( WebApiinfo->inputstr);
		
		
		for (i_SKSPushApi=0;i_SKSPushApi<WEBAPI_RETRY;i_SKSPushApi++){
			
			res_SKSPushApi = curl_easy_perform(curl_SKSPushApi);                      // default get
			if (Debug_Socket_server_webapi_String|Debug_file_data_show) 
				if (DEBUG_PRINTF)
					printf("***********************SKSWebApi curl_easy_perform() exec after input json data PushApiinfo->jsonstr==>\n(%s)\n",PushApiinfo.datastr);
			if (chunk_SKSPushApi.size){
				// WebApiinfo.outputstr=chunk_SKSWebApi.response;
				strcpy(PushApiinfo.outputstr,chunk_SKSPushApi.response);
				if (DEBUG_PRINTF)
					printf("Response output data==>%s\n",PushApiinfo.outputstr);  //******************
			} 
			else{
				if (DEBUG_PRINTF)
					printf("No output!\n");
			}
			
			if(res_SKSPushApi != CURLE_OK){
				sleep(1);
				if (DEBUG_PRINTF)
					printf("CURL RETRY index=%d,curl_easy_perform continue(res=%d)!\n",i_SKSPushApi,res_SKSPushApi);
				continue;
			}
			else{
				if (DEBUG_PRINTF)
					printf("CURLE_OK index=%d,curl_easy_perform break(res=%d)!\n",i_SKSPushApi,res_SKSPushApi);
				break;
			}	
			usleep(50);
		}

		if (i_SKSPushApi==WEBAPI_RETRY){//webapi send fail
			if (DEBUG_PRINTF)
				printf("Pushapi retry %d time fail!curl=(%d)\n",i_SKSPushApi,res_SKSPushApi);			
			sprintf(pushapi_response_message_SKSPushApi,"%s_(res=%d)",curl_easy_strerror(res_SKSPushApi),res_SKSPushApi);
			if (DEBUG_PRINTF)
				printf("push fail response message==>%s\n",pushapi_response_message_SKSPushApi);
			strcpy(PushApiinfo.response_message,pushapi_response_message_SKSPushApi);

			
			fprintf(stderr, "%s(res=%d) !\n",curl_easy_strerror(res_SKSPushApi),res_SKSPushApi);
			if (DEBUG_PRINTF)
				printf("PushApiDataInfo==>%s\n",PushApiinfo.response_message);
			// WebApiinfo.webapi_send_fail=true;
			return res_SKSPushApi;

		}
		else{		
			
            curl_easy_getinfo(curl_SKSPushApi, CURLINFO_RESPONSE_CODE, &response_code1_SKSPushApi);
			PushApiinfo.response_code = response_code1_SKSPushApi;
			// if (Debug_Socket_server_webapi_String|Debug_file_data_show) 
            // 	printf("\n\n***********************SKSWebApi ==>CURLINFO_RESPONSE_CODE status code (%03ld)\n*******************PushApiinfo->jsonstr=%s\n",PushApiinfo.response_code,PushApiinfo.jsonstr);
			if (PushApiinfo.response_code==200){
				sprintf(pushapi_response_message_SKSPushApi,"%s(%ld)","At.",PushApiinfo.response_code);				
			}else if (PushApiinfo.response_code==201){
				sprintf(pushapi_response_message_SKSPushApi,"%s(%ld)","Cted.",PushApiinfo.response_code);
			}else if (PushApiinfo.response_code==204){
				sprintf(pushapi_response_message_SKSPushApi,"%s(%ld)","No_content",PushApiinfo.response_code);
			}else if (PushApiinfo.response_code==400){
				sprintf(pushapi_response_message_SKSPushApi,"%s(%ld)","Bad_Request",PushApiinfo.response_code);
			}else if (PushApiinfo.response_code==404){
				sprintf(pushapi_response_message_SKSPushApi,"%s(%ld)","Not_Fount",PushApiinfo.response_code);
			}else if (PushApiinfo.response_code==403){
				sprintf(pushapi_response_message_SKSPushApi,"%s(%ld)","Forbidden",PushApiinfo.response_code);
			}else if (PushApiinfo.response_code==409){
				sprintf(pushapi_response_message_SKSPushApi,"%s(%ld)","Conflict",PushApiinfo.response_code);
			}else if (PushApiinfo.response_code==500){
				sprintf(pushapi_response_message_SKSPushApi,"%s(%ld)","Server_Error",PushApiinfo.response_code);
			}else{
				sprintf(pushapi_response_message_SKSPushApi,"%s(%ld)","Other_Condition",PushApiinfo.response_code);
			}

			strcpy(PushApiinfo.response_message,pushapi_response_message_SKSPushApi);
			// if (Debug_Socket_server_webapi_String) 
			// if (Debug_Socket_server_webapi_String|Debug_file_data_show) 
			// 	printf("\n\n***********************SKSWebApi CURLE_OK_resprone=>%s\n*******************PushApiinfo->jsonstr=%s\n",PushApiinfo.response_message,PushApiinfo.jsonstr);				
            res1_SKSPushApi = curl_easy_getinfo(curl_SKSPushApi, CURLINFO_HTTP_CONNECTCODE, &response_code2_SKSPushApi);
            // if ((!res1_SKSPushApi)&response_code2_SKSPushApi) printf("\n *************8CURLINFO_HTTP_CONNECTCODE status code******************** ==>%03ld,ret==>%3d\n",response_code2_SKSWebApi,res1_SKSWebApi);            
			// PushApiinfo.webapi_send_fail=false;
		}

		curl_slist_free_all(headers_SKSPushApi);
		curl_easy_cleanup(curl_SKSPushApi);
		curl_global_cleanup();	
		// PushApiinfo->Curl_exeic=false;
		// free(chunk_SKSPushApi.response);
		return res_SKSPushApi;
        
    }else{
		if (DEBUG_PRINTF)
        	printf("curl_easy_init fail\n\n");
		curl_global_cleanup();
		
		// WebApiinfo->Curl_exeic=false;
        return -1;
    }
	
}


int SKSWebApi(){

	CURL        *curl_SKSWebApi;
	CURLcode    res_SKSWebApi;
	char webapi_response_message_SKSWebApi[128];
	// char uis_temp_data[512];
	struct memory chunk_SKSWebApi;// = {0};	
	// if (Debug_Socket_server_webapi_String) 
	struct curl_slist *headers_SKSWebApi = NULL;
	int i_SKSWebApi;
	long response_code1_SKSWebApi;
	long response_code2_SKSWebApi;
	int res1_SKSWebApi; 

	const char *pfx_path = "./abc.pfx";
    const char *pfx_password = "12345678";
    const char *cacert_path = "./CA_list.pem";
	
	chunk_SKSWebApi.response = NULL;
	chunk_SKSWebApi.size =0 ;
    
	curl_global_init(CURL_GLOBAL_ALL);
    curl_SKSWebApi        = curl_easy_init();
		// printf("****************SKSWebApi urlstr=\n(%s)\n",WebApiinfo->urlstr);
    if (curl_SKSWebApi){
		// WebApiinfo->Curl_exeic=true;
		curl_easy_setopt(curl_SKSWebApi, CURLOPT_CUSTOMREQUEST, WebApiinfo.methodstr);
		
        curl_easy_setopt(curl_SKSWebApi, CURLOPT_URL, WebApiinfo.urlstr);        //URL parameter
		
		// 設置超時時間為 20 秒
        curl_easy_setopt(curl_SKSWebApi, CURLOPT_TIMEOUT, 20L);
        // 設置失敗時不返回 HTTP 錯誤響應
        curl_easy_setopt(curl_SKSWebApi, CURLOPT_FAILONERROR, 1L);

		headers_SKSWebApi = NULL;
		headers_SKSWebApi = curl_slist_append(headers_SKSWebApi, "postman-token: 59388f84-9771-32de-ae0f-f5234a0ea690");
		headers_SKSWebApi = curl_slist_append(headers_SKSWebApi, "cache-control: no-cache");
		if (strcmp(WebApiinfo.methodstr,"GET")){
			headers_SKSWebApi = curl_slist_append(headers_SKSWebApi, "content-type: application/json");
			if (DEBUG_PRINTF)
				printf("%s Method\n",WebApiinfo.methodstr);
		} else{
			if (DEBUG_PRINTF)
				printf("GET Method\n");
		} 
		curl_easy_setopt(curl_SKSWebApi, CURLOPT_HTTPHEADER, headers_SKSWebApi);

        curl_easy_setopt(curl_SKSWebApi, CURLOPT_FOLLOWLOCATION, 1L); // redirection
        curl_easy_setopt(curl_SKSWebApi, CURLOPT_WRITEFUNCTION, cb);	/* Function Pointer "writer" manages the required buffer size */
	    curl_easy_setopt(curl_SKSWebApi, CURLOPT_WRITEDATA, (void *)&chunk_SKSWebApi); /* Data Pointer &buffer stores downloaded web content */
        if (WebApiinfo.ssl_bool){

			if (CA_ENABLE){//CA_ENABLE
				// Set the PFX certificate and password
				curl_easy_setopt(curl_SKSWebApi, CURLOPT_SSLCERTTYPE, "P12");
				curl_easy_setopt(curl_SKSWebApi, CURLOPT_SSLCERT, pfx_path);
				curl_easy_setopt(curl_SKSWebApi, CURLOPT_KEYPASSWD, pfx_password);

				// Set the CA certificate
				curl_easy_setopt(curl_SKSWebApi, CURLOPT_CAINFO, cacert_path);
			}
			else{
				curl_easy_setopt(curl_SKSWebApi, CURLOPT_SSL_VERIFYHOST, 0);  //  SSL peer certificate
            	curl_easy_setopt(curl_SKSWebApi, CURLOPT_SSL_VERIFYPEER, 0);

			}

            
        }
		// curl_easy_setopt(curl, CURLOPT_POSTFIELDS, WebApiDataInfo.string_json_data);
		curl_easy_setopt(curl_SKSWebApi, CURLOPT_POSTFIELDS, WebApiinfo.jsonstr);
		if (DEBUG_PRINTF)
			if (Debug_Socket_server_webapi_String|Debug_file_data_show)
				printf("**********SKSWebApi input json data WebApiinfo->jsonstr==>\n(%s)\n",WebApiinfo.jsonstr);
		// printf( WebApiinfo->inputstr);
		
		
		for (i_SKSWebApi=0;i_SKSWebApi<WEBAPI_RETRY;i_SKSWebApi++){
			if (DEBUG_PRINTF)
				if (Debug_Socket_server_webapi_String|Debug_file_data_show) 
					printf("\n\n***********************SKSWebApi curl_easy_perform() exec before input json data WebApiinfo->jsonstr==>\n(%s)\n",WebApiinfo.jsonstr);
			res_SKSWebApi = curl_easy_perform(curl_SKSWebApi);
			if (DEBUG_PRINTF)                      // default get
				if (Debug_Socket_server_webapi_String|Debug_file_data_show) 
					printf("***********************SKSWebApi curl_easy_perform() exec after input json data WebApiinfo->jsonstr==>\n(%s)\n",WebApiinfo.jsonstr);
			
			if (chunk_SKSWebApi.size){
				strcpy(WebApiinfo.outputstr,chunk_SKSWebApi.response);
				if (DEBUG_PRINTF)
					printf("Response output data==>%s\n",WebApiinfo.outputstr);  //******************
			} 
			else{
				if (DEBUG_PRINTF)
					printf("No output!\n");
			}

			if(res_SKSWebApi != CURLE_OK){
				if (DEBUG_PRINTF)
					printf("CURL RETRY index=%d,curl_easy_perform continue(res=%d)!\n",i_SKSWebApi,res_SKSWebApi);
				continue;
			}
			else{
				if (DEBUG_PRINTF)
					printf("CURLE_OK index=%d,curl_easy_perform break(res=%d)!\n",i_SKSWebApi,res_SKSWebApi);
				break;
			}
			usleep(50);	
		}
		if (i_SKSWebApi==WEBAPI_RETRY){//webapi send fail
			if (DEBUG_PRINTF)
				printf("Webapi retry %d time fail!curl=(%d)\n",i_SKSWebApi,res_SKSWebApi);			
			sprintf(webapi_response_message_SKSWebApi,"%s_(res=%d)",curl_easy_strerror(res_SKSWebApi),res_SKSWebApi);
			strcpy(WebApiinfo.response_message,webapi_response_message_SKSWebApi);

			
			fprintf(stderr, "%s(res=%d) !\n",curl_easy_strerror(res_SKSWebApi),res_SKSWebApi);
			if (DEBUG_PRINTF)
				printf("WebApiDataInfo==>%s\n",WebApiinfo.response_message);
			WebApiinfo.webapi_send_fail=true;
			curl_slist_free_all(headers_SKSWebApi);		
			curl_easy_cleanup(curl_SKSWebApi);
			curl_global_cleanup();
			return res_SKSWebApi;
		}
		else{		
			
            curl_easy_getinfo(curl_SKSWebApi, CURLINFO_RESPONSE_CODE, &response_code1_SKSWebApi);
			WebApiinfo.response_code = response_code1_SKSWebApi;
			if (DEBUG_PRINTF)
				if (Debug_Socket_server_webapi_String|Debug_file_data_show) 
					printf("***********************SKSWebApi ==>CURLINFO_RESPONSE_CODE status code (%03ld)\n*******************WebApiinfo->jsonstr=%s\n",WebApiinfo.response_code,WebApiinfo.jsonstr);
			if (WebApiinfo.response_code==200){
				sprintf(webapi_response_message_SKSWebApi,"%s%ld","At.",WebApiinfo.response_code);				
			}else if (WebApiinfo.response_code==201){
				sprintf(webapi_response_message_SKSWebApi,"%s%ld","Cted.",WebApiinfo.response_code);
			}else if (WebApiinfo.response_code==204){
				sprintf(webapi_response_message_SKSWebApi,"%s%ld","No_content",WebApiinfo.response_code);
			}else if (WebApiinfo.response_code==400){
				sprintf(webapi_response_message_SKSWebApi,"%s%ld","Bad_Request",WebApiinfo.response_code);
			}else if (WebApiinfo.response_code==404){
				sprintf(webapi_response_message_SKSWebApi,"%s%ld","Not_Fount",WebApiinfo.response_code);
			}else if (WebApiinfo.response_code==403){
				sprintf(webapi_response_message_SKSWebApi,"%s%ld","Forbidden",WebApiinfo.response_code);
			}else if (WebApiinfo.response_code==409){
				sprintf(webapi_response_message_SKSWebApi,"%s%ld","Conflict",WebApiinfo.response_code);
			}else if (WebApiinfo.response_code==500){
				sprintf(webapi_response_message_SKSWebApi,"%s%ld","Server_Error",WebApiinfo.response_code);
			}else{
				sprintf(webapi_response_message_SKSWebApi,"%s%ld","Other_Condition",WebApiinfo.response_code);
			}

			strcpy(WebApiinfo.response_message,webapi_response_message_SKSWebApi);
			// if (Debug_Socket_server_webapi_String) 
			if (DEBUG_PRINTF)
				if (Debug_Socket_server_webapi_String|Debug_file_data_show) 
					printf("***********************SKSWebApi CURLE_OK_resprone=>%s\n*******************WebApiinfo->jsonstr=%s\n",WebApiinfo.response_message,WebApiinfo.jsonstr);				
            
			res1_SKSWebApi = curl_easy_getinfo(curl_SKSWebApi, CURLINFO_HTTP_CONNECTCODE, &response_code2_SKSWebApi);
            
			if ((!res1_SKSWebApi)&response_code2_SKSWebApi)
				if (DEBUG_PRINTF) 
					printf("*************8CURLINFO_HTTP_CONNECTCODE status code******************** ==>%03ld,ret==>%3d\n\n",response_code2_SKSWebApi,res1_SKSWebApi);            
			
			WebApiinfo.webapi_send_fail=false;
		}

		curl_slist_free_all(headers_SKSWebApi);
		
		curl_easy_cleanup(curl_SKSWebApi);
		curl_global_cleanup();	
		// WebApiinfo->Curl_exeic=false;
		// free(chunk_SKSWebApi.response);
		return res_SKSWebApi;
        
    }else{
		if (DEBUG_PRINTF)
        	printf("curl_easy_init fail\n\n");
		curl_global_cleanup();
		
        return -1;
    }
	
}

int ParseCurlStringPushapiExec()
{
    //ParseCurlStringWebapiExec()
	char* sectiontString1_ParseCurlStringPushapiExec       = NULL;
	char* sectiontString2_ParseCurlStringPushapiExec       = NULL;   
	char* sectiontString3_ParseCurlStringPushapiExec       = NULL;	

    if ( PushApiinfo.inputstring == NULL ){
		if (DEBUG_PRINTF)
        	printf("PUSH Inupt string NULL\n");
        return -1;
    }
    else{
		// if (DEBUG_PRINTF)
		if (Debug_Socket_server_webapi_String) 
			printf("/nParse Curl String PUSH==> %s \n",PushApiinfo.inputstring);
        sectiontString1_ParseCurlStringPushapiExec = strtok(PushApiinfo.inputstring , "\"");
		// WebApiinfo.curlcommandstr = sectiontString1_ParseCurlStringWebapiExec;
		// WebApiinfo.curlcommandstr[strlen(WebApiinfo.curlcommandstr)-1]=WebApiinfo.curlcommandstr[strlen(WebApiinfo.curlcommandstr)];
		// if (Debug_Socket_server_webapi_String) printf("Web Api Curl Command ==> (%s) \n",WebApiinfo.curlcommandstr);
		if (sectiontString1_ParseCurlStringPushapiExec==NULL){
			if (DEBUG_PRINTF)
				printf("curl command string is null\n\n");	
			return -1;
		}
		if (DEBUG_PRINTF)
			printf("\nsectiontString1_ParseCurlStringPushapiExec=>%s\n",sectiontString1_ParseCurlStringPushapiExec);
		
        sectiontString2_ParseCurlStringPushapiExec = strtok(NULL, "\"");
		
		strcpy(PushApiinfo.methodstr,"POST");

		if (sectiontString2_ParseCurlStringPushapiExec==NULL){
			if (DEBUG_PRINTF)
				printf("Push data string is null\n\n");	
			return -1;
		}
		strcpy(PushApiinfo.datastr,sectiontString2_ParseCurlStringPushapiExec);
		if (DEBUG_PRINTF)
			printf("Push data ==> (%s) \n",PushApiinfo.datastr);

		sectiontString3_ParseCurlStringPushapiExec = strtok(NULL, " ");
		// sectiontString3_ParseCurlStringPushapiExec = strtok(NULL, " ");

		if (sectiontString3_ParseCurlStringPushapiExec==NULL){
			if (DEBUG_PRINTF)
				printf("Push api string is null\n\n");	
			return -1;
		}

		if (THAI_ADDR_ENABLE){
			strcpy(PushApiinfo.urlstr,THAI_PUSH_ADDR);
		}else{
			strcpy(PushApiinfo.urlstr,sectiontString3_ParseCurlStringPushapiExec);
		}

		if (DEBUG_PRINTF)
			printf("\n\n****Push api string=>%s\n\n",PushApiinfo.urlstr);    
        
    }
	   
	return 0;
}

int ParseCurlStringWebapiExec()
{
    //ParseCurlStringWebapiExec()
	char *sectiontString1_ParseCurlStringWebapiExec       = NULL;   
	// char *sectiontString3_ParseCurlStringWebapiExec       = NULL;
	// char *sectiontString3Tmp_ParseCurlStringWebapiExec    = NULL;   
	char *string_post_ParseCurlStringWebapiExec	= "POST";
	char *string_put_ParseCurlStringWebapiExec	= "PUT";
	// char *strtok_tmp=NULL;
	char address[512];
	// char *https_str="https";
	// char *temp_str; 
	// char address[512];
	char mno_temp[10];
	

	memset(mno_temp,0,strlen(mno_temp));
	if (DEBUG_PRINTF)
		printf("\nParseCurlStringWebapiExec(\n");
    if ( WebApiinfo.inputstring == NULL ){
        printf("Inupt string NULL");
        return -1;
    }
    else{
		if (DEBUG_PRINTF) 
			printf("Parse Curl String webapi==> %s \n",WebApiinfo.inputstring);
        sectiontString1_ParseCurlStringWebapiExec = strtok(WebApiinfo.inputstring , "'");
		strcpy(WebApiinfo.curlcommandstr,sectiontString1_ParseCurlStringWebapiExec);
		// WebApiinfo.curlcommandstr = sectiontString1_ParseCurlStringWebapiExec;
		WebApiinfo.curlcommandstr[strlen(WebApiinfo.curlcommandstr)-1]=WebApiinfo.curlcommandstr[strlen(WebApiinfo.curlcommandstr)];
		WebApiinfo.ssl_bool= FLAG_SSL_BOOL;

		// if (Debug_Socket_server_webapi_String)
		if (DEBUG_PRINTF) 
			printf("Web Api Curl Command ==> (%s) \n",WebApiinfo.curlcommandstr);
		if (WebApiinfo.curlcommandstr==NULL){
			if (DEBUG_PRINTF)
				printf("curl command string is null\n\n");	
			return -1;
		}
		
        strcpy(WebApiinfo.jsonstr,strtok(NULL, "'"));

		// if (Debug_Socket_server_webapi_String) 
		if (DEBUG_PRINTF)
			printf("Json data ==> (%s) \n",WebApiinfo.jsonstr);
		if (WebApiinfo.jsonstr==NULL){
			printf("Json data string is null\n\n");	
			return -1;
		}
		// printf("agent run==>Json data ==> (%s) \n",WebApiinfo.jsonstr);	        
        // sectiontString3_ParseCurlStringWebapiExec = strtok(NULL, "'");
        // // printf("agent run==>sectiontString3 = (%s) \n",sectiontString3_ParseCurlStringWebapiExec);
        // sectiontString3Tmp_ParseCurlStringWebapiExec = strtok(NULL, " \n");
        // printf("agent run==>sectiontString3Tmp = (%s)\n",sectiontString3Tmp_ParseCurlStringWebapiExec);
		// temp_str = strtok(NULL, " ");
		// printf("\nagent temp_str Server address==>(%s)\n",temp_str);
		// printf("agent run==> show address \n");
		if (THAI_ADDR_ENABLE){
			// strcpy(WebApiinfo.urlstr,THAI_API_ADDR);
			if (DEBUG_PRINTF)
				printf("show mmo=>(%s)\n",WebApiinfo.apmib_mno);
			if (DEBUG_PRINTF)
				printf("mno_temp=>(%s)\n",mno_temp);
			memset(mno_temp,0,10);
			strncpy(mno_temp,WebApiinfo.apmib_mno,3);
			mno_temp[3]=0;	
			
			if (DEBUG_PRINTF)
				printf("show address rrrr mmo=>%s \n",mno_temp);
			
			sprintf(address, "https://receiver%3s.skscloudsecurity.com/signal", mno_temp);
			strcpy(WebApiinfo.urlstr,address);
			if (DEBUG_PRINTF)
				printf("data Server address==>%s\n\n",WebApiinfo.urlstr);
						
		}else{	
			if (DEBUG_PRINTF)
				printf("show mmo=>(%s)\n",WebApiinfo.apmib_mno);
			if (DEBUG_PRINTF)
				printf("mno_temp=>(%s)\n",mno_temp);
			memset(mno_temp,0,10);
			strncpy(mno_temp,WebApiinfo.apmib_mno,3);
			mno_temp[3]=0;	
			if (DEBUG_PRINTF)
				printf("show address sks mmo=>(%s)\n",mno_temp);			
			// https://wlsecurity069.sks.com.tw/uisgw/api/receive
			// https://wlsecurity069.sks.com.tw/uisgw/api/receive
			// sprintf(address,"https://wlsecurity069.sks.com.tw/uisgw/api/receive");
			sprintf(address,"%s%s%s","https://wlsecurity",mno_temp,".sks.com.tw/uisgw/api/receive");
			if (DEBUG_PRINTF)
				printf("address data Server address==>(%s)\n",address);
			strcpy(WebApiinfo.urlstr,address);
			if (DEBUG_PRINTF)
				printf("WebApiinfo.urlstr data Server address==>%s\n",WebApiinfo.urlstr);
			

			// strtok(temp_str,":");
			// strtok_tmp = strtok(NULL,":");
			// sprintf(address,"%s:%s",https_str,strtok_tmp);
			// strcpy(WebApiinfo.urlstr,address);
			// // WebApiinfo.urlstr= strtok(sectiontString3Tmp_ParseCurlStringWebapiExec, " ");
			// printf("\nagent run Sks data Server address==>(%s)\n\n",WebApiinfo.urlstr);
		}
		if (DEBUG_PRINTF)
			printf("json=>%s\n",WebApiinfo.jsonstr);
        // WebApiinfo.urlstr = strtok(sectiontString3Tmp_ParseCurlStringWebapiExec, " ");
		if (DEBUG_PRINTF)
			printf("agent run==>*****urlstr ==> (%s)\n",WebApiinfo.urlstr);
		if (WebApiinfo.urlstr==NULL){
			if (DEBUG_PRINTF)	
				printf("Webapi string is null\n\n");
			return -1;
		}    
    }
	

	if(strstr(sectiontString1_ParseCurlStringWebapiExec, string_post_ParseCurlStringWebapiExec)){
		strcpy(WebApiinfo.methodstr,string_post_ParseCurlStringWebapiExec);
		// WebApiinfo.methodstr = string_post_ParseCurlStringWebapiExec;
		// if (ShowDataMassage)
		if (DEBUG_PRINTF) 
			printf("%s Method!\n\n",WebApiinfo.methodstr);

	}else if(strstr(sectiontString1_ParseCurlStringWebapiExec, string_put_ParseCurlStringWebapiExec)){
		strcpy(WebApiinfo.methodstr,string_put_ParseCurlStringWebapiExec);
		// WebApiinfo.methodstr = string_put_ParseCurlStringWebapiExec;
		// if (ShowDataMassage) 
		if (DEBUG_PRINTF)
			printf("agent run==>%s Method!\n\n",WebApiinfo.methodstr);
	}else {		
		// if (ShowDataMassage) 
		if (DEBUG_PRINTF)
			printf("agent run==>other Method!(%s)\n\n",sectiontString1_ParseCurlStringWebapiExec);
	}    
	return 0;
}

// int WriteToDayLogFile(char *filename, char *NewEvent)
// {
// 	FILE *fp_WriteToDayLogFile;
// 	char dayname_WriteToDayLogFile[50];
// 	char tempname_WriteToDayLogFile[50];
// 	char NewEventWithTime_WriteToDayLogFile[DATA_SIZE];
// 	//WriteToDayLogFile()

// 	memset(dayname_WriteToDayLogFile,0,50);
// 	memset(NewEventWithTime_WriteToDayLogFile,0,DATA_SIZE);
// 	sprintf(dayname_WriteToDayLogFile,"%s","/tmp/usb/sda1/dailylog/");
// 	strncpy(tempname_WriteToDayLogFile,filename,(strlen(filename)-1));
// 	tempname_WriteToDayLogFile[strlen(filename)-1]=0;
// 	strcat(dayname_WriteToDayLogFile,tempname_WriteToDayLogFile);
// 	strcat(dayname_WriteToDayLogFile,"DayLog"); 
// 	fp_WriteToDayLogFile = fopen(dayname_WriteToDayLogFile, "a");
// 	sprintf(NewEventWithTime_WriteToDayLogFile,"%s %s",TimeFormat(),NewEvent);
// 	if (Debug_file_data_show)
// 		printf("WriteToDayLogFile(%s)",NewEventWithTime_WriteToDayLogFile);
// 	fwrite(NewEventWithTime_WriteToDayLogFile,strlen(NewEventWithTime_WriteToDayLogFile),1,fp_WriteToDayLogFile);
// 	fclose(fp_WriteToDayLogFile);	
// 	sync();
//     return 0;        
// }

// int WriteToPushFile(char *NewEvent)
// {

// 	FILE *fp_WriteToDayRunFile;
// 	char dayname_WriteToDayRunFile[50];
// 	//WriteToDayRunFile()

//     memset(dayname_WriteToDayRunFile,0,50);
// 	sprintf(dayname_WriteToDayRunFile,"%s",PUSHDATA_FILE);	 
// 	// strcat(dayname_WriteToDayRunFile,"DayRun"); 
// 	if (Debug_file_data_show)
// 		printf("write (%s) to dayRun file \n\n",NewEvent);
	
// 	fp_WriteToDayRunFile = fopen(dayname_WriteToDayRunFile, "a");
// 	fwrite(NewEvent,strlen(NewEvent),1,fp_WriteToDayRunFile);
// 	fclose(fp_WriteToDayRunFile);	
// 	sync();
//     return 0;    
// }


int WriteToDayRunFile(char *NewEvent, char *filename)
{

	FILE *fp_WriteToDayRunFile;
	
	if (DEBUG_PRINTF) 
		if (Debug_file_data_show)
			printf("write (%s) to %s file \n\n",NewEvent,filename);
	
	fp_WriteToDayRunFile = fopen(filename, "a");
	fwrite(NewEvent,strlen(NewEvent),1,fp_WriteToDayRunFile);
	fclose(fp_WriteToDayRunFile);	
	sync();
    return 0;    
}

// int WriteToDayRunPauseFile(char *filename, char *NewEvent1, char *NewEvent2, char *NewEvent3, char *Reason)
// {
// 	FILE *fp_WriteToDayRunPauseFile;    
// 	int i_WriteToDayRunPauseFile;	
// 	char dayname_WriteToDayRunPauseFile[50];
// 	char tempname_WriteToDayRunPauseFile[50];
// 	char AllNewEvent_WriteToDayRunPauseFile[DATA_SIZE];
// 	char NewEventWithTime_WriteToDayRunPauseFile[DATA_SIZE];
// 	char DayRunDataArray_WriteToDayRunPauseFile[BUFFER_CURL_JSON_WEBAPI_SIZE][DATA_SIZE];
// 	//WriteToDayRunPauseFile()
    
// 	memset(dayname_WriteToDayRunPauseFile,0,50);
// 	memset(NewEventWithTime_WriteToDayRunPauseFile,0,DATA_SIZE);
// 	for (i_WriteToDayRunPauseFile=0;i_WriteToDayRunPauseFile<BUFFER_CURL_JSON_WEBAPI_SIZE;i_WriteToDayRunPauseFile++){
//         memset(DayRunDataArray_WriteToDayRunPauseFile[i_WriteToDayRunPauseFile],0,DATA_SIZE);
//     }
	
// 	sprintf(dayname_WriteToDayRunPauseFile,"%s","/tmp/usb/sda1/dailylog/");	
// 	strncpy(tempname_WriteToDayRunPauseFile,filename,(strlen(filename)-1));	
// 	tempname_WriteToDayRunPauseFile[strlen(filename)-1]=0;	
// 	strcat(dayname_WriteToDayRunPauseFile,tempname_WriteToDayRunPauseFile);	 
// 	strcat(dayname_WriteToDayRunPauseFile,"RunFail"); 
	
// 	fp_WriteToDayRunPauseFile = fopen(dayname_WriteToDayRunPauseFile, "a");
// 	sprintf(AllNewEvent_WriteToDayRunPauseFile,"%s '%s' %s",NewEvent1,NewEvent2,NewEvent3);
// 	sprintf(NewEventWithTime_WriteToDayRunPauseFile,"%s %s %s",TimeFormat(),Reason,AllNewEvent_WriteToDayRunPauseFile);
// 	strcat(NewEventWithTime_WriteToDayRunPauseFile,"\n");
// 	printf("WriteToDayRunPauseFile(%s)",NewEventWithTime_WriteToDayRunPauseFile);
// 	fwrite(NewEventWithTime_WriteToDayRunPauseFile,strlen(NewEventWithTime_WriteToDayRunPauseFile),1,fp_WriteToDayRunPauseFile);
// 	fclose(fp_WriteToDayRunPauseFile);	
// 	sync();
	
//     return 0;    
// }

// int WriteToDayRunOverFile_b(char *filename, char *NewEvent1, char *NewEvent2,  char *NewEvent3, char *Reason)
// {
// 	FILE *fp_WriteToDayRunOverFile, *fp0_WriteToDayRunOverFile;
// 	int Rows_WriteToDayRunOverFile=0;    
// 	// char day_run_api_array[BUFFER_CURL_JSON_WEBAPI_SIZE][DATA_SIZE];
// 	int i_WriteToDayRunOverFile,j_WriteToDayRunOverFile;
// 	// char tmpfilename[50];
// 	// char delfilename[50];
// 	char dayname_WriteToDayRunOverFile[50];
// 	char tempname_WriteToDayRunOverFile[50];
// 	char AllNewEvent_WriteToDayRunOverFile[DATA_SIZE];
// 	char NewEventWithTime_WriteToDayRunOverFile[DATA_SIZE];
// 	char DayRunDataArray_WriteToDayRunOverFile[BUFFER_CURL_JSON_WEBAPI_SIZE][DATA_SIZE];
// 	//WriteToDayRunOverFile()


    
// 	memset(dayname_WriteToDayRunOverFile,0,50);
// 	memset(NewEventWithTime_WriteToDayRunOverFile,0,DATA_SIZE);
// 	for (i_WriteToDayRunOverFile=0;i_WriteToDayRunOverFile<BUFFER_CURL_JSON_WEBAPI_SIZE;i_WriteToDayRunOverFile++){
//         memset(DayRunDataArray_WriteToDayRunOverFile[i_WriteToDayRunOverFile],0,DATA_SIZE);
//     }	
// 	sprintf(dayname_WriteToDayRunOverFile,"%s","/tmp/usb/sda1/dailylog/");	
// 	strncpy(tempname_WriteToDayRunOverFile,filename,(strlen(filename)-1));	
// 	tempname_WriteToDayRunOverFile[strlen(filename)-1]=0;	
// 	strcat(dayname_WriteToDayRunOverFile,tempname_WriteToDayRunOverFile);
// 	strcat(dayname_WriteToDayRunOverFile,"RunOver");	
// 	sprintf(AllNewEvent_WriteToDayRunOverFile,"%s '%s' %s",NewEvent1,NewEvent2,NewEvent3);
// 	if (Debug_file_data_show)
// 		printf("\n(1AllNewEvent=%s)\n",AllNewEvent_WriteToDayRunOverFile);	

// 	fp_WriteToDayRunOverFile = fopen("/tmp/usb/sda1/dailylog/DayRun", "r");
// 	while (fgets(DayRunDataArray_WriteToDayRunOverFile[Rows_WriteToDayRunOverFile], sizeof(DayRunDataArray_WriteToDayRunOverFile[Rows_WriteToDayRunOverFile]), fp_WriteToDayRunOverFile) != NULL){
// 		// printf("%d)###(%s)\n",Rows,DayRunDataArray[Rows]); //#讀出每一行
// 		Rows_WriteToDayRunOverFile++;  
// 	}
// 	fclose(fp_WriteToDayRunOverFile);
// 	for (i_WriteToDayRunOverFile=0;i_WriteToDayRunOverFile<Rows_WriteToDayRunOverFile;i_WriteToDayRunOverFile++){
// 		if (Debug_file_data_show)
// 			printf("%d)****strcmp\n(%s)\n,\n(%s)\n",i_WriteToDayRunOverFile,DayRunDataArray_WriteToDayRunOverFile[i_WriteToDayRunOverFile],AllNewEvent_WriteToDayRunOverFile);
// 		if (strncmp(DayRunDataArray_WriteToDayRunOverFile[i_WriteToDayRunOverFile],AllNewEvent_WriteToDayRunOverFile,250)==0){
// 			fp0_WriteToDayRunOverFile = fopen(dayname_WriteToDayRunOverFile, "a");
// 			// printf("WriteToDayRunOverFile==(Reason=%s),(NewEvent1=%s)\n,(NewEvent2=%s),(NewEvent3=%s)\n",Reason,NewEvent1,NewEvent2,NewEvent3);
// 			sprintf(NewEventWithTime_WriteToDayRunOverFile,"%s %s %s",TimeFormat(),Reason,AllNewEvent_WriteToDayRunOverFile);	
// 			if (Debug_file_data_show)
// 				printf("\nNewEventWithTime====(%s)\n",NewEventWithTime_WriteToDayRunOverFile);
// 			strcat(NewEventWithTime_WriteToDayRunOverFile,"\n");
// 			fwrite(NewEventWithTime_WriteToDayRunOverFile,strlen(NewEventWithTime_WriteToDayRunOverFile),1,fp0_WriteToDayRunOverFile);
// 			fclose(fp0_WriteToDayRunOverFile);	
// 			sync();

// 			for(j_WriteToDayRunOverFile=i_WriteToDayRunOverFile;j_WriteToDayRunOverFile<(Rows_WriteToDayRunOverFile-1);j_WriteToDayRunOverFile++){
// 				strcpy(DayRunDataArray_WriteToDayRunOverFile[j_WriteToDayRunOverFile],DayRunDataArray_WriteToDayRunOverFile[j_WriteToDayRunOverFile+1]);
// 			}
// 			memset(DayRunDataArray_WriteToDayRunOverFile[j_WriteToDayRunOverFile],0,DATA_SIZE);
// 			break;
// 		}
// 	}
// 	fp_WriteToDayRunOverFile = fopen("/tmp/usb/sda1/dailylog/DayRun", "w+");
// 	for (i_WriteToDayRunOverFile=0;i_WriteToDayRunOverFile<(Rows_WriteToDayRunOverFile-1);i_WriteToDayRunOverFile++){
// 		// printf("%d)%s))\n",i+1,webapibuf[Rows]);
// 		fwrite(DayRunDataArray_WriteToDayRunOverFile[i_WriteToDayRunOverFile],strlen(DayRunDataArray_WriteToDayRunOverFile[i_WriteToDayRunOverFile]),1,fp_WriteToDayRunOverFile);	
// 	}	
// 	fclose(fp_WriteToDayRunOverFile);	
// 	sync();
//     return 0;    
// }

// int WriteToPushDataOverFile()
// {
// 	FILE *fp_WriteToPushDataOverFile;//, *fp0_WriteToDayRunOverFile;
// 	int Rows_WriteToPushDataOverFile=0;    
// 	// char day_run_api_array[BUFFER_CURL_JSON_WEBAPI_SIZE][DATA_SIZE];
// 	int i_WriteToPushDataOverFile;//,j_WriteToDayRunOverFile;
	
// 	char PushDataArray_WriteToPushDataOverFile[BUFFER_CURL_JSON_WEBAPI_SIZE][DATA_SIZE];
// 	//WriteToDayRunOverFile()


// 	fp_WriteToPushDataOverFile = fopen(PUSHDATA_FILE, "r");
// 	while (fgets(PushDataArray_WriteToPushDataOverFile[Rows_WriteToPushDataOverFile], sizeof(PushDataArray_WriteToPushDataOverFile[Rows_WriteToPushDataOverFile]), fp_WriteToPushDataOverFile) != NULL){
// 		// printf("read_all=>%d)###(%s)\n",Rows_WriteToDayRunOverFile,DayRunDataArray_WriteToDayRunOverFile[Rows_WriteToDayRunOverFile]); //#讀出每一行
// 		Rows_WriteToPushDataOverFile++;  
// 	}
// 	fclose(fp_WriteToPushDataOverFile);
	
// 	fp_WriteToPushDataOverFile = fopen(PUSHDATA_FILE, "w+");
// 	for (i_WriteToPushDataOverFile=1;i_WriteToPushDataOverFile<(Rows_WriteToPushDataOverFile);i_WriteToPushDataOverFile++){
// 		// printf("write_back=>%d)%s))\n",i_WriteToDayRunOverFile,DayRunDataArray_WriteToDayRunOverFile[i_WriteToDayRunOverFile]);
// 		fwrite(PushDataArray_WriteToPushDataOverFile[i_WriteToPushDataOverFile],strlen(PushDataArray_WriteToPushDataOverFile[i_WriteToPushDataOverFile]),1,fp_WriteToPushDataOverFile);	
// 	}	
// 	fclose(fp_WriteToPushDataOverFile);	
// 	sync();
//     return 0;    
// }
int WriteToDayRunOverFile(char *file_name)
{
	FILE *fp_WriteToDayRunOverFile;//, *fp0_WriteToDayRunOverFile;
	int Rows_WriteToDayRunOverFile=0;    
	
	int i_WriteToDayRunOverFile;//,j_WriteToDayRunOverFile;
	// int i;
	
	char DayRunDataArray_WriteToDayRunOverFile[BUFFER_CURL_JSON_WEBAPI_SIZE][DATA_SIZE];
	fp_WriteToDayRunOverFile = fopen(file_name, "r");
	while (fgets(DayRunDataArray_WriteToDayRunOverFile[Rows_WriteToDayRunOverFile], sizeof(DayRunDataArray_WriteToDayRunOverFile[Rows_WriteToDayRunOverFile]), fp_WriteToDayRunOverFile) != NULL){
		// printf("read_all=>%d)###(%s)\n",Rows_WriteToDayRunOverFile,DayRunDataArray_WriteToDayRunOverFile[Rows_WriteToDayRunOverFile]); //#讀出每一行
		Rows_WriteToDayRunOverFile++;  
		if (Rows_WriteToDayRunOverFile>=BUFFER_CURL_JSON_WEBAPI_SIZE){
			Rows_WriteToDayRunOverFile=Rows_WriteToDayRunOverFile-50;
		}

		// if (Rows_WriteToDayRunOverFile>=BUFFER_CURL_JSON_WEBAPI_SIZE){
		// 	// for(i_WriteToDayRunOverFile=0;i_WriteToDayRunOverFile<BUFFER_CURL_JSON_WEBAPI_SIZE-1;i_WriteToDayRunOverFile++){
		// 	// 	strcpy(DayRunDataArray_WriteToDayRunOverFile[i_WriteToDayRunOverFile],DayRunDataArray_WriteToDayRunOverFile[i_WriteToDayRunOverFile+1]);
		// 	// }
		// 	// break;
		// }
		// Rows_WriteToDayRunOverFile=99;
	}
	fclose(fp_WriteToDayRunOverFile);
	
	fp_WriteToDayRunOverFile = fopen(file_name, "w+");
	for (i_WriteToDayRunOverFile=1;i_WriteToDayRunOverFile<(Rows_WriteToDayRunOverFile);i_WriteToDayRunOverFile++){
		// printf("write_back=>%d)%s))\n",i_WriteToDayRunOverFile,DayRunDataArray_WriteToDayRunOverFile[i_WriteToDayRunOverFile]);
		fwrite(DayRunDataArray_WriteToDayRunOverFile[i_WriteToDayRunOverFile],strlen(DayRunDataArray_WriteToDayRunOverFile[i_WriteToDayRunOverFile]),1,fp_WriteToDayRunOverFile);	
	}	
	fclose(fp_WriteToDayRunOverFile);	
	sync();
    return 0;    
}


// int WriteToDayFile(char *filename, char *NewEventDay)
// {

// 	FILE *fp_WriteToDayFile;
// 	int Rows_WriteToDayFile=0;    
// 	char daynamearray_WriteToDayFile[MAX_DAY_NAME_ARRAY][DAY_NAME_SIZE];
// 	int i_WriteToDayFile;
// 	char tmpfilename_WriteToDayFile[50];
// 	char delfilename_WriteToDayFile[50];
// 	//WriteToDayFile()

//     for (i_WriteToDayFile=0;i_WriteToDayFile<MAX_DAY_NAME_ARRAY;i_WriteToDayFile++){
//         memset(daynamearray_WriteToDayFile[i_WriteToDayFile],0,DAY_NAME_SIZE);
//     }
// 	if (DEBUG_PRINTF) 
// 		if (Debug_file_data_show) 
// 			printf("2filename=%s\n", filename);
//     if (NULL == (fp_WriteToDayFile = fopen(filename, "r"))){
// 		if (DEBUG_PRINTF) 
// 			if (Debug_file_data_show)
// 				printf("no file,Create New file %s\n",filename);
//         fp_WriteToDayFile = fopen(filename, "w");       
//     }

//     if (fp_WriteToDayFile != NULL){
//         Rows_WriteToDayFile = 0;
//         while (fgets(daynamearray_WriteToDayFile[Rows_WriteToDayFile], sizeof(daynamearray_WriteToDayFile[Rows_WriteToDayFile]), fp_WriteToDayFile) != NULL){
//             // printf("%d)%s",Rows,daynamearray[Rows]); //#讀出每一行
//             Rows_WriteToDayFile++;  
// 			if (Rows_WriteToDayFile==MAX_DAY_NAME_ARRAY){
// 				strcpy(delfilename_WriteToDayFile,daynamearray_WriteToDayFile[Rows_WriteToDayFile-1]);
// 				delfilename_WriteToDayFile[strlen(delfilename_WriteToDayFile)-1]=0;
// 				printf("delfilename=%s))\n",delfilename_WriteToDayFile);
// 				break;
// 			}          
//         }
// 		fclose(fp_WriteToDayFile);
//         if (Debug_file_data_show) 
// 			printf("\nTotal Rows=%d and close file\n", Rows_WriteToDayFile); //# 總共有幾筆		
//     }
// 	// printf("read no data!\n");
// 	if (Rows_WriteToDayFile>0){
// 		if(strcmp(daynamearray_WriteToDayFile[0],NewEventDay)==0){
// 			if (Debug_file_data_show)
// 			printf("No NewEventDay!\n");
// 		}
// 		else{
// 			fp_WriteToDayFile = fopen(filename, "w+"); //# 把檔案重寫
// 			if (Debug_file_data_show)
// 				printf("NewEvent=%s\n\n",NewEventDay);
// 			fwrite(NewEventDay,strlen(NewEventDay),1,fp_WriteToDayFile);
// 			if(Rows_WriteToDayFile >= MAX_DAY_NAME_ARRAY){		   
// 			for (i_WriteToDayFile=0;i_WriteToDayFile<(Rows_WriteToDayFile-1);i_WriteToDayFile++){
// 				// printf("%d)%s))\n",i+1,webapibuf[Rows]);
// 				fwrite(daynamearray_WriteToDayFile[i_WriteToDayFile],strlen(daynamearray_WriteToDayFile[i_WriteToDayFile]),1,fp_WriteToDayFile);	
// 			}
// 			if (Debug_file_data_show) 
// 				printf("%d)-the last list in array==%s)\n",i_WriteToDayFile,daynamearray_WriteToDayFile[i_WriteToDayFile-1]);
// 				// /tmp/usb/sda1/dailylog/
// 			sprintf(tmpfilename_WriteToDayFile,"rm /tmp/usb/sda1/dailylog/%s*\n",delfilename_WriteToDayFile);
// 			printf(tmpfilename_WriteToDayFile);
// 			system(tmpfilename_WriteToDayFile);

// 			} else {
// 				if (Debug_file_data_show) 
// 					printf("< MAX_DAY_NAME_ARRAY\n"); 
// 				for (i_WriteToDayFile=0;i_WriteToDayFile<(Rows_WriteToDayFile);i_WriteToDayFile++){
// 					if (Debug_file_data_show)
// 						printf("%d)%s))\n",i_WriteToDayFile+1,daynamearray_WriteToDayFile[Rows_WriteToDayFile]);
// 					fwrite(daynamearray_WriteToDayFile[i_WriteToDayFile],strlen(daynamearray_WriteToDayFile[i_WriteToDayFile]),1,fp_WriteToDayFile);
// 				}
// 				if (Debug_file_data_show) 
// 					printf("%d)%s-the last list\n",i_WriteToDayFile,daynamearray_WriteToDayFile[i_WriteToDayFile-1]);
// 				Rows_WriteToDayFile++;
// 			}
// 			printf("\n The NEW total Rows=%d \n", Rows_WriteToDayFile); //# 總共有幾筆
// 			usleep(100);
// 			fclose(fp_WriteToDayFile);
// 			sync();
// 			if (Debug_file_data_show)
// 				printf("%s file write over(1)\n",filename);
// 		}
		
// 	}else {
// 		if (Debug_file_data_show)
// 			printf("First NewEvent=%s\n\n",NewEventDay);
// 		fp_WriteToDayFile = fopen(filename, "w+"); //# 把檔案重寫
// 		fwrite(NewEventDay,strlen(NewEventDay),1,fp_WriteToDayFile);
// 		usleep(100);
// 		fclose(fp_WriteToDayFile);
// 		sync();
// 		// if (Debug_Socket_server_webapi_String)
// 		printf("%s file write over(2)\n",filename);
// 	}	
//     return 0;    
// }

char *ReadFilefirstlinedata(char *filename){

	FILE *fp_ReadFirstRunDate;
	char *RunData_ReadFirstRunDate;
	char RunDatastring_ReadFirstRunDate[DATA_SIZE];
	//ReadFirstRunDate()


	// if (Debug_file_data_show)
    // 	printf("ReadFirstRunDate3filename=%s\n", filename);
	if (NULL == (fp_ReadFirstRunDate = fopen(filename, "r"))){
		if (DEBUG_PRINTF) 
        	printf(" ReadFirstRunDate() no file, %s\n",filename);
		// fclose(fp_ReadFirstRunDate);
        return NULL;       
    }else{
		if (fgets(RunDatastring_ReadFirstRunDate, sizeof(RunDatastring_ReadFirstRunDate), fp_ReadFirstRunDate) != NULL){
			RunData_ReadFirstRunDate = RunDatastring_ReadFirstRunDate;
			if (DEBUG_PRINTF)
				printf("\nfile %s, first line data=>%s)\n\n",filename,RunData_ReadFirstRunDate);
			fclose(fp_ReadFirstRunDate);
			return RunData_ReadFirstRunDate;
		}else{
			fclose(fp_ReadFirstRunDate);
			return NULL;
		}		
	}
	// fclose(fp_ReadFirstRunDate);
}

time_t *UtcTime(){ //時間處理程式

	static time_t timep_UtcTime;  
	struct tm *p_UtcTime; 
	//UtcTime()
	 
	time(&timep_UtcTime);
	p_UtcTime=localtime(&timep_UtcTime); /*取得當地時間*/ 
	if (DEBUG_PRINTF) 
		printf("\nUTCtime()==>%ld)\n",timep_UtcTime);
	return &timep_UtcTime;
}
void *Pidof_thread_work(){

	// char iot_adjust_str[50];
	char check_webapi_run_adjust_str[50];
	bool iot_flag_run_once = true;	
	bool check_webapi_file_run_once = true;
	FILE *iot_fp;
	// FILE *webapi_fp;
    int iot_Rows=0;    
    char iot_buf[DATA_SIZE];
	// char echo_buf[DATA_SIZE];
	char iot_kill_string[50];
	char remove_string[50];
	int iot_pid=0;
	int iot_mem_init_size=0;
	int iot_mem_max_size=0;

	struct stat buffer;
    int exist;
	int count=0;
	
	sleep(10);

	system("ps > /var/www/ps.log");
	iot_mem_init_size=check_ps_log("/var/www/ps.log",IOT_FILE_NAME,count);
	if (iot_mem_init_size==0){
		iot_mem_init_size=24000;
	}

	if (DEBUG_PRINTF)
		printf("%s\n mem init size=%d)\n",IOT_FILE_NAME,iot_mem_init_size);
	iot_mem_max_size=iot_mem_init_size*1.3;
	if (DEBUG_PRINTF)	
		printf("%s mem max size=%d,init size=%d)\n",IOT_FILE_NAME,iot_mem_max_size,iot_mem_init_size);
	system("rm /var/www/ps.log");

	while(true){
		usleep(100000);

		gettimeofday (&tv_now , &tz_now);		
		// printf("tv_now_sec:%ld\n",tv_now.tv_sec); 
		
		if ((tv_now.tv_sec%60)==CHECK_IOT_TIME){
			system("ps > /var/www/ps.log");
			if (iot_flag_run_once){
				iot_flag_run_once=false;
				

				if (DEBUG_PRINTF)
					printf("\ncheck %s mem max size =%d,init size=%d)\n",IOT_FILE_NAME,MAX_IOTCAPI_MEM_SIZE,iot_mem_init_size);
				count++;
				if(check_ps_log("/var/www/ps.log",IOT_FILE_NAME,count)>MAX_IOTCAPI_MEM_SIZE){

					sprintf(iot_buf,"%s - aw.%s_%s_MEM_%d! \n",TimeFormat(),"reset",IOT_FILE_NAME,MAX_IOTCAPI_MEM_SIZE);
					if (DEBUG_PRINTF)
						printf("%s",iot_buf);
					echo_file(iot_buf);


					if (DEBUG_PRINTF)
						printf("%s over %d\n",IOT_FILE_NAME,MAX_IOTCAPI_MEM_SIZE);
					sprintf(iot_buf,"pidof %s > %s",IOT_FILE_NAME,IOT_TXT_DATA);
					system(iot_buf);
					usleep(10000);
					sprintf(iot_buf,"cat %s",IOT_TXT_DATA);
					system(iot_buf);
					memset(iot_buf,0,DATA_SIZE);
					// if (Debug_Socket_server_webapi_String) printf("filename=%s\n", "ii.txt");
					if (NULL == (iot_fp = fopen(IOT_TXT_DATA, "r"))){
						if (DEBUG_PRINTF)
							printf("no file,no data %s\n",IOT_TXT_DATA);
						// fp = fopen(filename, "w");       
					}else{
						iot_Rows = 0;
						// printf("uu\n");
						memset(iot_buf,0,DATA_SIZE);
						if(fgets(iot_buf, sizeof(iot_buf), iot_fp) != NULL){
							// printf("%d)%s",ss_Rows,ss_webapibuf);            
							iot_pid=atoi(iot_buf);
							if (DEBUG_PRINTF)
								printf("%s pid=%d\n",IOT_FILE_NAME,iot_pid);
							sprintf(iot_kill_string,"kill %d",iot_pid);
							system(iot_kill_string);
						}
						else{
							if (DEBUG_PRINTF)
								printf("%s pid NULL\n",IOT_FILE_NAME);
						}					
						sprintf(remove_string,"rm %s",IOT_TXT_DATA);
						if (DEBUG_PRINTF)
							printf(remove_string);
						system(remove_string);
						fclose(iot_fp);												
					}					
				}
				if (count>=MEM_CHECK_MIN) count=0;
			}
			system("rm /var/www/ps.log");
		}else{
			iot_flag_run_once=true;
		}	
		strcpy(check_webapi_run_adjust_str,SecFormat());
		if (strcmp(check_webapi_run_adjust_str,CHECK_RUN_TIME)==0){
			if (check_webapi_file_run_once){
				check_webapi_file_run_once=false;
				// printf("(now time=%s)\n",MinFormat());
				// system("pidof Heartbeat_8198c");
				if (DEBUG_PRINTF)
					printf("(now time=%s)check webapi\n",HourFormat());				
				exist = stat(RUN_PATH,&buffer);
				if(exist == 0){
					if (DEBUG_PRINTF)
						printf("%s exit!\n",RUN_PATH);
					WebApiinfo.Webapi_run=true;
					sync();
					sprintf(remove_string,"rm %s\n\n",RUN_PATH);
					if (DEBUG_PRINTF)
						printf(remove_string);
					system(remove_string);
				}else{
					if (DEBUG_PRINTF)
						printf("%s dont exit!\n",RUN_PATH);
				}
			}			
		}
		else{
			check_webapi_file_run_once=true;
		}

	}
}

int WebapiBufferExec()
{    

	char temp_WebapiBufferExec[128];
	char write_sys_log_buf_WebapiBufferExec[1024];
	// char string_data_WebapiBufferExec[DATA_SIZE];
	cJSON *input_json_data_WebapiBufferExec;
	char scode_message_WebapiBufferExec[20];
	// char temp_time_day_WebapiBufferExec[20];
	char inputstrinngs_WebapiBufferExec[DATA_SIZE];
	int size_WebapiBufferExec;
	char push_data_string[DATA_SIZE];
	// char push_data_exec[DATA_SIZE];
	unsigned int utc_now,utc_data;
	char *utc_data_string;
	char *temp_str;
	const char *str1=" ";
	const char *str2=">";
	char *push_string;
	// char push_key[64];
	char sno[20];
	int SKSWebApi_res;
	int i;

	bool flag_http=false;
	bool flag_push=false;
	
	
	if(WebApiinfo.Curl_exec==false){
		WebApiinfo.Curl_exec=true;

		memset(inputstrinngs_WebapiBufferExec,0,DATA_SIZE);

		if (ReadFilefirstlinedata(DAYRUN_FILE)!=NULL){
			flag_http=true;
			strcpy(inputstrinngs_WebapiBufferExec,ReadFilefirstlinedata(DAYRUN_FILE));
		}else if(ReadFilefirstlinedata(PUSHDATA_FILE)!=NULL){
			flag_push=true;
			strcpy(inputstrinngs_WebapiBufferExec,ReadFilefirstlinedata(PUSHDATA_FILE));
		}else{
			flag_push=false;
			flag_http=false;
		}
		
		if (flag_http|flag_push){


			
			if (strstr(inputstrinngs_WebapiBufferExec, HTTPS_DATA)!=NULL){	

				strcpy(inputstrinngs_WebapiBufferExec,ReadFilefirstlinedata(DAYRUN_FILE));
				if (DEBUG_PRINTF)
					printf("\nexecute https data!!\n");
				
				strcpy(WebApiinfo.inputstring,inputstrinngs_WebapiBufferExec);
				if (DEBUG_PRINTF)
					printf("https data==>%s\n",WebApiinfo.inputstring);
				
				int ParseCurlString_res = ParseCurlStringWebapiExec();
				
				if (ParseCurlString_res == -1){  //			
					sprintf(temp_WebapiBufferExec,"Parse_fail!");
						
					sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.%s! \n",TimeFormat(),temp_WebapiBufferExec);
					if (DEBUG_PRINTF)
						printf("%s",write_sys_log_buf_WebapiBufferExec);

					echo_file(write_sys_log_buf_WebapiBufferExec);

					// WebApiinfo.sys_backup_count++;
					// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf_WebapiBufferExec);
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
					// printf(temp_WebapiBufferExec);
					// printf("\n");
					if (DEBUG_PRINTF)
						printf("****************Parse_fail\n");

					if (flag_http){
						WriteToDayRunOverFile(DAYRUN_FILE);

						if (DEBUG_PRINTF) printf("\nwrite usb 5 start time=>%s\n\n",TimeFormat());

						system("cp /var/www/DayRun /tmp/usb/sda1/dayrun/DayRun ");
						sync();
						// system("cp /var/www/DayRun /var/www/DayRuntmp");
						// system("cat /var/www/DayRuntmp");
						// system("tar cvf /var/www/DayRuntmp.tar /var/www/DayRuntmp");
						// // system("cat /var/www/DayRuntmp");
						// if(flash_write(FLASH_DayRun_START, 4096*4, "/var/www/DayRuntmp.tar")){
						// 	if (DEBUG_PRINTF)
						// 		printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
						// }
							
						// else{
						// 	if (DEBUG_PRINTF)
						// 		printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
						// }
						// if (DEBUG_PRINTF) printf("write flash 5 end time=>%s\n",TimeFormat());
						// // printf(time_tmp);																		
						// system("rm /var/www/DayRuntmp");
						// system("rm /var/www/DayRuntmp.tar");						
						// sync();
						
					}
					

					WebApiinfo.Curl_exec=false;
					// return -1;	
				} else {
					// input_json_data_WebapiBufferExec=cJSON_Parse(WebApiinfo.jsonstr);
					// cJSON_ReplaceItemInObject(input_json_data_WebapiBufferExec, "scode", cJSON_CreateString("ER00001,"));
					// // cJSON_ReplaceItemInObject(input_json_data_WebapiBufferExec, "sigtime", cJSON_CreateString(TimeFormat()));
					
					// sprintf(WebApiinfo.jsonstr,cJSON_Print(input_json_data_WebapiBufferExec));

					SKSWebApi_res=SKSWebApi(); 
					if (SKSWebApi_res == -1){
						sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.Curl_function_open_fail! \n",TimeFormat());
						if (DEBUG_PRINTF)
							printf("%s",write_sys_log_buf_WebapiBufferExec);

						echo_file(write_sys_log_buf_WebapiBufferExec);
						// WebApiinfo.sys_backup_count++;
						// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf_WebapiBufferExec);
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
						WebApiinfo.Curl_exec=false;
						// return -2;
					} else if (SKSWebApi_res == CURLE_OK){
						if (DEBUG_PRINTF)
							if (Debug_Socket_server_webapi_String) printf("CURLE_OK todo!\n");			
						input_json_data_WebapiBufferExec=cJSON_Parse(WebApiinfo.jsonstr);			
						sprintf(scode_message_WebapiBufferExec,cJSON_Print(cJSON_GetObjectItem(input_json_data_WebapiBufferExec,"scode")));
						sprintf(sno,cJSON_Print(cJSON_GetObjectItem(input_json_data_WebapiBufferExec,"sno")));
						// printf("scode_message=>%s\n",scode_message);
						size_WebapiBufferExec=strlen(scode_message_WebapiBufferExec);
						scode_message_WebapiBufferExec[size_WebapiBufferExec-2]=scode_message_WebapiBufferExec[size_WebapiBufferExec-1];
						scode_message_WebapiBufferExec[size_WebapiBufferExec-1]=scode_message_WebapiBufferExec[size_WebapiBufferExec];
						if (DEBUG_PRINTF)
							if (Debug_Socket_server_webapi_String) printf("scode_message=>%s\n\n",scode_message_WebapiBufferExec);
						cJSON_Delete(input_json_data_WebapiBufferExec);

						if(WebApiinfo.response_code==409){   
							sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.%s_%s,%s! \n",TimeFormat(),scode_message_WebapiBufferExec,WebApiinfo.response_message,sno);
							if (DEBUG_PRINTF)
								printf("%s\n",write_sys_log_buf_WebapiBufferExec);
							if (DEBUG_PRINTF)
								printf("******** response 409!\n");
							if (flag_http){
								WriteToDayRunOverFile(DAYRUN_FILE);

								if (DEBUG_PRINTF) printf("\nwrite usb 4 start time=>%s\n\n",TimeFormat());	

								system("cp /var/www/DayRun /tmp/usb/sda1/dayrun/DayRun ");
								sync();								
								
							}
							
							WebApiinfo.COUNT_HTTP_OTHER_FAIL=0;

						}else if((WebApiinfo.response_code==200)|(WebApiinfo.response_code==201)){	

							if ((WebApiinfo.response_code==201)&((strcmp(WebApiinfo.methodstr,"POST")==0))){
								if (strcmp(GetInterface(),"ppp0")==0){
									sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.%s_%s_%s_%s_4G! \n",TimeFormat(),scode_message_WebapiBufferExec,WebApiinfo.response_message,sno,FIRMWARE_VER2);
								}else{
									sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.%s_%s_%s_%s_lan! \n",TimeFormat(),scode_message_WebapiBufferExec,WebApiinfo.response_message,sno,FIRMWARE_VER2);
								}
							}
							else if ((WebApiinfo.response_code==200)&((strcmp(WebApiinfo.methodstr,"PUT")==0))){
								if (strcmp(GetInterface(),"ppp0")==0){
									sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.%s_%s_%s_%s_4G! \n",TimeFormat(),scode_message_WebapiBufferExec,WebApiinfo.response_message,sno,FIRMWARE_VER2);
								}else{
									sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.%s_%s_%s_%s_lan! \n",TimeFormat(),scode_message_WebapiBufferExec,WebApiinfo.response_message,sno,FIRMWARE_VER2);
								}
							}
							else if ((WebApiinfo.response_code==200)&((strcmp(WebApiinfo.methodstr,"POST")==0))){
								if (strcmp(GetInterface(),"ppp0")==0){
									sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.error_match_%s_%s_%s_%s_4G! \n",TimeFormat(),scode_message_WebapiBufferExec,WebApiinfo.response_message,sno,FIRMWARE_VER2);
								}else{
									sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.error_match_%s_%s_%s_%s_lan! \n",TimeFormat(),scode_message_WebapiBufferExec,WebApiinfo.response_message,sno,FIRMWARE_VER2);
								}
							}
							else{
								if (strcmp(GetInterface(),"ppp0")==0){
									sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.other_issue_%s_%s_%s_%s_%s_%ld_4G! \n",TimeFormat(),scode_message_WebapiBufferExec,WebApiinfo.response_message,sno,FIRMWARE_VER2,WebApiinfo.methodstr,WebApiinfo.response_code);
								}else{
									sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.0ther_issue_%s_%s_%s_%s_%s_%ld_lan! \n",TimeFormat(),scode_message_WebapiBufferExec,WebApiinfo.response_message,sno,FIRMWARE_VER2,WebApiinfo.methodstr,WebApiinfo.response_code);
								}
							}
							if (DEBUG_PRINTF)
								printf("%s\n",write_sys_log_buf_WebapiBufferExec);						
							if (Debug_Socket_server_webapi_String|Debug_file_data_show) {
								printf("****   ****   response 200 201!\n");
								printf("******list1=\n(WebApiinfo->curlcommandstr=%s)\n(WebApiinfo->jsonstr=%s)\n(WebApiinfo->urlstr=%s)\n( WebApiinfo->response_message=%s)\n",WebApiinfo.curlcommandstr,WebApiinfo.jsonstr,WebApiinfo.urlstr, WebApiinfo.response_message);
							}	
								
							if (flag_http){
								WriteToDayRunOverFile(DAYRUN_FILE);
								if (DEBUG_PRINTF) printf("\nwrite usb 3 start time=>%s\n\n",TimeFormat());	
								system("cp /var/www/DayRun /tmp/usb/sda1/dayrun/DayRun ");
								sync();			
							}
							
							WebApiinfo.COUNT_HTTP_OTHER_FAIL=0;
						}
						else{ //other response code
							
							sprintf(temp_WebapiBufferExec,WebApiinfo.response_message);
							if (strcmp(GetInterface(),"ppp0")==0){
								sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.%s_fail_%s_%s_%s_4G! \n",TimeFormat(),scode_message_WebapiBufferExec,sno,temp_WebapiBufferExec,FIRMWARE_VER2);
							}else{
								sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.%s_fail_%s_%s_%s_lan! \n",TimeFormat(),scode_message_WebapiBufferExec,sno,temp_WebapiBufferExec,FIRMWARE_VER2);
							}
							if (DEBUG_PRINTF)							
								printf("%s",write_sys_log_buf_WebapiBufferExec);
							if (DEBUG_PRINTF)
								printf(temp_WebapiBufferExec);
							if (DEBUG_PRINTF)
								printf("********other response code!\n");
							WebApiinfo.COUNT_HTTP_OTHER_FAIL++;
							if (WebApiinfo.COUNT_HTTP_OTHER_FAIL>=MAX_CURL_FAIL){ //other response code! try 3 time
								WebApiinfo.COUNT_HTTP_OTHER_FAIL=0;		
								if (flag_http){
									WriteToDayRunOverFile(DAYRUN_FILE);

									if (DEBUG_PRINTF) printf("\nwrite usb 2 start time=>%s\n\n",TimeFormat());																
									system("cp /var/www/DayRun /tmp/usb/sda1/dayrun/DayRun ");
									sync();
									
									// system("cp /var/www/DayRun /var/www/DayRuntmp");
									// system("cat /var/www/DayRuntmp");
									// system("tar cvf /var/www/DayRuntmp.tar /var/www/DayRuntmp");
									// // system("cat /var/www/DayRuntmp");
									// if(flash_write(FLASH_DayRun_START, 4096*4, "/var/www/DayRuntmp.tar")){
									// 	if (DEBUG_PRINTF)
									// 		printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
									// }
										
									// else{
									// 	if (DEBUG_PRINTF)
									// 		printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
									// }
									// if (DEBUG_PRINTF) printf("write flash 2 end time=>%s\n",TimeFormat());
									// // printf(time_tmp);																		
									// system("rm /var/www/DayRuntmp");
									// system("rm /var/www/DayRuntmp.tar");						
									// sync();

								}
								
								sleep(1);
								if (DEBUG_PRINTF)
									printf("******list2=\n(WebApiinfo->curlcommandstr=%s)\n(WebApiinfo->jsonstr=%s)\n(WebApiinfo->urlstr=%s)\n( WebApiinfo->response_message=%s)\n",WebApiinfo.curlcommandstr,WebApiinfo.jsonstr,WebApiinfo.urlstr, WebApiinfo.response_message);
								input_json_data_WebapiBufferExec=cJSON_Parse(WebApiinfo.jsonstr);
								cJSON_ReplaceItemInObject(input_json_data_WebapiBufferExec, "scode", cJSON_CreateString("ER000000001,"));
								cJSON_ReplaceItemInObject(input_json_data_WebapiBufferExec, "sigtime", cJSON_CreateString(TimeFormat()));
								
								sprintf(WebApiinfo.jsonstr,cJSON_Print(input_json_data_WebapiBufferExec));
								if (DEBUG_PRINTF)
									printf("1json replace new scode =>(%s)\n",WebApiinfo.jsonstr);	
								cJSON_Delete(input_json_data_WebapiBufferExec);
								SKSWebApi_res=SKSWebApi();// for ER000000001 //other response code try 3 time
								if (SKSWebApi_res == -1){
									sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.Curl_function_open_fail_ER000000001! \n",TimeFormat());
									if (DEBUG_PRINTF)
										printf("%s",write_sys_log_buf_WebapiBufferExec);

									echo_file(write_sys_log_buf_WebapiBufferExec);
									// WebApiinfo.sys_backup_count++;
									// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf_WebapiBufferExec);
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
									
									return -2;
								}else if (SKSWebApi_res == CURLE_OK){
									if (DEBUG_PRINTF)
										if (Debug_Socket_server_webapi_String) printf("CURLE_OK!\n");			
									input_json_data_WebapiBufferExec=cJSON_Parse(WebApiinfo.jsonstr);			
									sprintf(scode_message_WebapiBufferExec,cJSON_Print(cJSON_GetObjectItem(input_json_data_WebapiBufferExec,"scode")));
									sprintf(sno,cJSON_Print(cJSON_GetObjectItem(input_json_data_WebapiBufferExec,"sno")));
									// printf("scode_message=>%s\n",scode_message);
									size_WebapiBufferExec=strlen(scode_message_WebapiBufferExec);
									scode_message_WebapiBufferExec[size_WebapiBufferExec-2]=scode_message_WebapiBufferExec[size_WebapiBufferExec-1];
									scode_message_WebapiBufferExec[size_WebapiBufferExec-1]=scode_message_WebapiBufferExec[size_WebapiBufferExec];
									if (DEBUG_PRINTF)
										if (Debug_Socket_server_webapi_String) printf("scode_message=>%s\n\n",scode_message_WebapiBufferExec);
									cJSON_Delete(input_json_data_WebapiBufferExec);

									if(WebApiinfo.response_code==409){    
										sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.%s_%s_%s_%s! \n",TimeFormat(),scode_message_WebapiBufferExec,WebApiinfo.response_message,sno,FIRMWARE_VER2);
										if (DEBUG_PRINTF)
											printf("%s\n",write_sys_log_buf_WebapiBufferExec);
										if (DEBUG_PRINTF)
											printf("******** scode ER000000001 response 409!\n");
										
									}else if((WebApiinfo.response_code==200)|(WebApiinfo.response_code==201)){

										if ((WebApiinfo.response_code==201)&((strcmp(WebApiinfo.methodstr,"POST")==0))){
											if (strcmp(GetInterface(),"ppp0")==0){
												sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.%s_%s_%s_%s_4G! \n",TimeFormat(),scode_message_WebapiBufferExec,WebApiinfo.response_message,sno,FIRMWARE_VER2);
											}else{
												sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.%s_%s_%s_%s_lan! \n",TimeFormat(),scode_message_WebapiBufferExec,WebApiinfo.response_message,sno,FIRMWARE_VER2);
											}
										}
										else if ((WebApiinfo.response_code==200)&((strcmp(WebApiinfo.methodstr,"PUT")==0))){
											if (strcmp(GetInterface(),"ppp0")==0){
												sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.%s_%s_%s_%s_4G! \n",TimeFormat(),scode_message_WebapiBufferExec,WebApiinfo.response_message,sno,FIRMWARE_VER2);
											}else{
												sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.%s_%s_%s_%s_lan! \n",TimeFormat(),scode_message_WebapiBufferExec,WebApiinfo.response_message,sno,FIRMWARE_VER2);
											}
										}
										else if ((WebApiinfo.response_code==200)&((strcmp(WebApiinfo.methodstr,"POST")==0))){
											if (strcmp(GetInterface(),"ppp0")==0){
												sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.error_match_%s_%s_%s_%s_4G! \n",TimeFormat(),scode_message_WebapiBufferExec,WebApiinfo.response_message,sno,FIRMWARE_VER2);
											}else{
												sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.error_match_%s_%s_%s_%s_lan! \n",TimeFormat(),scode_message_WebapiBufferExec,WebApiinfo.response_message,sno,FIRMWARE_VER2);
											}
										}
										else{
											if (strcmp(GetInterface(),"ppp0")==0){
												sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.other_issue_%s_%s_%s_%s_%s_%ld_4G! \n",TimeFormat(),scode_message_WebapiBufferExec,WebApiinfo.response_message,sno,FIRMWARE_VER2,WebApiinfo.methodstr,WebApiinfo.response_code);
											}else{
												sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.0ther_issue_%s_%s_%s_%s_%s_%ld_lan! \n",TimeFormat(),scode_message_WebapiBufferExec,WebApiinfo.response_message,sno,FIRMWARE_VER2,WebApiinfo.methodstr,WebApiinfo.response_code);
											}
										}
										if (DEBUG_PRINTF)
											printf("(%s)\n",write_sys_log_buf_WebapiBufferExec);
									}
									else{//	other response code						
										sprintf(temp_WebapiBufferExec,WebApiinfo.response_message);
										if (strcmp(GetInterface(),"ppp0")==0){
											sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.%s_fail_%s_%s_%s_4G! \n",TimeFormat(),scode_message_WebapiBufferExec,temp_WebapiBufferExec,sno,FIRMWARE_VER2);
										}else{
											sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.%s_fail_%s_%s_%s_lan! \n",TimeFormat(),scode_message_WebapiBufferExec,temp_WebapiBufferExec,sno,FIRMWARE_VER2);
										}
										if (DEBUG_PRINTF) printf("(%s)\n",write_sys_log_buf_WebapiBufferExec);
										if (DEBUG_PRINTF) printf("(%s)\n",temp_WebapiBufferExec);
										if (DEBUG_PRINTF) printf("Scode ER000000001 response other response!\n");															
									}
									
									WriteToFileSys(FILE_PATH,write_sys_log_buf_WebapiBufferExec);
									// system("cat /var/www/sys.log");
									// WebApiinfo.sys_backup_count++;
									sync();
									system("cp /var/www/sys.log /var/www/sys.logtmp");
									if(flash_write(FLASH_UISLOG, 4096*2, "/var/www/sys.logtmp")){
										if (DEBUG_PRINTF)
											printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
									}									
									else {
										if (DEBUG_PRINTF)
											printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
									}															
									system("rm /var/www/sys.logtmp");
									sync();
									echo_file(write_sys_log_buf_WebapiBufferExec);
									//with json
									sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.%s_%s_%s_%s_(%s)4G! \n",TimeFormat(),scode_message_WebapiBufferExec,WebApiinfo.response_message,sno,FIRMWARE_VER2,WebApiinfo.jsonstr);

									echo_file(write_sys_log_buf_WebapiBufferExec);

									// WebApiinfo.sys_backup_count++;
									// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf_WebapiBufferExec);
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

								}else {  //curl non ok
									if (DEBUG_PRINTF)
										printf("scode ER000000001 Non CURLE_OK!\n");
									input_json_data_WebapiBufferExec=cJSON_Parse(WebApiinfo.jsonstr);			
									sprintf(scode_message_WebapiBufferExec,cJSON_Print(cJSON_GetObjectItem(input_json_data_WebapiBufferExec,"scode")));
									sprintf(sno,cJSON_Print(cJSON_GetObjectItem(input_json_data_WebapiBufferExec,"sno")));
									// printf("scode_message=>%s\n",scode_message);
									size_WebapiBufferExec=strlen(scode_message_WebapiBufferExec);
									scode_message_WebapiBufferExec[size_WebapiBufferExec-2]=scode_message_WebapiBufferExec[size_WebapiBufferExec-1];
									scode_message_WebapiBufferExec[size_WebapiBufferExec-1]=scode_message_WebapiBufferExec[size_WebapiBufferExec];
									cJSON_Delete(input_json_data_WebapiBufferExec);
									sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.%s_%s_%s_%s! \n",TimeFormat(),"Other_curl_non_ok","ER000000001",sno,FIRMWARE_VER2);
									if (DEBUG_PRINTF)
										printf("%s",write_sys_log_buf_WebapiBufferExec);								
									WriteToFileSys(FILE_PATH,write_sys_log_buf_WebapiBufferExec);
									// WebApiinfo.sys_backup_count++;								
									system("cp /var/www/sys.log /var/www/sys.logtmp");
									if(flash_write(FLASH_UISLOG, 4096*2, "/var/www/sys.logtmp"))
									{
										if (DEBUG_PRINTF)
											printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
									}							
									else{
										if (DEBUG_PRINTF)
											printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
									}						
									system("rm /var/www/sys.logtmp");
									// sync();	
									
									echo_file(write_sys_log_buf_WebapiBufferExec);
									// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf_WebapiBufferExec);
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
								
							}//other response code!	try 3 time						
						}//other response code!						
												
						WriteToFileSys(FILE_PATH,write_sys_log_buf_WebapiBufferExec);
												
						system("cp /var/www/sys.log /var/www/sys.logtmp");
						if(flash_write(FLASH_UISLOG, 4096*2, "/var/www/sys.logtmp")){
							if (DEBUG_PRINTF)
								printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
						}							
						else
						{
							if (DEBUG_PRINTF)
								printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
						}
													
						system("rm /var/www/sys.logtmp");
						sync();

						echo_file(write_sys_log_buf_WebapiBufferExec);						

						WebApiinfo.Curl_exec=false;
						
					} 
					else 
					{
						if (DEBUG_PRINTF)
							printf("Non CURLE_OK!  WebApiinfo.jsonstr==>%s)\n",WebApiinfo.jsonstr);
						input_json_data_WebapiBufferExec=cJSON_Parse(WebApiinfo.jsonstr);			
						sprintf(scode_message_WebapiBufferExec,cJSON_Print(cJSON_GetObjectItem(input_json_data_WebapiBufferExec,"scode")));
						sprintf(sno,cJSON_Print(cJSON_GetObjectItem(input_json_data_WebapiBufferExec,"sno")));
						// printf("scode_message=>%s\n",scode_message);
						size_WebapiBufferExec=strlen(scode_message_WebapiBufferExec);
						scode_message_WebapiBufferExec[size_WebapiBufferExec-2]=scode_message_WebapiBufferExec[size_WebapiBufferExec-1];
						scode_message_WebapiBufferExec[size_WebapiBufferExec-1]=scode_message_WebapiBufferExec[size_WebapiBufferExec];
						cJSON_Delete(input_json_data_WebapiBufferExec);
						memset(write_sys_log_buf_WebapiBufferExec,0,128);
						WebApiinfo.COUNT_HTTP_OTHER_FAIL++;
						if (SKSWebApi_res== 22){
							sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.scode_fail_or_time_%s_%s(%d)! \n",TimeFormat(),sno,WebApiinfo.response_message,WebApiinfo.COUNT_HTTP_OTHER_FAIL);
						}
						else{
							sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.message_%s_%s(%d)! \n",TimeFormat(),sno,WebApiinfo.response_message,WebApiinfo.COUNT_HTTP_OTHER_FAIL);
						}
						
						if (DEBUG_PRINTF)
							printf("%s",write_sys_log_buf_WebapiBufferExec);

						// WebApiinfo.sys_backup_count++;

						WriteToFileSys(FILE_PATH,write_sys_log_buf_WebapiBufferExec);
						// sync();
						system("cp /var/www/sys.log /var/www/sys.logtmp");
						if(flash_write(FLASH_UISLOG, 4096*2, "/var/www/sys.logtmp")){
							if (DEBUG_PRINTF)
								printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
						}							
						else
						{
							if (DEBUG_PRINTF)
								printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
						}													
						system("rm /var/www/sys.logtmp");
						sync();

						if (SKSWebApi_res== 22){
							sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.scode_or_time_issue_%s_%s(%d_%s)! \n",TimeFormat(),sno,WebApiinfo.response_message,WebApiinfo.COUNT_HTTP_OTHER_FAIL,WebApiinfo.jsonstr);
						}
						else{
							sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.message_%s_%s(%d_%s)! \n",TimeFormat(),sno,WebApiinfo.response_message,WebApiinfo.COUNT_HTTP_OTHER_FAIL,WebApiinfo.jsonstr);
						}

						echo_file(write_sys_log_buf_WebapiBufferExec);
						
						if (strstr(WebApiinfo.response_message,"resolve host")){//Couldn't resolve host
							WebApiinfo.COUNT_HTTP_OTHER_FAIL=0;
							printf("find _resolve host_ string!\n");
							WebApiinfo.Webapi_run=false;
						}
						else if (strstr(WebApiinfo.response_message,"Timeout")){//Timeout
							WebApiinfo.COUNT_HTTP_OTHER_FAIL=0;
							printf("find _Timeout_ string!\n");
							WebApiinfo.Webapi_run=false;
						}
						else if (strstr(WebApiinfo.response_message,"connect to server")){//Couldn't connect to server
							WebApiinfo.COUNT_HTTP_OTHER_FAIL=0;
							printf("find _connect to server_ string!\n");
							WebApiinfo.Webapi_run=false;
						}						
						else{
							WebApiinfo.Webapi_run=true;
						}						
						if (WebApiinfo.COUNT_HTTP_OTHER_FAIL>=MAX_CURL_FAIL){// curl non ok retry 3 time
							WebApiinfo.COUNT_HTTP_OTHER_FAIL=0;

							if (flag_http){
								
								WriteToDayRunOverFile(DAYRUN_FILE);
								if (DEBUG_PRINTF) printf("\nexec DAYRUN_FILE\n");
															
								//flag_push PUSHDATA_FILE
							}
							
							sleep(1);
							if (DEBUG_PRINTF)
								printf("******list3=\n(WebApiinfo->curlcommandstr=%s)\n(WebApiinfo->jsonstr=%s)\n(WebApiinfo->urlstr=%s)\n( WebApiinfo->response_message=%s)\n",WebApiinfo.curlcommandstr,WebApiinfo.jsonstr,WebApiinfo.urlstr, WebApiinfo.response_message);
							input_json_data_WebapiBufferExec=cJSON_Parse(WebApiinfo.jsonstr);
							cJSON_ReplaceItemInObject(input_json_data_WebapiBufferExec, "scode", cJSON_CreateString("ER000000001,"));
							cJSON_ReplaceItemInObject(input_json_data_WebapiBufferExec, "sigtime", cJSON_CreateString(TimeFormat()));
							
							sprintf(WebApiinfo.jsonstr,cJSON_Print(input_json_data_WebapiBufferExec));
							if (DEBUG_PRINTF)
								printf("2json replace new scode =>(%s)\n",WebApiinfo.jsonstr);	
							cJSON_Delete(input_json_data_WebapiBufferExec);
							SKSWebApi_res=SKSWebApi();  // curl non ok retry 3 time

							if (SKSWebApi_res == -1){
								sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.Curl_function_open_fail(ER0001)! \n",TimeFormat());
								if (DEBUG_PRINTF)
									printf("%s",write_sys_log_buf_WebapiBufferExec);

								WriteToFileSys(FILE_PATH,write_sys_log_buf_WebapiBufferExec);
								
								system("cp /var/www/sys.log /var/www/sys.logtmp");
								if(flash_write(FLASH_UISLOG, 4096*2, "/var/www/sys.logtmp"))
									printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
								else
									printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);						
								system("rm /var/www/sys.logtmp");
								sync();

								echo_file(write_sys_log_buf_WebapiBufferExec);			
								
								return -2;
							}else if (SKSWebApi_res == CURLE_OK){
								if (DEBUG_PRINTF)
									if (Debug_Socket_server_webapi_String) printf("CURLE_OK todo!\n");			
								input_json_data_WebapiBufferExec=cJSON_Parse(WebApiinfo.jsonstr);			
								sprintf(scode_message_WebapiBufferExec,cJSON_Print(cJSON_GetObjectItem(input_json_data_WebapiBufferExec,"scode")));
								sprintf(sno,cJSON_Print(cJSON_GetObjectItem(input_json_data_WebapiBufferExec,"sno")));
								// printf("scode_message=>%s\n",scode_message);
								size_WebapiBufferExec=strlen(scode_message_WebapiBufferExec);
								scode_message_WebapiBufferExec[size_WebapiBufferExec-2]=scode_message_WebapiBufferExec[size_WebapiBufferExec-1];
								scode_message_WebapiBufferExec[size_WebapiBufferExec-1]=scode_message_WebapiBufferExec[size_WebapiBufferExec];
								if (DEBUG_PRINTF)
									if (Debug_Socket_server_webapi_String) printf("scode_message=>%s\n\n",scode_message_WebapiBufferExec);
								cJSON_Delete(input_json_data_WebapiBufferExec);

								if(WebApiinfo.response_code==409){    
									sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.%s_%s_%s_%s! \n",TimeFormat(),scode_message_WebapiBufferExec,WebApiinfo.response_message,sno,FIRMWARE_VER2);
									if (DEBUG_PRINTF)
										printf("%s\n",write_sys_log_buf_WebapiBufferExec);
									if (DEBUG_PRINTF)
										printf("******** scode ER000000001 response 409!\n");
									
								}else if((WebApiinfo.response_code==200)|(WebApiinfo.response_code==201)){

									if ((WebApiinfo.response_code==201)&((strcmp(WebApiinfo.methodstr,"POST")==0))){
										if (strcmp(GetInterface(),"ppp0")==0){
											sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.%s_%s_%s_%s_4G! \n",TimeFormat(),scode_message_WebapiBufferExec,WebApiinfo.response_message,sno,FIRMWARE_VER2);
										}else{
											sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.%s_%s_%s_%s_lan! \n",TimeFormat(),scode_message_WebapiBufferExec,WebApiinfo.response_message,sno,FIRMWARE_VER2);
										}
									}
									else if ((WebApiinfo.response_code==200)&((strcmp(WebApiinfo.methodstr,"PUT")==0))){
										if (strcmp(GetInterface(),"ppp0")==0){
											sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.%s_%s_%s_%s_4G! \n",TimeFormat(),scode_message_WebapiBufferExec,WebApiinfo.response_message,sno,FIRMWARE_VER2);
										}else{
											sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.%s_%s_%s_%s_lan! \n",TimeFormat(),scode_message_WebapiBufferExec,WebApiinfo.response_message,sno,FIRMWARE_VER2);
										}
									}
									else if ((WebApiinfo.response_code==200)&((strcmp(WebApiinfo.methodstr,"POST")==0))){
										if (strcmp(GetInterface(),"ppp0")==0){
											sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.error_match_%s_%s_%s_%s_4G! \n",TimeFormat(),scode_message_WebapiBufferExec,WebApiinfo.response_message,sno,FIRMWARE_VER2);
										}else{
											sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.error_match_%s_%s_%s_%s_lan! \n",TimeFormat(),scode_message_WebapiBufferExec,WebApiinfo.response_message,sno,FIRMWARE_VER2);
										}
									}
									else{
										if (strcmp(GetInterface(),"ppp0")==0){
											sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.other_issue_%s_%s_%s_%s_%s_%ld_4G! \n",TimeFormat(),scode_message_WebapiBufferExec,WebApiinfo.response_message,sno,FIRMWARE_VER2,WebApiinfo.methodstr,WebApiinfo.response_code);
										}else{
											sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.0ther_issue_%s_%s_%s_%s_%s_%ld_lan! \n",TimeFormat(),scode_message_WebapiBufferExec,WebApiinfo.response_message,sno,FIRMWARE_VER2,WebApiinfo.methodstr,WebApiinfo.response_code);
										}
									}
									if (DEBUG_PRINTF)
										printf("(%s)\n",write_sys_log_buf_WebapiBufferExec);
								}
								else{							
									sprintf(temp_WebapiBufferExec,WebApiinfo.response_message);
									if (strcmp(GetInterface(),"ppp0")==0){
										sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.%s_fail_%s_%s_%s_4G! \n",TimeFormat(),scode_message_WebapiBufferExec,temp_WebapiBufferExec,sno,FIRMWARE_VER2);
									}else{
										sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.%s_fail_%s_%s_%s_lan! \n",TimeFormat(),scode_message_WebapiBufferExec,temp_WebapiBufferExec,sno,FIRMWARE_VER2);
									}
									if (DEBUG_PRINTF) printf("(%s)\n",write_sys_log_buf_WebapiBufferExec);
									if (DEBUG_PRINTF) printf("(%s)\n",temp_WebapiBufferExec);
									if (DEBUG_PRINTF) printf("Scode ER000000001 response other response!\n");															
								}

								WriteToFileSys(FILE_PATH,write_sys_log_buf_WebapiBufferExec);
								// system("cat /var/www/sys.log");
								// WebApiinfo.sys_backup_count++;
								// sync();
								system("cp /var/www/sys.log /var/www/sys.logtmp");
								if(flash_write(FLASH_UISLOG, 4096*2, "/var/www/sys.logtmp")){
									if (DEBUG_PRINTF)
										printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
								}									
								else {
									if (DEBUG_PRINTF)
										printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
								}															
								system("rm /var/www/sys.logtmp");
								sync();

								echo_file(write_sys_log_buf_WebapiBufferExec);

							}else {
								if (DEBUG_PRINTF)
									printf("scode ER000000001 Non CURLE_OK!\n");
								input_json_data_WebapiBufferExec=cJSON_Parse(WebApiinfo.jsonstr);			
								sprintf(scode_message_WebapiBufferExec,cJSON_Print(cJSON_GetObjectItem(input_json_data_WebapiBufferExec,"scode")));
								sprintf(sno,cJSON_Print(cJSON_GetObjectItem(input_json_data_WebapiBufferExec,"sno")));
								// printf("scode_message=>%s\n",scode_message);
								size_WebapiBufferExec=strlen(scode_message_WebapiBufferExec);
								scode_message_WebapiBufferExec[size_WebapiBufferExec-2]=scode_message_WebapiBufferExec[size_WebapiBufferExec-1];
								scode_message_WebapiBufferExec[size_WebapiBufferExec-1]=scode_message_WebapiBufferExec[size_WebapiBufferExec];
								cJSON_Delete(input_json_data_WebapiBufferExec);
								sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.%s_%s_%s_%s! \n",TimeFormat(),"curl_non_ok","ER000000001",sno,FIRMWARE_VER2);
								if (DEBUG_PRINTF)
									printf("%s",write_sys_log_buf_WebapiBufferExec);								
								WriteToFileSys(FILE_PATH,write_sys_log_buf_WebapiBufferExec);
								// WebApiinfo.sys_backup_count++;								
								system("cp /var/www/sys.log /var/www/sys.logtmp");
								if(flash_write(FLASH_UISLOG, 4096*2, "/var/www/sys.logtmp"))
								{
									if (DEBUG_PRINTF)
										printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
								}							
								else{
									if (DEBUG_PRINTF)
										printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
								}						
								system("rm /var/www/sys.logtmp");
								echo_file(write_sys_log_buf_WebapiBufferExec);									
							}
							
						} // curl non ok retry 3 time
						
												
						WebApiinfo.Curl_exec=false;					
						
					}
				}


			}
			else if (strstr(inputstrinngs_WebapiBufferExec, PUSH_DATA)!=NULL)
			{
				if (DEBUG_PRINTF)
					printf("\nexec pust data!!\n");

				strcpy(push_data_string,inputstrinngs_WebapiBufferExec);
				// push_data_all_size=strlen(push_data_string)-1;
				if (DEBUG_PRINTF)
					printf("**RUN PUSH DATA==>%s)\n",push_data_string);
				// printf(push_data_string);
				// printf("\n");
				gettimeofday (&tv_now , &tz_now);
				utc_now=tv_now.tv_sec;
				utc_data_string=strtok(push_data_string,str1);
				utc_data=atoi(utc_data_string);
				if (DEBUG_PRINTF)
					printf("utc_now=%d,utc_data=%d\n",utc_now,utc_data);

				temp_str=strtok(NULL,str2);
				if (DEBUG_PRINTF)
					printf("strtok==>%s)\n",temp_str);
				strcpy(PushApiinfo.inputstring,temp_str);

				int ParseCurlStringPush_res=ParseCurlStringPushapiExec();

				if (ParseCurlStringPush_res==-1){
					if (DEBUG_PRINTF)
						printf("****************push Parse_fail\n");
					if (flag_push){
						WriteToDayRunOverFile(PUSHDATA_FILE);
						//flag_push PUSHDATA_FILE
					}
					WebApiinfo.Curl_exec=false;
					return -1;
				}else{
					// push_string= strstr(PushApiinfo.datastr," - ");
					// if (DEBUG_PRINTF)
					// 	printf("\n*push massahe=>%s*",push_string);
					
					// strcpy(push_key,push_string);
					// // push_key[19]=0;
					
					// for (i=0;i<(strlen(push_key)-3);i++){
					// 	push_key[i]=push_key[i+3];
					// }
					// if (DEBUG_PRINTF)
					// 	printf("\n\n\n**pushkey(%s)**\n",push_key);

					if((utc_now-utc_data)<PUSH_DATA_CANCEL_SEC){
						if (DEBUG_PRINTF)
							printf("\n**loss 10 min\n");

						int SKSPushApi_res=SKSPushApi();
						if (SKSPushApi_res == -1){
							sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.push_Curl_function_open_fail! \n",TimeFormat());
							if (DEBUG_PRINTF)
								printf("%s",write_sys_log_buf_WebapiBufferExec);

							echo_file(write_sys_log_buf_WebapiBufferExec);	

							WebApiinfo.Curl_exec=false;
							return -2;
						}else if (SKSPushApi_res == CURLE_OK){

							push_string= strstr(PushApiinfo.pushkey," - ");
							if (DEBUG_PRINTF)
								printf("\n*push massahe=>%s*",push_string);

							strcpy(PushApiinfo.pushkey,push_string);

							for (i=0;i<(strlen(PushApiinfo.pushkey)-3);i++){
								PushApiinfo.pushkey[i]=PushApiinfo.pushkey[i+3];
							}
							if (DEBUG_PRINTF)
								printf("\n\n\n**pushkey(%s)**\n",PushApiinfo.pushkey);
						
							if(((PushApiinfo.response_code==200)|(PushApiinfo.response_code==201))|(PushApiinfo.response_code==202)){	

								if ((PushApiinfo.response_code==201)&((strcmp(PushApiinfo.methodstr,"POST")==0))){
									if (strcmp(GetInterface(),"ppp0")==0){
										sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.push_%s_%s_%s_4G! \n",TimeFormat(),PushApiinfo.pushkey,PushApiinfo.response_message,FIRMWARE_VER2);
									}else{
										sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.push_%s_%s_%s_lan! \n",TimeFormat(),PushApiinfo.pushkey,PushApiinfo.response_message,FIRMWARE_VER2);
									}
								}
								else if ((PushApiinfo.response_code==200)&((strcmp(PushApiinfo.methodstr,"PUT")==0))){
									if (strcmp(GetInterface(),"ppp0")==0){
										sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.push_%s_%s_%s_4G! \n",TimeFormat(),PushApiinfo.pushkey,PushApiinfo.response_message,FIRMWARE_VER2);
									}else{
										sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.push_%s_%s_%s_lan! \n",TimeFormat(),PushApiinfo.pushkey,PushApiinfo.response_message,FIRMWARE_VER2);
									}
								}
								else if ((PushApiinfo.response_code==200)&((strcmp(PushApiinfo.methodstr,"POST")==0))){
									if (strcmp(GetInterface(),"ppp0")==0){
										sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.push_error_match_%10s_%s_%s_4G! \n",TimeFormat(),PushApiinfo.pushkey,PushApiinfo.response_message,FIRMWARE_VER2);
									}else{
										sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.push_error_match_%10s_%s_%s_lan! \n",TimeFormat(),PushApiinfo.pushkey,PushApiinfo.response_message,FIRMWARE_VER2);
									}
								}
								else{
									if (strcmp(GetInterface(),"ppp0")==0){
										sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.push_other_issue_%10s_%s_%s_%ld_4G! \n",TimeFormat(),PushApiinfo.pushkey,PushApiinfo.response_message,PushApiinfo.methodstr,PushApiinfo.response_code);
									}else{
										sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.push_other_issue_%10s_%s_%s_%ld_lan! \n",TimeFormat(),PushApiinfo.pushkey,PushApiinfo.response_message,PushApiinfo.methodstr,PushApiinfo.response_code);
									}
								}						
								if (DEBUG_PRINTF)
									printf("%s\n",write_sys_log_buf_WebapiBufferExec);
									PushApiinfo.COUNT_PUSH_NON_OK=0;
								// if (Debug_Socket_server_webapi_String|Debug_file_data_show) {
								// 	printf("****   ****   response 200 201 202!\n");
								// 	printf("******list=\n(WebApiinfo->curlcommandstr=%s)\n(WebApiinfo->jsonstr=%s)\n(WebApiinfo->urlstr=%s)\n( WebApiinfo->response_message=%s)\n",WebApiinfo.curlcommandstr,WebApiinfo.jsonstr,WebApiinfo.urlstr, WebApiinfo.response_message);
								// }
								if (flag_push){
									WriteToDayRunOverFile(PUSHDATA_FILE);
									
								}								
							}
							else{
								// sprintf(temp_WebapiBufferExec,WebApiinfo.response_message);
								if (strcmp(GetInterface(),"ppp0")==0){
									sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.push_other(%s_%s_%d)%s_4G! \n",TimeFormat(),PushApiinfo.GROUP_STRING,PushApiinfo.GW_STRING,PushApiinfo.COUNT_PUSH_NON_OK,PushApiinfo.response_message);

								}else{
									sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.push_other(%s_%s_%d)%s_lan! \n",TimeFormat(),PushApiinfo.GROUP_STRING,PushApiinfo.GW_STRING,PushApiinfo.COUNT_PUSH_NON_OK,PushApiinfo.response_message);

								}
									
								// sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.push(%s)%s_%s! \n",TimeFormat(),FIRMWARE_VER2,push_key,PushApiinfo.response_message);
								
								if (DEBUG_PRINTF){
									printf("%s\n",write_sys_log_buf_WebapiBufferExec);
									printf("********other response!\n");
								}
								PushApiinfo.COUNT_PUSH_NON_OK++;
								if (PushApiinfo.COUNT_PUSH_NON_OK>=MAX_CURL_FAIL){
									PushApiinfo.COUNT_PUSH_NON_OK=0;
									if (flag_push){
										WriteToDayRunOverFile(PUSHDATA_FILE);
										
									}									
								}
							}	
							
							// WebApiinfo.sys_backup_count++;							

							// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf_WebapiBufferExec);
							echo_file(write_sys_log_buf_WebapiBufferExec);
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
							WebApiinfo.Curl_exec=false;
						}else {
							if (DEBUG_PRINTF)
								printf("Non CURLE_OK!\n");
							
							if (strcmp(GetInterface(),"ppp0")==0){
								sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.push_fail(%s_%s_%d)_4G! \n",TimeFormat(),PushApiinfo.GROUP_STRING,PushApiinfo.GW_STRING,PushApiinfo.COUNT_PUSH_NON_OK);

							}else{
								sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.push_fail(%s_%s_%d)_lan! \n",TimeFormat(),PushApiinfo.GROUP_STRING,PushApiinfo.GW_STRING,PushApiinfo.COUNT_PUSH_NON_OK);
							}
							PushApiinfo.COUNT_PUSH_NON_OK++;
							if (PushApiinfo.COUNT_PUSH_NON_OK>=MAX_CURL_FAIL){
								if (flag_push){
									WriteToDayRunOverFile(PUSHDATA_FILE);
									
								}
								PushApiinfo.COUNT_PUSH_NON_OK=0;
							}
							
							if (DEBUG_PRINTF)
								printf("%s",write_sys_log_buf_WebapiBufferExec);
							echo_file(write_sys_log_buf_WebapiBufferExec);							
							
							// WebApiinfo.sys_backup_count++;						
							// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf_WebapiBufferExec);
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
							// WriteToDayRunPauseFile(temp_time_day_WebapiBufferExec, WebApiinfo.curlcommandstr, WebApiinfo.jsonstr,WebApiinfo.urlstr, WebApiinfo.response_message);
							
							WebApiinfo.Webapi_run=false;
							WebApiinfo.Curl_exec=false;
							return -3;
						}
						

					}else{
						if (DEBUG_PRINTF)
							printf("\n**more 10 min\n");
						sprintf(write_sys_log_buf_WebapiBufferExec,"%s - aw.%s%s! \n",TimeFormat(),"non_push(>10)",PushApiinfo.pushkey);
						
						echo_file(write_sys_log_buf_WebapiBufferExec);
						// WebApiinfo.sys_backup_count++;
						// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf_WebapiBufferExec);
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
						if (flag_push){
							WriteToDayRunOverFile(PUSHDATA_FILE);
							
						}						
						
					}					
					WebApiinfo.Curl_exec=false;
					return 0;
				}	
			}
			else{
				if (DEBUG_PRINTF)
					printf("\n\nother data  1737!!\n\n");

				if (flag_push){
					WriteToDayRunOverFile(PUSHDATA_FILE);
					if (DEBUG_PRINTF) printf("\nexec PUSHDATA_FILE\n");
					flag_push=false;
					
				}
				if (flag_http){
								
					WriteToDayRunOverFile(DAYRUN_FILE);
					if (DEBUG_PRINTF) printf("\nexec DAYRUN_FILE\n");
					flag_http=false;							
					//flag_push PUSHDATA_FILE
				}

				WebApiinfo.Curl_exec=false;
				return 0;
			}			
		} else {
			WebApiinfo.Webapi_run=false;
			WebApiinfo.Curl_exec=false;
			return 0;
		}
	}
	return 0;	
}

char Identify_key[1024*80];

void *ServerSocket_thread(void *arg)
{

	char temp_ServerSocket_thread[512];	
	char write_sys_log_buf_ServerSocket_thread[1024];
	// char temp_time_day_ServerSocket_thread[20];	

	socklen_t clilen_ServerSocket_thread;
	struct sockaddr_in servaddr_ServerSocket_thread,cliaddr_ServerSocket_thread;
	int i_ServerSocket_thread=0;
	fd_set rset_ServerSocket_thread , allset_ServerSocket_thread;
	ssize_t n_ServerSocket_thread, ret_ServerSocket_thread;
	int count_i_ServerSocket_thread=0;
	bool bool_write_back_ServerSocket_thread=false;  
	char inputstrings_ServerSocket_thread[DATA_SIZE];
	char last_inputstrings_ServerSocket_data[DATA_SIZE];
	char inputstrings_push_file_ServerSocket_thread[DATA_SIZE];
	
	bool flag_frist_run=true;
	
	
	WebApiinfo.listenfd = socket(AF_INET, SOCK_STREAM, 0);/*IPV4流式协议即TCP协议*/
	
	// printf("1=====thread_ServerSocket=======\n");
	if(WebApiinfo.listenfd < 0)
	{
		sprintf(temp_ServerSocket_thread,"listenfd(<0)=%d fial",WebApiinfo.listenfd);
		
		// sprintf(temp_ServerSocket_thread,"setsockopt(SO_REUSEADDR) failed");

		error_print(temp_ServerSocket_thread);
		// printf("\n\n");
		if (strcmp(GetInterface(),"ppp0")==0){
			sprintf(write_sys_log_buf_ServerSocket_thread,"%s - aw.%s_4G! \n",TimeFormat(),temp_ServerSocket_thread);
		}
		else{
			sprintf(write_sys_log_buf_ServerSocket_thread,"%s - aw.%s_lan! \n",TimeFormat(),temp_ServerSocket_thread);
		}
		if (DEBUG_PRINTF)				
			printf("*****%s*********\n",write_sys_log_buf_ServerSocket_thread);
		// if (Debug_log_file_save){
		// WriteToFileSys(FILE_PATH,write_sys_log_buf_ServerSocket_thread);
		// WebApiinfo.sys_backup_count++;

		// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf_ServerSocket_thread);
		// // sync();
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
		echo_file(write_sys_log_buf_ServerSocket_thread);

		// error_print(temp_ServerSocket_thread);
		printf("\n\n");
		close(WebApiinfo.listenfd);
		WebApiinfo.flag_run_once = true;
		usleep(500);
		if (DEBUG_PRINTF)
			printf("sleep 500us exit !\n");
		// pthread_exit(0);
		exit(0);
	}
	// printf("2====bzero \n\n");
	bzero(&servaddr_ServerSocket_thread,sizeof(servaddr_ServerSocket_thread));
	servaddr_ServerSocket_thread.sin_family = AF_INET;/*IPV4*/
	servaddr_ServerSocket_thread.sin_port = 1234;                                                        
	servaddr_ServerSocket_thread.sin_addr.s_addr = inet_addr(SOCKET_SERVER_IP);/*使用本地环回地址做测试*/
	// servaddr.sin_addr.s_addr = htonl(INADDR_ANY);/*使用本地环回地址做测试*/

	const int enable=1;
	if (setsockopt(WebApiinfo.listenfd,SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))<0){
		sprintf(temp_ServerSocket_thread,"setsockopt(SO_REUSEADDR) failed");

		error_print(temp_ServerSocket_thread);
		// printf("\n\n");
		if (strcmp(GetInterface(),"ppp0")==0){
			sprintf(write_sys_log_buf_ServerSocket_thread,"%s - aw.%s_4G! \n",TimeFormat(),temp_ServerSocket_thread);
		}
		else{
			sprintf(write_sys_log_buf_ServerSocket_thread,"%s - aw.%s_lan! \n",TimeFormat(),temp_ServerSocket_thread);
		}
		if (DEBUG_PRINTF)				
			printf("*****%s*********\n",write_sys_log_buf_ServerSocket_thread);
		
		
		// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf_ServerSocket_thread);
		// // sync();
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
		echo_file(write_sys_log_buf_ServerSocket_thread);

		// WebApiinfo.sys_backup_count++;

		close(WebApiinfo.listenfd);
		WebApiinfo.flag_run_once = true;
		usleep(500);
		if (DEBUG_PRINTF)
			printf("sleep 500us exit !\n");
		// pthread_exit(0);
		exit(0);
	}
			
	/*绑定本地Socket地址*/
	// printf("3====bind \n\n");
	if(bind(WebApiinfo.listenfd, (struct sockaddr*)&servaddr_ServerSocket_thread, sizeof(servaddr_ServerSocket_thread)) < 0){

		sprintf(temp_ServerSocket_thread,"bind()_fail");
		// ExceptionWebApiData(WebApiinfo,temp,++sno);	
		// SKSWebAipi(WebApiinfo);
		// sprintf(write_sys_log_buf_ServerSocket_thread,"%s - aw.%s! \n",TimeFormat(),temp_ServerSocket_thread);
		// printf("%s",write_sys_log_buf_ServerSocket_thread);
		// if (Debug_log_file_save){
		// 	WriteToFileSys("/tmp/usb/sda1/debuglog/sys.log",write_sys_log_buf_ServerSocket_thread);
		// 	sync();
		// }

		error_print(temp_ServerSocket_thread);
		// printf("\n\n");
		if (strcmp(GetInterface(),"ppp0")==0){
			sprintf(write_sys_log_buf_ServerSocket_thread,"%s - aw.%s_4G! \n",TimeFormat(),temp_ServerSocket_thread);
		}
		else{
			sprintf(write_sys_log_buf_ServerSocket_thread,"%s - aw.%s_lan! \n",TimeFormat(),temp_ServerSocket_thread);
		}
		if (DEBUG_PRINTF)				
			printf("*****%s*********\n",write_sys_log_buf_ServerSocket_thread);
		
		echo_file(write_sys_log_buf_ServerSocket_thread);
		// WriteToFileSys(FILE_PATH,write_sys_log_buf_ServerSocket_thread);
		// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf_ServerSocket_thread);
		// sync();
		// system("cp /var/www/sks.log /var/www/sks.logtmp");
		// if(flash_write(FLASH_SKSLOG_START, 4096*2, "/var/www/sks.logtmp")){   //FLASH_SKSLOG_START
		// 	if (DEBUG_PRINTF)
		// 		printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
		// }									
		// else{
		// 	if (DEBUG_PRINTF)
		// 		printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
		// }															
		// system("rm /var/www/sks.logtmp");
		// sync();

		WebApiinfo.sys_backup_count++;		
		close(WebApiinfo.listenfd);
		WebApiinfo.flag_run_once=true;
		usleep(500);
		if (DEBUG_PRINTF)
			printf("sleep 500us exit !\n");
		// pthread_exit(0);
		exit(0);
	}
		
	/*监听连接*/
	// printf("4===listen= \n\n");
	if(listen(WebApiinfo.listenfd, LISTENQ) < 0){
		
		sprintf(temp_ServerSocket_thread,"listen()_fail");
		// ExceptionWebApiData(WebApiinfo,temp,++sno);	
		// SKSWebAipi(WebApiinfo);
		// sprintf(write_sys_log_buf_ServerSocket_thread,"%s - aw.%s! \n",TimeFormat(),temp_ServerSocket_thread);
		// printf("%s",write_sys_log_buf_ServerSocket_thread);
		error_print(temp_ServerSocket_thread);
		// printf("\n\n");

		if (strcmp(GetInterface(),"ppp0")==0){
			sprintf(write_sys_log_buf_ServerSocket_thread,"%s - aw.%s_4G! \n",TimeFormat(),temp_ServerSocket_thread);
		}
		else{
			sprintf(write_sys_log_buf_ServerSocket_thread,"%s - aw.%s_lan! \n",TimeFormat(),temp_ServerSocket_thread);
		}				
		if (DEBUG_PRINTF)
			printf("*****%s*********\n",write_sys_log_buf_ServerSocket_thread);
		echo_file(write_sys_log_buf_ServerSocket_thread);
		// WriteToFileSys(FILE_PATH,write_sys_log_buf_ServerSocket_thread);
		// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf_ServerSocket_thread);
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
		// WebApiinfo.sys_backup_count++;
		// sync();
		close(WebApiinfo.listenfd);
		WebApiinfo.flag_run_once=true;
		usleep(500);
		if (DEBUG_PRINTF)
			printf("sleep 500us exit !\n");
		// pthread_exit(0);
		exit(0);
	}
	

	WebApiinfo.maxfd=WebApiinfo.listenfd;
	WebApiinfo.maxi =-1;
	for (i_ServerSocket_thread=0;i_ServerSocket_thread<FD_SETSIZE; ++i_ServerSocket_thread){
		WebApiinfo.client[i_ServerSocket_thread]=-1;
	}
	FD_ZERO(&allset_ServerSocket_thread);
	FD_SET(WebApiinfo.listenfd , &allset_ServerSocket_thread);

	
	memset(last_inputstrings_ServerSocket_data,0,DATA_SIZE);

	if (DEBUG_PRINTF)
	printf("\nSocket_server set ok while run !!!!\n\n");
	while (true){
		
		rset_ServerSocket_thread = allset_ServerSocket_thread;
		WebApiinfo.nready = select(WebApiinfo.maxfd+1, &rset_ServerSocket_thread , NULL , NULL , NULL);

		if(FD_ISSET(WebApiinfo.listenfd , &rset_ServerSocket_thread)){
			clilen_ServerSocket_thread = sizeof(cliaddr_ServerSocket_thread);
			if (DEBUG_PRINTF)
				printf("\naccpet connection~\n");

			if((WebApiinfo.connfd = accept(WebApiinfo.listenfd , (struct sockaddr *)&cliaddr_ServerSocket_thread , &clilen_ServerSocket_thread)) < 0)
			{

				// sprintf(temp_ServerSocket_thread,"accept error.");
				// ExceptionWebApiData(WebApiinfo,temp,++sno);	
				// SKSWebAipi(WebApiinfo);
				// sprintf(write_sys_log_buf_ServerSocket_thread,"%s - aw.%s! \n",TimeFormat(),temp_ServerSocket_thread);
				// if (DEBUG_PRINTF)
				// 	printf("%s",write_sys_log_buf_ServerSocket_thread);

				// echo_file(write_sys_log_buf_ServerSocket_thread);
				perror("accept error.\n");

				if (strcmp(GetInterface(),"ppp0")==0){
					sprintf(write_sys_log_buf_ServerSocket_thread,"%s - aw.%s_4G! \n",TimeFormat(),"accept_issue");
				}
				else{
					sprintf(write_sys_log_buf_ServerSocket_thread,"%s - aw.%s_lan! \n",TimeFormat(),"accept_issue");
				}
				if (DEBUG_PRINTF)				
					printf("*****%s*********\n",write_sys_log_buf_ServerSocket_thread);
				// if (Debug_log_file_save){
				// WriteToFileSys(FILE_PATH,write_sys_log_buf_ServerSocket_thread);
				
				echo_file(write_sys_log_buf_ServerSocket_thread);

				// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf_ServerSocket_thread);
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

				// WebApiinfo.sys_backup_count++;
				close(WebApiinfo.listenfd);
				WebApiinfo.flag_run_once=true;
				sleep(1);
				if (DEBUG_PRINTF)
					printf("sleep 1s exit pthread!\n");

				pthread_exit(0);
				// exit(1);
			}//if

			printf("\n\nSocket_server accpet a new client: %s:%d\n\n", inet_ntoa(cliaddr_ServerSocket_thread.sin_addr) , cliaddr_ServerSocket_thread.sin_port);
            /*將客戶連結套接字描述符新增到陣列*/
			for(i_ServerSocket_thread=0 ; i_ServerSocket_thread<FD_SETSIZE ; ++i_ServerSocket_thread)
			{
				if(WebApiinfo.client[i_ServerSocket_thread] < 0)
				{
					WebApiinfo.client[i_ServerSocket_thread] = WebApiinfo.connfd;
					break;
				}//if
			}//for

			if(FD_SETSIZE == i_ServerSocket_thread)
			{
				perror("too many connection.\n");

				if (strcmp(GetInterface(),"ppp0")==0){
					sprintf(write_sys_log_buf_ServerSocket_thread,"%s - aw.%s_4G! \n",TimeFormat(),"many_connection");
				}
				else{
					sprintf(write_sys_log_buf_ServerSocket_thread,"%s - aw.%s_lan! \n",TimeFormat(),"many_connection");
				}
				if (DEBUG_PRINTF)				
					printf("*****%s*********\n",write_sys_log_buf_ServerSocket_thread);
				// if (Debug_log_file_save){
				// WriteToFileSys(FILE_PATH,write_sys_log_buf_ServerSocket_thread);
				echo_file(write_sys_log_buf_ServerSocket_thread);
				// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf_ServerSocket_thread);
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
				// WebApiinfo.sys_backup_count++;
				close(WebApiinfo.connfd);
				close(WebApiinfo.listenfd);
				WebApiinfo.flag_run_once=true;

				sleep(1);
				if (DEBUG_PRINTF)
					printf("sleep 1s exit pthread!\n");

				pthread_exit(0);
				// exit(1);
			}//if
			FD_SET(WebApiinfo.connfd , &allset_ServerSocket_thread);
			if(WebApiinfo.connfd > WebApiinfo.maxfd)
				WebApiinfo.maxfd = WebApiinfo.connfd;
			if(i_ServerSocket_thread > WebApiinfo.maxi)
				WebApiinfo.maxi = i_ServerSocket_thread;

			if(--WebApiinfo.nready < 0)
				continue;
		}

		for(i_ServerSocket_thread=0; i_ServerSocket_thread<=WebApiinfo.maxi ; ++i_ServerSocket_thread)
		{
			if((WebApiinfo.sockfdd = WebApiinfo.client[i_ServerSocket_thread]) < 0)
				continue;
			if(FD_ISSET(WebApiinfo.sockfdd , &rset_ServerSocket_thread))
			{
				
				memset(inputstrings_ServerSocket_thread,0,DATA_SIZE);
				n_ServerSocket_thread = read(WebApiinfo.sockfdd , inputstrings_ServerSocket_thread , DATA_SIZE);
				if( n_ServerSocket_thread <= 0)
				{
					// printf("\nread data~~~ \n");
					sprintf(temp_ServerSocket_thread,"read() size(<0)=%d",n_ServerSocket_thread);
					// ExceptionWebApiData(WebApiinfo,temp,++sno);	
					// SKSWebAipi(WebApiinfo);
					sprintf(write_sys_log_buf_ServerSocket_thread,"%s - aw.%s! \n",TimeFormat(),temp_ServerSocket_thread);
					if (DEBUG_PRINTF)
						printf("%s",write_sys_log_buf_ServerSocket_thread);
					
                    close(WebApiinfo.sockfdd);
					if (DEBUG_PRINTF)
						printf("close socket fd!\n");
					
					// printf("\nb FD_CLR~~~ \n");
					FD_CLR(WebApiinfo.sockfdd , &allset_ServerSocket_thread);
					// printf("\nb Clinet set -1~~~ \n");
					WebApiinfo.client[i_ServerSocket_thread] = -1;
					// printf("\n end~~~ \n");
				}else if(n_ServerSocket_thread < 230){
					if (DEBUG_PRINTF)
						printf("\n\nsocket data fail!\n");
				}
				else{

					inputstrings_ServerSocket_thread[n_ServerSocket_thread] = 0;	
					if (strcmp(WebApiinfo.apmib_gw,"123") !=0)				
					if (strncmp(inputstrings_ServerSocket_thread,last_inputstrings_ServerSocket_data,210)!=0){  //避免輸入資料重覆
						
						count_i_ServerSocket_thread++;
						if (count_i_ServerSocket_thread>=2){
							count_i_ServerSocket_thread=0;
							bool_write_back_ServerSocket_thread=true;
						}

						strcat(inputstrings_ServerSocket_thread,"\n");
						if (DEBUG_PRINTF)
							printf("\n**get ht data(%d)=>%s*\n",n_ServerSocket_thread,inputstrings_ServerSocket_thread);	


						if(n_ServerSocket_thread < 230){
							if (DEBUG_PRINTF)
								printf("\n\n*get datd from HT less 230,and don,t use!!!**\n");
							sprintf(temp_ServerSocket_thread,"HT datd size fail(%d)",n_ServerSocket_thread);
							sprintf(write_sys_log_buf_ServerSocket_thread,"%s - aw.%s! \n",TimeFormat(),temp_ServerSocket_thread);
							
							// WebApiinfo.sys_backup_count++;

							// // WriteToFileSys(FILE_PATH,write_sys_log_buf_ServerSocket_thread);

							// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf_ServerSocket_thread);
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

							echo_file(write_sys_log_buf_ServerSocket_thread);


							if (DEBUG_PRINTF)
								printf("*****%s*********\n",write_sys_log_buf_ServerSocket_thread);

						}else{
							if (strstr(inputstrings_ServerSocket_thread, HTTPS_DATA)!=NULL){

								if(run_once){									
									run_once=false;
									printf("\n\n\n************1read usb data back !\n");
									system("cat /tmp/usb/sda1/dayrun/DayRun >> /var/www/DayRun");
									
									// printf("\n\n\n************3read usb data back !\n");
									// system("cat /tmp/usb/sda1/dayrun/DayRun");
									
									printf("rm data!\n");
									system("rm /tmp/usb/sda1/dayrun/DayRun");
									
									// system("cat /tmp/usb/sda1/dayrun/DayRun");
									printf("\n\n\n************5read usb data back !\n");
									
								}
								
								WriteToDayRunFile(inputstrings_ServerSocket_thread,DAYRUN_FILE);//https存檔
								
								sprintf(temp_ServerSocket_thread,"get_https_from_HT(%s)",FIRMWARE_VER2);//接收漢唐資料

								if (strcmp(GetInterface(),"ppp0")==0){
									sprintf(write_sys_log_buf_ServerSocket_thread,"%s - aw.%s_(%s)\n",TimeFormat(),temp_ServerSocket_thread,inputstrings_ServerSocket_thread);

								}else{
									sprintf(write_sys_log_buf_ServerSocket_thread,"%s - aw.%s_(%s) \n",TimeFormat(),temp_ServerSocket_thread,inputstrings_ServerSocket_thread);
								}
								if (DEBUG_PRINTF)
									printf("*****%s*********\n",write_sys_log_buf_ServerSocket_thread);

								echo_file(write_sys_log_buf_ServerSocket_thread);

								if (DEBUG_PRINTF) printf("write usb 1 start time=>%s\n\n",TimeFormat());	
																							
								system("cp /var/www/DayRun /tmp/usb/sda1/dayrun/DayRun");

								if (DEBUG_PRINTF) printf("ls usb DayRun start time=>%s\n\n",TimeFormat());
								system("ls /tmp/usb/sda1/dayrun/DayRun");								

								if (DEBUG_PRINTF)
									printf("\n((inputstrings_ServerSocket_thread(%d)=>%s)\n",n_ServerSocket_thread,inputstrings_ServerSocket_thread);
								sync();
								if (DEBUG_PRINTF)
									printf("\n\n save https data!!\n\n");
								if (flag_frist_run==true){
									WebApiinfo.Webapi_run=false;
									flag_frist_run=false;
								}else{
									WebApiinfo.Webapi_run=true;
								}
								
							}else if (strstr(inputstrings_ServerSocket_thread, PUSH_DATA)!=NULL){

								
								gettimeofday (&tv_now , &tz_now);
								sprintf(inputstrings_push_file_ServerSocket_thread,"%ld %s",tv_now.tv_sec,inputstrings_ServerSocket_thread);
								WriteToDayRunFile(inputstrings_push_file_ServerSocket_thread,PUSHDATA_FILE);//push存檔
								

								sprintf(temp_ServerSocket_thread,"get_push_from_HT(%s)",FIRMWARE_VER2);//接收漢唐資料

								if (strcmp(GetInterface(),"ppp0")==0){
									sprintf(write_sys_log_buf_ServerSocket_thread,"%s - aw.%s_4G! \n",TimeFormat(),temp_ServerSocket_thread);
								}
								else{
									sprintf(write_sys_log_buf_ServerSocket_thread,"%s - aw.%s_lan! \n",TimeFormat(),temp_ServerSocket_thread);
								}

								if (DEBUG_PRINTF)
									printf("*****%s*********\n",write_sys_log_buf_ServerSocket_thread);

								echo_file(write_sys_log_buf_ServerSocket_thread);
								// WebApiinfo.sys_backup_count++;									
								// WriteToFileSys(FILE_PATH,write_sys_log_buf_ServerSocket_thread);
								// system("cp /var/www/sys.log /var/www/sys.logtmp");
								// if(flash_write(FLASH_UISLOG, 4096*2, "/var/www/sys.logtmp")){
								// 	if (DEBUG_PRINTF)
								// 		printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
								// }					
								// else{
								// 	if (DEBUG_PRINTF)
								// 		printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);	
								// }														
								// system("rm /var/www/sys.logtmp");
								// sync();
								
								if (DEBUG_PRINTF)
									printf("\n\nsave pust data!!\n\n");
							}else{
								if (DEBUG_PRINTF)
									printf("\n\nother data 2305!!\n\n");

							}
						}
							
						sprintf(temp_ServerSocket_thread,"200");  //收到資料回寫200  

						// if (bool_write_back)
						
						if((ret_ServerSocket_thread = write(WebApiinfo.sockfdd , temp_ServerSocket_thread , strlen(temp_ServerSocket_thread))) != strlen(temp_ServerSocket_thread))
						{
							sprintf(temp_ServerSocket_thread,"sockfd write back fail!\n");
							// ExceptionWebApiData(WebApiinfo,temp,++sno);	
							// SKSWebiApi(WebApiinfo);
							sprintf(write_sys_log_buf_ServerSocket_thread,"%s - aw.%s! \n",TimeFormat(),temp_ServerSocket_thread);
							if (DEBUG_PRINTF)
								printf("%s",write_sys_log_buf_ServerSocket_thread);

							echo_file(write_sys_log_buf_ServerSocket_thread);

							// WriteToFileSys(SKS_FILE_PATH,write_sys_log_buf_ServerSocket_thread);
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
							// error_print(temp);
							// printf("\n\n");
							// printf("error writing to the sockfd!\n");
							bool_write_back_ServerSocket_thread=false;							
							break;
						}//if
						else{
							bool_write_back_ServerSocket_thread=false;
							// if (Debug_file_data_show)
							if (DEBUG_PRINTF)
								printf("socket server write back string=(%s)\n",temp_ServerSocket_thread);
						}
						// if (WebApiinfo.buffer_residual>=3){
						
						if (DEBUG_PRINTF)
							printf("\nrun webapi data!\n");
						// }							
						
					}
					// char inputstrings_ServerSocket_thread[DATA_SIZE];
					// char last_inputstrings_ServerSocket_data[DATA_SIZE];
					strcpy(last_inputstrings_ServerSocket_data,inputstrings_ServerSocket_thread);

				}//else
				if(--WebApiinfo.nready <= 0)
					break;
			}//if
		}//for

	}
    // close(WebApiinfo->conn);
    // close(WebApiinfo->sockfd);
    return 0;
}




// gcc -g -Wall -O2 Socket_server.c  cJSON.c -lm -lcurl -lpthread -W -o Socket_server_8198c_test_h
//msdk-linux-gcc -I /home/kk/8198c_toolchain/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/include -g -Wall -O2 Socket_server.c libapmib.so cJSON.c -lIOTCAPIs -lNebulaAPIs -lTUTKGlobalAPIs -lm -lcurl -lpthread -L /home/kk/8198c_toolchain/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/lib -Wl,-rpath=/home/kk/8198c_toolchain/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/lib -lpthread -o Socket_server_8198c
//msdk-linux-gcc -I /home/kk/8198c_toolchain/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/include -g -Wall -O2 Socket_server.c libapmib.so cJSON.c -lIOTCAPIs -lNebulaAPIs -lTUTKGlobalAPIs -lm -lcurl -lpthread -L /home/kk/8198c_toolchain/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/lib -Wl,-rpath=/home/kk/8198c_toolchain/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/lib -lpthread -o Socket_server_8198c
//hostname -I
//ifconfig "eth1" | grep "inet addr:" | cut -f2 -d: | cut -f1 -d' '
// ifconfig "enp0s3" | grep "inet " | cut -f2 -dt |cut -f1 -dn
int main(int argc, char *argv[])
{

	//main()
	char *UID_main;
	char buf[512];
	// int shmid_main = 0,shm_j_main=0,shm1_tmp_main=0;
	// char *shm_main = "";
	// char temp_str[128]; 

	pthread_t Thread_ServerSocket;
	pthread_t Thread_pidof;

	PushApiinfo.COUNT_PUSH_NON_OK=0;
	WebApiinfo.COUNT_HTTP_OTHER_FAIL=0;
	// pthread_t Thread_WebApiHeartbeat;
	// pthread_t Thread_check_log;
	// ******************************p***
	
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
		printf("\n\n***SOCKET%s,%s***\n\n",DATE_TIME,FIRMWARE_VER2);

	sprintf(buf,"%s - aw.SocketServer_on_%s_%s! \n",TimeFormat(),DATE_TIME,FIRMWARE_VER2);
	if (DEBUG_PRINTF)
		printf("%s",buf);
	echo_file(buf);
	

	Ampib_data = (Apmib *)malloc(sizeof(Apmib));
	
	if (argc < 2){
		//**********************************************
		// printf("No UID input!!!\n");
		if (DEFAULT_APMIB){

			strcpy(Ampib_data->apmib_gw,DEFAULT_GW);
			strcpy(Ampib_data->apmib_mno,DEFAULT_MNO);
			strcpy(Ampib_data->apmib_UID,DEFAULT_UID);

			UID_main = (char *)Ampib_data->apmib_UID;
			printf("UID(read from parm.) = %s\n",Ampib_data->apmib_UID);

		}else {
			apmib_init();
			apmib_get(MIB_USER_PASSWORD, (void *)Ampib_data->apmib_gw);
			apmib_get(MIB_SNMP_RW_COMMUNITY, (void *)Ampib_data->apmib_mno);
			apmib_get(MIB_PPP_PASSWORD4, (void*)Ampib_data->apmib_UID);
			UID_main = (char *)Ampib_data->apmib_UID;
			if (DEBUG_PRINTF)	
				printf("UID(read from board) = %s\n",Ampib_data->apmib_UID);
		}		
        
	}
	else{
		strcpy(Ampib_data->apmib_gw,DEFAULT_GW);
		strcpy(Ampib_data->apmib_mno,DEFAULT_MNO);
		UID_main = (char *)argv[1];
		strcpy(Ampib_data->apmib_UID,UID_main);
		// printf("else UID = %s\n",UID);
	}
	// WebApiinfo=(WebApiData *)malloc(sizeof(WebApiData));
	strcpy(WebApiinfo.apmib_gw,Ampib_data->apmib_gw);
	strcpy(WebApiinfo.apmib_mno,Ampib_data->apmib_mno);
	strcpy(WebApiinfo.apmib_UID,Ampib_data->apmib_UID);

	
	if (DEBUG_PRINTF)
		printf("***%sSecket Server Gw=>%s)\nmno=>%s)\nUID=>%s)\nCHECK_IOT_TIME=%d)\n",TimeFormat(),WebApiinfo.apmib_gw,WebApiinfo.apmib_mno,WebApiinfo.apmib_UID,CHECK_IOT_TIME);

	WebApiinfo.flag_run_once=true;	

	// pthread_create(&Thread_WebApiHeartbeat, NULL, &WebApiHeartbeat, NULL);
	// pthread_detach(Thread_WebApiHeartbeat);
	pthread_create(&Thread_pidof, NULL, &Pidof_thread_work, NULL);
	pthread_detach(Thread_pidof);

	system("mkdir /tmp/usb/sda1/log");
	system("mkdir /tmp/usb/sda1/dayrun");
	system("echo 1 > /var/tmp/intcont");	
	// sprintf(temp_str,"cp /var/www/sks.log /tmp/usb/sda1/log/%s_sks.log",TimeFormatDay());
	
	// // system();
	// // printf("system command==>%s**\n",temp_str);
	// system(temp_str);
	// printf("upload sks.log\n");
	// system("upload_8198 &");
	// WebApiinfo.sys_backup_count=0;
	
	if (DEBUG_PRINTF) printf("read flash end time=>%s\n",TimeFormat());
	// printf(time_tmp);
	
	while(true){
		usleep(100000);//~= 0.1s
		if(WebApiinfo.flag_run_once){
			WebApiinfo.flag_run_once=false;
			sleep(1);
			pthread_create(&Thread_ServerSocket, NULL, &ServerSocket_thread, NULL);
			pthread_detach(Thread_ServerSocket);
		}
		if (strcmp(WebApiinfo.apmib_gw,"123") !=0){			
			if (WebApiinfo.Webapi_run){
				sleep(1);
				WebapiBufferExec();
			}
		}
		// if (WebApiinfo.sys_backup_count>40){
		// 	sprintf(temp_str,"cp /var/www/sks.log /tmp/usb/sda1/log/%s_sks.log",TimeFormatDay());
		// 	system(temp_str);

		// 	printf("upload sks.log\n");
		// 	system("upload_8198 &");
		// 	WebApiinfo.sys_backup_count=0;
		// 	// if (DEBUG_PRINTF)
		// 	// 	printf("\nsocket server do sys.log backup\n");
		// 	// if (DEBUG_PRINTF)
		// 	// 	printf("remove befor last year log\n");
		// 	// sprintf(temp_str,"rm /tmp/usb/sda1/log/*.log");
		// 	// if (DEBUG_PRINTF)
		// 	// 	printf("exec (%s)\n",temp_str);
		// 	// system(temp_str);
		// }
		
	};   
	return 0;
}


// heartbeat正常運做時 連線指令設定使GW 連線LED常亮
// 例外WEBAPI修正，WEBAPI重傳修正及重傳寫入UIS
// 參數檔數檔設置 SksAgentUtility.h
// heartbeat ip加入版本號
// 修正 GW 外部RESET,EVENT Webapi HTTP  傳送存檔問題
// 增加WEBAPI BUFFER 使WEBAPI指令放入BUFFER 使接收與處理WEBAPI 非同步執行
// 當WEBAPI BUFFER有資料時,Heartbeat WEBAPI 暫停,WEBAPI BUFFER為空時,Heartbeat WEBAPI恢復運行
// 加大WEBAPI BUFFER為100筆,且BUFFER已滿時資料 WEBAPI資料先進先出


// https CURL function
// 524C
// heartbeat first time and each 100 fail,save UIS

// 530C
// http 資料寫檔 放入/tmp/usb/sda1/dailylog/ 資料夾內
// 收資料放入*DayLog 傳送完*RunOver(200,201,409,500) 傳失敗(網路不通)RunFail
// 欲傳送資放入DayRun
// dayname.log 日期檔案存留依據的資料

// 531C
// 使用select()完成socket server 收http資料，每筆資料都是獨立性的CLINET資料且回覆200字串表示接收資料完成

//當主機重置時　ＧＷ為１２３時不傳送ＨＴＴＰ資料


//537C
//AGENT連續攘收HTTPS存入資料,透過CURL傳出有插隊狀況,輸入與傳出空間各自獨立
//偵測SIM網路

//546C
//HEARTBEAT與事件https傳送分開
//利用share memory檢測heartbeat是否運作,無運作重啟heartbeat,有運作即可傳送未送出https資料


//修正VSZ漸漸變大(全域變數)
//heartbeat加入socket_server

//清除不必要的UIS


//560C　
//漢唐輸入資料比對　相同的https資料不處理
//存HTTPS資料到隨身碟版本但隨身碟因電池電源切換而無法工作
//將HTTPS資料放入記憶體BUFFER
// 依先進先出方式執行BUFFER WEBAPI OK
//heartbeat失敗每兩分鐘一次寫入UIS
//HTTPS 次料未送出去寫入UIS
//執後送的HTTP SAVE TO /var/www/DayRun
//APP PUSH meeeage save to /var/www/PushData
//more 10 min adn then don't push dada

//584C
//推播 和受信資料放入同一buffer
//推播使用 curl function
//推播　URL 重新編碼
//加大推播buffer長度

//595C
// 推播HTTP資料網路傳送回覆錯誤時,間隔約兩分鐘後再重送,三次錯誤資料跳過,傳送下一筆
// 推播失敗時,可看推播參數
// 寫入ＵＩＳ時也寫入flash 避免資料遺失


//推播與https資料分開送
//push and https timeout ２０s
//598C
//台灣版與泰國版　受信機與推播網址切換
//修正用戶防火牆造成資料上傳阻塞
//修正　４Ｇ　ＳＩＭ上網開機訊號

//601C
//curl non ok send 3 time
//push no mach fail
//copy sys.log

//602C
//加sno to UIS log
//重置agent訊息寫入到 UIS log
//修正所有檔案啟閉與Cjson的指標應用以避免Momery leak






