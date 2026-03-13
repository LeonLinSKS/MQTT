#include "SksAgentUtility.h"

// #define SIZE_OF_SKS_DATA sizeof(SKS_data)
int sno;

/*处理系统调用中产生的错误*/
void error_print(char * ptr);
/*处理通信结束时回调函数接收到的信号*/
// void quit_tranmission(int sig);
char *TimeFormat();
struct memory {
	char *response;
	size_t size;
};

struct timeval tv_now; 
struct timezone tz_now;
Apmib *Ampib_data;
WebApiData WebApiinfo;
PushApiData PushApiinfo;
char IP4_string[64];

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



int WriteToFileSys(char *filename, char *NewEvent);

char get_ip(){ 
	char ip_data_get_ip4[50];
	char ip_data_get_ip6[50];
	// char *ipstr_get_ip;
	// WebApiData *WebApiinfo=(WebApiData *)arg; 
	struct ifaddrs * ifAddrStruct_get_ip=NULL;
	void * tmpAddrPtr_get_ip=NULL; 
	char addressBuffer_get_ip[INET_ADDRSTRLEN];
	char addressBuffer_inet6_get_ip[INET6_ADDRSTRLEN];
	// int i=0;
	if (0)
		printf("before getifaddrs\n");   
    getifaddrs(&ifAddrStruct_get_ip);
	if (0)
		printf("after getifaddrs before while!\n");
    while (ifAddrStruct_get_ip!=NULL) { 
		if (0)      
			printf("run while\n"); 
        if (ifAddrStruct_get_ip->ifa_addr->sa_family==AF_INET) { // check it is IP4
			if (0)
				printf("IP4 part1\n");
            tmpAddrPtr_get_ip=&((struct sockaddr_in *)ifAddrStruct_get_ip->ifa_addr)->sin_addr;
			if (0)
				printf("IP4 part2\n");
            
			if (0)
				printf("IP4 part3\n");
            inet_ntop(AF_INET, tmpAddrPtr_get_ip, addressBuffer_get_ip, INET_ADDRSTRLEN);
            // printf("%s IP4 Address %s\n", ifAddrStruct_get_ip->ifa_name, addressBuffer_get_ip);
            if (strcmp(INTERFACE_NAME,ifAddrStruct_get_ip->ifa_name)==0){
                strcpy(ip_data_get_ip4,addressBuffer_get_ip);
            }
        } 
        else if (ifAddrStruct_get_ip->ifa_addr->sa_family==AF_INET6) { // check it is IP6
            tmpAddrPtr_get_ip=&((struct sockaddr_in *)ifAddrStruct_get_ip->ifa_addr)->sin_addr;
            
            inet_ntop(AF_INET6, tmpAddrPtr_get_ip, addressBuffer_inet6_get_ip, INET6_ADDRSTRLEN);
            
			// printf("%s IP6 Address %s\n", ifAddrStruct_get_ip->ifa_name, addressBuffer_inet6_get_ip); 
            if (strcmp(INTERFACE_NAME,ifAddrStruct_get_ip->ifa_name)==0){
                strcpy(ip_data_get_ip6,addressBuffer_inet6_get_ip);
            }
        } 
        ifAddrStruct_get_ip=ifAddrStruct_get_ip->ifa_next;
    }
	// if (0)
	if (DEBUG_PRINTF)
    	printf("******IP Adress = %s\n\n\n",ip_data_get_ip4);
	strcpy(IP4_string,ip_data_get_ip4);
	// ipstr_get_ip = ip_data_get_ip4;
    
    return 0;
}
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

