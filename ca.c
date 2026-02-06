#include "SksAgentUtility.h"

int command_to_string(char *command,char *out){
    char command_str[64];
    char buffer[1024];
    int i=0;
    sprintf(command_str,"%s %s",command,out);
    
    FILE *fp=popen(command,"r");

    if (fp == NULL){
        perror("ponen() failed");
        return 1;
    }
    while (fgets(buffer,sizeof(buffer),fp)!=NULL){
        i++;
        printf("%d)%s",i,buffer);
        strcat(out,buffer);
    }
    pclose(fp);
    return 0;
}


// msdk-linux-gcc -I/home/kk/8198c_toolchain/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/include -g -Wall -O2 -L/home/kk/8198c_toolchain/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/lib -Wl,-rpath=/home/kk/8198c_toolchain/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/lib ca.c libapmib.so cJSON.c -lIOTCAPIs -lNebulaAPIs -lTUTKGlobalAPIs -lm -lcurl -lpthread -o ca_8198c
// msdk-linux-gcc -I/home/kk/8198c_toolchain/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/include -g -Wall -O2 -L/home/kk/8198c_toolchain/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/lib -Wl,-rpath=/home/kk/8198c_toolchain/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/lib ca.c libapmib.so cJSON.c -lIOTCAPIs -lNebulaAPIs -lTUTKGlobalAPIs -lm -lcurl -lpthread -o ca_8198c
int main(){

    struct stat buffer;
    int exist;
    // char p_buffer[1024];
    char total_string[1024];
    cJSON *input_json_data;
    char *timestamp_string="timestamp";
    char timestamp_data[32];
    int str_len=0;
    int i;
    int timestamp_int;
    struct timeval tv_now; 
    struct timezone tz_now;

    // char sys_command_tmp[128];  

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
		printf("\n\n***CA %s,%s***\n\n",DATE_TIME,FIRMWARE_VER2);


    printf("\ncopy /bin/getca.tar to /var/www/ and xtar (time=>%s)\n",TimeFormat());

    
    system("cp /bin/getca.tar /var/www/");
    if (chdir("/var/www/") == 0) {
        printf("Directory changed to %s\n", "/var/www/");
    } else {
        perror("chdir(/var/www/) failed");
        return 1;
    }
    // sync();
    // sleep(1);
    // system("ls -l");
    
    system("tar xvf getca.tar");
    system("mkdir generated");
    usleep(100);  
    // if (chdir("generated/") == 0) {
    //     printf("Directory changed to %s\n", "generated/");
    // } else {
    //     perror("chdir(generated/www/) failed");
    //     return 1;        
    // }  


    printf("\nget ca.tar to /var/www/ and xtar (time=>%s)\n",TimeFormat());
    if(flash_read(FLASH_GOLDENKEY_START, 4096*2, "ca.tar") == 0){
		if (DEBUG_PRINTF)
			printf("%s: %s (%d)flash read ok\n",__FILE__,__func__,__LINE__);
	}			                    
	else{
		if (DEBUG_PRINTF)
			printf("%s: %s (%d)flash read fail\n",__FILE__,__func__,__LINE__);
	}
    
  
	system("tar xvf ca.tar");
    system("rm ca.tar");
    printf("\nget ca.tar (time=>%s)\n",TimeFormat());
    
    exist = stat("generated/CA_download.pem",&buffer);
    if(exist == 0){
        if (DEBUG_PRINTF)
            printf("%s exist!\n","generated/CA_download.pem");
        
        sync();        
        memset(total_string,0,sizeof(total_string));
        if (command_to_string("/var/www/getExpireDate /var/www/generated/CA_download.pem",total_string)==0){
            printf("total string=>(%s)",total_string);
        }
        input_json_data=cJSON_Parse(total_string);
        memset(timestamp_data,0,sizeof(timestamp_data));
        sprintf(timestamp_data,cJSON_Print(cJSON_GetObjectItem(input_json_data,timestamp_string)));
        printf("timestamp_data=(%s)\n",timestamp_data);
        str_len=strlen(timestamp_data);
        for (i=0;i<str_len-2;i++){
            timestamp_data[i]=timestamp_data[i+1];            
        }
        timestamp_data[i]=0;
        printf("timestamp string=>(%s)\n",timestamp_data);
        timestamp_int=atoi(timestamp_data);
        printf("timestamp int=>(%d)\n",timestamp_int);
        gettimeofday (&tv_now , &tz_now);
				// utc_now=tv_now.tv_sec;
        printf("Uinx timestamp=>%ld",tv_now.tv_sec);
        if ((timestamp_int-tv_now.tv_sec)>(60*60*24*30)){
            printf("expire timestamp=>(%d)\n now timesyamp=>(%ld)\n(%d months remaining to expire)\n",timestamp_int,tv_now.tv_sec,(int)(timestamp_int-tv_now.tv_sec)/(60*60*24*30));
        }else{
            pritnf("The last mnoth and extension certificate....");
            

        }
        
    }else{
        // system("cd /var/www/"); 
        if (chdir("/var/www/") == 0) {
            printf("Directory changed to %s\n", "/var/www/");
        } else {
            perror("chdir(/var/www/) failed");
            return 1;        
        }       
        system("./getCA ./sks.pub ./generated/CA_download.pem ./generated/rsa_key ");
        // system("cd /var/www/generated");
        system("pwd");
        
        
    }
    // if (chdir("/var/www/") == 0) {
    //     printf("Directory changed to %s\n", "/var/www/");
    // } else {
    //     perror("chdir(/var/www/) failed");
    //     return 1;        
    // }
    // system("ls -l");
    // system("ls generated -l");
    return 0;
}