int SKSWebApi(){

	CURL        *curl_SKSWebApi;
	CURLcode    res_SKSWebApi;
	char webapi_response_message_SKSWebApi[128];
	// char uis_temp_data[512];
	struct memory chunk_SKSWebApi;	
	// if (Debug_Socket_server_webapi_String) 
	struct curl_slist *headers_SKSWebApi = NULL;
	int i_SKSWebApi;
	long response_code1_SKSWebApi;
	long response_code2_SKSWebApi;
	int res1_SKSWebApi; 
	chunk_SKSWebApi.response=NULL;
	chunk_SKSWebApi.size=0;
	
	const char *pfx_path = "./abc.pfx";
    const char *pfx_password = "12345678";
    const char *cacert_path = "./CA_list.pem";
	//SKSWebApi()    
	curl_global_init(CURL_GLOBAL_ALL);
    curl_SKSWebApi        = curl_easy_init();
	if (DEBUG_PRINTF)
		printf("****************SKSWebApi urlstr=\n(%s)\n",WebApiinfo.urlstr);
    if (curl_SKSWebApi){
		
		curl_easy_setopt(curl_SKSWebApi, CURLOPT_CUSTOMREQUEST, WebApiinfo.methodstr);
        curl_easy_setopt(curl_SKSWebApi, CURLOPT_URL, WebApiinfo.urlstr);        //URL parameter

		
		headers_SKSWebApi = NULL;
		// headers_SKSWebApi = curl_slist_append(headers_SKSWebApi, "postman-token: 59388f84-9771-32de-ae0f-f5234a0ea690");
		// headers_SKSWebApi = curl_slist_append(headers_SKSWebApi, "cache-control: no-cache");
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

				// // 設置 SSL 憑證
				// curl_easy_setopt(curl_SKSWebApi, CURLOPT_SSLCERT, "/var/www/generated/CA_download.pem");
				// // 設置 SSL 私鑰
				// // curl_easy_setopt(curl_SKSWebApi, CURLOPT_SSLKEY, "/var/www/generated/csr.pem");
				// curl_easy_setopt(curl_SKSWebApi, CURLOPT_SSLKEY, "/var/www/generated/rsa_key");
			}else{
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
			res_SKSWebApi = curl_easy_perform(curl_SKSWebApi);                      // default get
			if (DEBUG_PRINTF)
				if (Debug_Socket_server_webapi_String|Debug_file_data_show) 
					printf("***********************SKSWebApi curl_easy_perform() exec after input json data WebApiinfo->jsonstr==>\n(%s)\n",WebApiinfo.jsonstr);
			if (chunk_SKSWebApi.response){
				strcpy(WebApiinfo.outputstr,chunk_SKSWebApi.response);
				// WebApiinfo.outputstr=chunk_SKSWebApi.response;
				if (DEBUG_PRINTF)
					printf("Response output data==>%s\n",WebApiinfo.outputstr);  //******************
			} 
			else{
				if (DEBUG_PRINTF)
					printf("No output!\n");
			}

			if(res_SKSWebApi != CURLE_OK){
				if (DEBUG_PRINTF)
					if (Debug_Socket_server_webapi_String) printf("CURL RETRY index=%d,curl_easy_perform continue(res=%d)!\n",i_SKSWebApi,res_SKSWebApi);
				continue;
			}
			else{
				if (DEBUG_PRINTF)
					if (Debug_Socket_server_webapi_String) printf("CURLE_OK index=%d,curl_easy_perform break(res=%d)!\n",i_SKSWebApi,res_SKSWebApi);
				break;
			}	
		}
		if (i_SKSWebApi==WEBAPI_RETRY){//webapi send fail
			if (DEBUG_PRINTF)
				printf("Webapi retry %d time fail!curl=(%d)\n",i_SKSWebApi,res_SKSWebApi);			
			sprintf(webapi_response_message_SKSWebApi,"send_failed_%s_(%d)! \n",curl_easy_strerror(res_SKSWebApi),res_SKSWebApi);
			strcpy(WebApiinfo.response_message,webapi_response_message_SKSWebApi);

			// sprintf(temp_ServerSocket_thread,"get_size=%d)_from_HT(%s)",n_ServerSocket_thread,FIRMWARE_VER2);//接收漢唐資料
            // sleep(1);

			// sleep(2);
						
			
			fprintf(stderr, "%s(res=%d) !\n",curl_easy_strerror(res_SKSWebApi),res_SKSWebApi);
			if (DEBUG_PRINTF)
				printf("WebApiDataInfo==>%s\n",WebApiinfo.response_message);
			WebApiinfo.webapi_send_fail=true;
			

		}
		else{

			if(CA_ENABLE){
				printf("\n\n*************HEART BEAT CA OK\n\n");
			}		
			
            curl_easy_getinfo(curl_SKSWebApi, CURLINFO_RESPONSE_CODE, &response_code1_SKSWebApi);
			WebApiinfo.response_code = response_code1_SKSWebApi;
			if (DEBUG_PRINTF)
				if (Debug_Socket_server_webapi_String|Debug_file_data_show) 
					printf("***********************SKSWebApi ==>CURLINFO_RESPONSE_CODE status code (%03ld)\n*******************WebApiinfo->jsonstr=%s\n",WebApiinfo.response_code,WebApiinfo.jsonstr);
			if (WebApiinfo.response_code==200){
				sprintf(webapi_response_message_SKSWebApi,"%s(%ld)","At.",WebApiinfo.response_code);				
			}else if (WebApiinfo.response_code==201){
				sprintf(webapi_response_message_SKSWebApi,"%s(%ld)","Cted.",WebApiinfo.response_code);
			}else if (WebApiinfo.response_code==204){
				sprintf(webapi_response_message_SKSWebApi,"%s(%ld)","No_content",WebApiinfo.response_code);
			}else if (WebApiinfo.response_code==400){
				sprintf(webapi_response_message_SKSWebApi,"%s(%ld)","Bad_Request",WebApiinfo.response_code);
			}else if (WebApiinfo.response_code==404){
				sprintf(webapi_response_message_SKSWebApi,"%s(%ld)","Not_Fount",WebApiinfo.response_code);
			}else if (WebApiinfo.response_code==403){
				sprintf(webapi_response_message_SKSWebApi,"%s(%ld)","Forbidden",WebApiinfo.response_code);
			}else if (WebApiinfo.response_code==409){
				sprintf(webapi_response_message_SKSWebApi,"%s(%ld)","Conflict",WebApiinfo.response_code);
			}else if (WebApiinfo.response_code==500){
				sprintf(webapi_response_message_SKSWebApi,"%s(%ld)","Server_Error",WebApiinfo.response_code);
			}else{
				sprintf(webapi_response_message_SKSWebApi,"%s(%ld)","Other_Condition",WebApiinfo.response_code);
			}

			strcpy(WebApiinfo.response_message,webapi_response_message_SKSWebApi);
			// if (Debug_Socket_server_webapi_String) 
			if (DEBUG_PRINTF)
				if (Debug_Socket_server_webapi_String|Debug_file_data_show) 
					printf("***********************SKSWebApi CURLE_OK_resprone=>%s\n*******************WebApiinfo->jsonstr=%s\n",WebApiinfo.response_message,WebApiinfo.jsonstr);				
            res1_SKSWebApi = curl_easy_getinfo(curl_SKSWebApi, CURLINFO_HTTP_CONNECTCODE, &response_code2_SKSWebApi);
            if (DEBUG_PRINTF)
				if ((!res1_SKSWebApi)&response_code2_SKSWebApi) printf("*************8CURLINFO_HTTP_CONNECTCODE status code******************** ==>%03ld,ret==>%3d\n",response_code2_SKSWebApi,res1_SKSWebApi);            
			WebApiinfo.webapi_send_fail=false;
		}
		curl_easy_cleanup(curl_SKSWebApi);
		curl_global_cleanup();	
		// free(chunk_SKSWebApi.response);
		// WebApiinfo->Curl_exeic=false;
		return res_SKSWebApi;
        
    }else{
		if (DEBUG_PRINTF)
        	printf("curl_easy_init fail\n\n");
		curl_global_cleanup();
		// WebApiinfo->Curl_exeic=false;
        return -1;
    }
	// //////////////////
}


void *WebApiHeartbeat(){

	char urlstr_WebApiHeartbeat[1024];	
	// char *httpsStr      ="https:/";
	char *httpStr_WebApiHeartbeat       ="";
	// char *urls          ="wlsecurity999.sks.com.tw";//59.120.36.164
	// char *ips           ="59.120.36.164";
	// char *url           ="wlsecurity059.sks.com.tw";//125.227.145.165
	char url_WebApiHeartbeat[50]           ={0};//125.227.145.165
	// char *ip            ="125.227.145.165";
	char *apiStr_WebApiHeartbeat        ="uisgw/api";
	char *cmd_HeartBeat_WebApiHeartbeat ="HeartBeat";
	char temp_WebApiHeartbeat[128];
	char mno_str_WebApiHeartbeat[5]={0};
	char write_sys_log_buf_WebApiHeartbeat[128];
	char event_data_buf_WebApiHeartbeat[512];
	char *curlcommand_WebApiHeartbeat="curl -m 30 -i -X PUT -H \"Content-Type:application/json\" -d";
	// char ppp0IP[50];
	// bool flag_ip_first_run = true;
	int i_WebApiHeartbeat=0;
	// int j_WebApiHeartbeat=0;
	//WebApiHeartbeat()

	// WebApiData *WebApiinfo=(WebApiData *)arg; 
	
	bool flag_run_one=true;
	// bool flag_webnet_ok_once=true;
	// bool flag_power_on_run=true;


	// int shmid = 0, shm_index = 0;    
    // char *shm = NULL;    
	
	if (FLAG_SSL_BOOL){
		httpStr_WebApiHeartbeat="https:/";
	}
	else{
		httpStr_WebApiHeartbeat="http:/";
	}
	strncpy(mno_str_WebApiHeartbeat,WebApiinfo.apmib_mno,3);	  
	sprintf(url_WebApiHeartbeat,"wlsecurity%s.sks.com.tw",mno_str_WebApiHeartbeat);
	if (DEBUG_PRINTF)
		printf("WebApiinfo->mno=>%s;WebApiinfo->UID=>%s;WebApiinfo->apmib_gw=>%s\n",WebApiinfo.apmib_mno,WebApiinfo.apmib_UID,WebApiinfo.apmib_gw);
	sprintf(urlstr_WebApiHeartbeat,"%s/%s/%s/%s/",httpStr_WebApiHeartbeat,url_WebApiHeartbeat,apiStr_WebApiHeartbeat,cmd_HeartBeat_WebApiHeartbeat);
    // Timerset(HEART_BEAT_SEC,0);//sec,usec	
	// WebApiinfo.urlstr = urlstr_WebApiHeartbeat;
	strcpy(WebApiinfo.urlstr,urlstr_WebApiHeartbeat);                                      ///
	WebApiinfo.ssl_bool = FLAG_SSL_BOOL;
	strcpy(WebApiinfo.methodstr,"PUT");
	// WebApiinfo.methodstr = "PUT";
	sleep(2);	
	WebApiinfo.hb_delay_mod_sec= atoi(WebApiinfo.apmib_mno)%HB_DELAY_MOD_SEC;
	if (DEBUG_PRINTF)
		printf("------WebApiHeartbeat----\n");	
	
	if (strcmp(GetInterface(),"ppp0")==0)
	{
		// sprintf(ppp0IP, system("ifconfig \"ppp0\"| grep \"\"inet addr:\"\" | cut -f2 -d: | cut -f1 -d\" \""));
		// printf("ppp0IP=%s\n",ppp0IP);
		if (DEBUG_PRINTF)
			printf("****ppp0\n");
		sprintf(WebApiinfo.CurrIP,"10.107.100.248");
	}
	else{
		// IP4_string
		get_ip();
		if (DEBUG_PRINTF)
			printf("***dhcp\n");
		sprintf(WebApiinfo.CurrIP,"%s",IP4_string);
		if (DEBUG_PRINTF)
			printf("*****ip==>(%s)\n",WebApiinfo.CurrIP);
				
	}	
	strncpy(mno_str_WebApiHeartbeat,WebApiinfo.apmib_mno,3);	  
	sprintf(url_WebApiHeartbeat,"wlsecurity%s.sks.com.tw",mno_str_WebApiHeartbeat);
	if (DEBUG_PRINTF)
		printf("*******WebApiinfo->mno=>%s;WebApiinfo->UID=>%s;WebApiinfo->apmib_gw=>%s\n",WebApiinfo.apmib_mno,WebApiinfo.apmib_UID,WebApiinfo.apmib_gw);


	if (THAI_ADDR_ENABLE){
		sprintf(urlstr_WebApiHeartbeat,"%s/receiver%s.skscloudsecurity.com/heartbeat",httpStr_WebApiHeartbeat,mno_str_WebApiHeartbeat);
		if (DEBUG_PRINTF)
			printf("Thai HB adderee==>%s\n",urlstr_WebApiHeartbeat);

	}else{
		sprintf(urlstr_WebApiHeartbeat,"%s/%s/%s/%s/",httpStr_WebApiHeartbeat,url_WebApiHeartbeat,apiStr_WebApiHeartbeat,cmd_HeartBeat_WebApiHeartbeat);
	}	
	if (DEBUG_PRINTF)
		printf("******urlstring=>%s\n",urlstr_WebApiHeartbeat);
	sprintf(temp_WebApiHeartbeat,"{\"mno\":\"%s\",\"uid\":\"%s\",\"ip\":\"%s %s\",\"gw\":\"%s\"}",WebApiinfo.apmib_mno,WebApiinfo.apmib_UID,WebApiinfo.CurrIP,FIRMWARE_VER2,WebApiinfo.apmib_gw);
	if (DEBUG_PRINTF)
		printf("******heartbeat json data=%s\n",temp_WebApiHeartbeat);


	sprintf(write_sys_log_buf_WebApiHeartbeat,"%s - HeartBeat_start! \n",TimeFormat());				
	if (DEBUG_PRINTF)
		printf("%s",write_sys_log_buf_WebApiHeartbeat);
	
	if (DEBUG_PRINTF)	
    	printf("****************HB first run!!\n\n");    
	while (true) {
		usleep(1000);
		gettimeofday (&tv_now , &tz_now);
		if (strcmp(WebApiinfo.apmib_gw,"123") !=0){
			if (((tv_now.tv_sec%HEART_BEAT_SEC)==HB_DELAY_SEC+WebApiinfo.hb_delay_mod_sec))		
			{				
				if (flag_run_one){
					flag_run_one=false;
					if (DEBUG_PRINTF)
						printf("exec ntpclient -s -h pool.ntp.org -i 5\n");
					system("ntpclient -s -h pool.ntp.org -i 5");
					// printf("\n\n*******heartbeat count(%d)(utc=%ld);\n",++j_WebApiHeartbeat,tv_now.tv_sec);
					// printf("*****MY ip=%s)****\n",WebApiinfo.CurrIP);
					strcpy(WebApiinfo.methodstr,"PUT");			
					// WebApiinfo.methodstr = "PUT";
					// printf("****method =>%s\n",WebApiinfo.methodstr);
					// WebApiinfo.urlstr = urlstr_WebApiHeartbeat;
					strcpy(WebApiinfo.urlstr,urlstr_WebApiHeartbeat);
					if (DEBUG_PRINTF)
						printf("****hb urlstr =>%s\n",WebApiinfo.urlstr);
					WebApiinfo.ssl_bool = FLAG_SSL_BOOL;
					// if (WebApiinfo.ssl_bool){
					// 	printf("SSL is ture!\n");
					// }else{
					// 	printf("SSL is false!\n");
					// }			
					// WebApiinfo.jsonstr = temp_WebApiHeartbeat;
					strcpy(WebApiinfo.jsonstr,temp_WebApiHeartbeat);
					if (DEBUG_PRINTF)
						printf("****jsonstr =>%s\n",WebApiinfo.jsonstr);
					////////////////////////
					if (SKSWebApi()!=CURLE_OK){
						
						if ((i_WebApiHeartbeat%HEARTBEAT_SEND_FAIL_COUNT)==0)
						{
							sprintf(write_sys_log_buf_WebApiHeartbeat,"%s - heartbeat_curl_NON_OK! \n",TimeFormat());				
							if (DEBUG_PRINTF)
								printf("%s",write_sys_log_buf_WebApiHeartbeat);
							// if (Debug_log_file_save){
								
							// 	WriteToFileSys(FILE_PATH,write_sys_log_buf_WebApiHeartbeat);//write heartbeat message to file;

							// 	system("cp /var/www/sys.log /var/www/sys.logtmp");
							// 	if(flash_write(FLASH_UISLOG, 4096*2, "/var/www/sys.logtmp")){
							// 		if (DEBUG_PRINTF)
							// 			printf("%s: %s (%d)flash_write fail\n",__FILE__,__func__,__LINE__);
							// 	}
									
							// 	else{
							// 		if (DEBUG_PRINTF)
							// 			printf("%s: %s (%d)flash_write OK\n",__FILE__,__func__,__LINE__);
							// 	}															
							// 	system("rm /var/www/sys.logtmp");
							// 	sync();
								
							// }					
							i_WebApiHeartbeat=0;
						}
						i_WebApiHeartbeat++;
						if (DEBUG_PRINTF)
							printf("send fail! Event process.\n");
										
						sprintf(event_data_buf_WebApiHeartbeat,"%s '%s' %s\n",curlcommand_WebApiHeartbeat,temp_WebApiHeartbeat,urlstr_WebApiHeartbeat);
						if (DEBUG_PRINTF)
							printf("hartbeat event str==>(%s)\n",event_data_buf_WebApiHeartbeat);
						// ex->ct_cnt[2]=0;
						
						system("echo 0 > /var/tmp/intcont");
						sleep(1);

						sprintf(event_data_buf_WebApiHeartbeat,"hb_web_fail(%s)",FIRMWARE_VER2);

						if (strcmp(GetInterface(),"ppp0")==0){
							sprintf(write_sys_log_buf_WebApiHeartbeat,"%s - aw.%s_4G! \n",TimeFormat(),event_data_buf_WebApiHeartbeat);

						}else{
							sprintf(write_sys_log_buf_WebApiHeartbeat,"%s - aw.%s_lan! \n",TimeFormat(),event_data_buf_WebApiHeartbeat);
						}						
						printf("*****%s**\n",write_sys_log_buf_WebApiHeartbeat);
						// if (Debug_log_file_save){

						
						WriteToFileSys(FILE_PATH,write_sys_log_buf_WebApiHeartbeat);//write heartbeat message to file;

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
						echo_file(write_sys_log_buf_WebApiHeartbeat);					
						// flag_webnet_ok_once=true;										
					}
					else{
						i_WebApiHeartbeat=0;
						// printf("******curlcommand=>%s\n",curlcommand_WebApiHeartbeat);
						// printf("%s\n",temp);
						// printf("%s\n",urlstr);
						// sprintf(write_sys_log_buf_WebApiHeartbeat,"%s - Heartbeat(agent)_%s \n",TimeFormat(),WebApiinfo.response_message);
						// printf("*****(%s)\n",write_sys_log_buf_WebApiHeartbeat);
												
						// if (flag_webnet_ok_once){
							// sprintf(event_data_buf_WebApiHeartbeat,"wan_port_connected(%s)",FIRMWARE_VER2);
							
						if ((WebApiinfo.response_code==200)){
							system("echo 1 > /var/tmp/intcont");
							sleep(2);
							WriteToFileSys(RUN_PATH,"aaaa");							
						}
						else{
							system("echo 0 > /var/tmp/intcont");
							sleep(2);
							sprintf(event_data_buf_WebApiHeartbeat,"Hb_code=%ld(%s)",WebApiinfo.response_code,FIRMWARE_VER2);
							sprintf(write_sys_log_buf_WebApiHeartbeat,"%s - aw.%s! \n",TimeFormat(),event_data_buf_WebApiHeartbeat);
							if (DEBUG_PRINTF)
								printf("*****%s**\n",write_sys_log_buf_WebApiHeartbeat);
							// if (Debug_log_file_save){
							WriteToFileSys(FILE_PATH,write_sys_log_buf_WebApiHeartbeat);

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
							echo_file(write_sys_log_buf_WebApiHeartbeat);
						}
							
					}
					// system("ps");
				}
			}
			else{
				flag_run_one=true;
			}
		}
		
	}
	pthread_exit(0);		
}

char Identify_key[1024*80];
// gcc -g -Wall -O2 Socket_server.c  cJSON.c -lm -lcurl -lpthread -W -o Socket_server_8198c_test_h
// msdk-linux-gcc -I /home/kk/8198c_toolchain/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/include -g -Wall -O2 Heartbeat.c libapmib.so cJSON.c -lIOTCAPIs -lNebulaAPIs -lTUTKGlobalAPIs -lm -lcurl -lpthread -L /home/kk/8198c_toolchain/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/lib -Wl,-rpath=/home/kk/8198c_toolchain/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/lib -lpthread -o Heartbeat_8198c
//hostname -I
//ifconfig "eth1" | grep "inet addr:" | cut -f2 -d: | cut -f1 -d' '
int main(int argc, char *argv[])
{
	char *UID_main;
	char buf[512];
	// int shmid_main = 0,shm_j_main=0,shm1_tmp_main=0;
	// char *shm_main = ""; 

	// pthread_t Thread_ServerSocket;
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
		printf("\n////HEARTBEAT%s,%s/////\n",DATE_TIME,FIRMWARE_VER2);

	sprintf(buf,"%s - aw.Heartbeat_on_%s_%s! \n",TimeFormat(),DATE_TIME,FIRMWARE_VER2);
	if (DEBUG_PRINTF)
		printf("%s",buf);
	// echo_file(buf);
	pthread_t Thread_WebApiHeartbeat;

	// pthread_t Thread_check_log;

	// FILE *fp_sys;
    // int Rows=0;    
    // char sys_file_buf[DATA_COUNT][DATA_SIZE];
    // int i;
	// char *p;
	// char *s1=WDATA;
	// char *filename=FILE_PATH;


	// ******************************p***
	

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
		printf("***Secket Server Gw=>%s,mno=>%s,UID=>%s\n",WebApiinfo.apmib_gw,WebApiinfo.apmib_mno,WebApiinfo.apmib_UID);

    // if( ( shmid_main = shmget( Key, SHMSZ, IPC_CREAT | 0666 ) ) < 0 ){
    //     perror( "shmget" );
    //     exit(1);
    // }   
    
    // if( ( shm_main = shmat( shmid_main, NULL, 0 ) ) == (char *)-1 ){
    //     perror( "shmat" );
    //     exit(1);
    // }   
	// pthread_create(&Thread_ServerSocket, NULL, &ServerSocket_thread, NULL);
	// pthread_detach(Thread_ServerSocket);
	pthread_create(&Thread_WebApiHeartbeat, NULL, &WebApiHeartbeat, NULL);
	pthread_detach(Thread_WebApiHeartbeat);
	// pthread_create(&Thread_check_log, NULL, &CheckLog_thread, NULL);
	// pthread_detach(Thread_check_log);    
	
	while(true){
		// usleep(100000);//~= 0.1s
		sleep(5);
		// printf("disconnect time==>%s\n",TimeFormat());
		// system("echo 0 > /var/tmp/intcont");
		// sleep(172);
		// for (i=0;i<DATA_COUNT;i++){
        // memset(sys_file_buf[i],0,DATA_SIZE);
		// }	
		// if (Debug_Socket_server_webapi_String) printf("filename=%s\n", filename);
		// if (NULL == (fp_sys = fopen(filename, "r"))){
		// 	printf("no file %s\n",filename);
		// 	// fp_sys = fopen("sys.log", "w");       
		// }
		// else{
		// 	Rows = 0;
		// 	i=0;
		// 	while (fgets(sys_file_buf[Rows], sizeof(sys_file_buf[Rows]), fp_sys) != NULL){
				
		// 		// printf("%d)%s",i,sys_file_buf[Rows]); //
		// 		i++;
		// 		p = strstr(sys_file_buf[Rows], s1);
		// 		if (p){
		// 			// printf("agent string =(%s)! ",p);
		// 		}
		// 		else{
		// 			Rows++;
		// 		}              
		// 		if (i==DATA_COUNT){
		// 			break;
		// 		}          
		// 	}
		// 	// p=strstr(sys_file_buf[Rows],s1);
		// 	// if (p){
		// 	// }
		// 	// printf("\n\n");
		// 	for (i=0;i<Rows;i++){
		// 		// printf("%d)%s",i,sys_file_buf[i]);
		// 	}


		// 	if (Debug_Socket_server_webapi_String) printf("\nTotal Rows=%d \n", i); //		
		// }

		// fp_sys = fopen(filename, "w+"); //
		// for (i=0;i<(Rows);i++){
		// 	// printf("%d)%s))\n",i+1,sys_file_buf[i]);
		// 	fwrite(sys_file_buf[i],strlen(sys_file_buf[i]),1,fp_sys);	
		// }
		
		// // usleep(100);
		// fclose(fp_sys);
		// sync();
	} 
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

///////
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

//heartbeat通訊完成檢查webapi Buffer
//網路連線狀況變化由漢唐寫入一次
//網路正常heartbeat傳訊失敗回傳碼寫入ＵＩＳ，成功不需另外寫入

//597C
//網路不通　持繼寫入HB web fail比Ｗan port disconnect 慢一秒
//每兩分鐘+30+0~19秒傳送
// 寫入ＵＩＳ時也寫入flash 避免資料遺失

//未
// ip display fail
