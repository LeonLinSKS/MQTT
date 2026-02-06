
/*
可壓縮至K指令
msdk-linux-gcc -g -Os -I ~/openssl-1.0.2d/include -I ./include -Wall cJSON.c  MqttComm.c mqtt.c mqtt_pal.c create_objects.c ReadZoneMerge.c ReadZoneStatus.c ReadFlash.c RealTime.c LogManage.c -L ~/openssl-1.0.2d -lpthread -lssl -ldl -lcrypto libapmib.so -lm -D MQTT_USE_BIO -o MqttComm

沒有tls
msdk-linux-gcc -g -I ~/openssl-1.0.2d/include -I ./include -Wall cJSON.c  MqttComm.c mqtt.c mqtt_pal.c create_objects.c ReadZoneMerge.c ReadZoneStatus.c ReadFlash.c RealTime.c LogManage.c -L ./lib -lpthread -lssl -ldl -lcrypto libapmib.so -lm -o MqttComm

有tls
gcc -g -Os -I ./include cJSON.c  MqttComm.c mqtt.c mqtt_pal.c create_objects.c ReadZoneMerge.c ReadZoneStatus.c ReadFlash.c RealTime.c LogManage.c -L /usr/local/lib -lm -lpthread -lssl -lcrypto -Wextra -Wall -std=gnu99 -Wno-unused-parameter -Wno-unused-variable -Wno-duplicate-decl-specifier -D MQTT_USE_BIO -o MqttComm
gcc -g -I ./include cJSON.c  MqttComm.c mqtt.c mqtt_pal.c create_objects.c ReadZoneMerge.c ReadZoneStatus.c ReadFlash.c RealTime.c LogManage.c -L /usr/local/lib -lm -lpthread -lssl -lcrypto -D MQTT_USE_BIO -o MqttComm
沒有tls
gcc -g -I ./include cJSON.c  MqttComm.c mqtt.c mqtt_pal.c create_objects.c ReadZoneMerge.c ReadZoneStatus.c ReadFlash.c RealTime.c LogManage.c -L /usr/local/lib -lm -lpthread -lssl -lcrypto -Wextra -Wall -std=gnu99 -Wno-unused-parameter -Wno-unused-variable -Wno-duplicate-decl-specifier -o MqttComm

SHM 要加-lrt

msdk-linux-gcc -I ~/openssl-1.0.2d/include -I ./include -I /home/kk/8198c_toolchain/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/include -g -Wall -O2 cJSON.c  MqttComm.c mqtt.c mqtt_pal.c create_objects.c ReadZoneMerge.c ReadZoneStatus.c ReadFlash.c RealTime.c LogManage.c libapmib.so  -lm -lssl -ldl -lcrypto -lcurl -lpthread -L /home/kk/8198c_toolchain/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/lib -Wl,-rpath=/home/kk/8198c_toolchain/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/lib -lpthread -o MqttComm
valgrind --leak-check=full ./MqttComm
*/

#include "common.h"
#include "MqttComm.h"
#include "mqtt.h"

#ifdef VM
#include <openssl/ssl.h>
#else
// #include "/usr/local/include/openssl/ssl.h"
#include <openssl/ssl.h>
#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif

// define TLSON,則同時宣告指標
#ifdef TLSON
// #define MQTT_USE_BIO
// const char *ca_file;
// const char *cert_file;
// const char *key_file;
#include "../templates/openssl_sockets.h"
#else
#include "../templates/posix_sockets.h"
#endif

extern char *buf; // 組回JSON值 // todo 寫在這裡不好,容易忘記釋放記憶體
extern int bufFlag;
extern char g_mno[30];
// static void *ptr;
#define MAX_LOGGER_SIZE 3840
#ifdef TLSON
SSL_CTX *ssl_ctx;
BIO *sockfd;
#else
int sockfd;
#endif

FILE *fpChkFile;
FILE *fp_WriteToSHMMQTT;
reconnect_state_t reconnect_state;
pthread_t client_daemon;
int client_daemon_flag = 0; // # 1:表示創建過
int CA_ERROR_flag = 0; 
// SocketData socketDatainfo;
// bool Flag_SID_Login_Run_IOT[4];

/**
 * @brief My reconnect callback. It will reestablish the connection whenever
 *        an error occurs.
 */
// void reconnect_client(struct mqtt_client *client, void **reconnect_state_vptr);

/**
 * @brief The function will be called whenever a PUBLISH message is received.
 */
void publish_callback(void **unused, struct mqtt_response_publish *published);

/**
 * @brief The client's refresher. This function triggers back-end routines to
 *        handle ingress/egress traffic to the broker.
 *
 * @note All this function needs to do is call \ref __mqtt_recv and
 *       \ref __mqtt_send every so often. I've picked 100 ms meaning that
 *       client ingress/egress traffic will be handled every 100 ms.
 */
void *client_refresher(void *client);

/**
 * @brief Safelty closes the \p sockfd and cancels the \p client_daemon before \c exit.
 */
#ifdef TLSON
void exit_reenter(int status, BIO *sockfd, pthread_t *client_daemon);
#else
void exit_reenter(int status, int sockfd, pthread_t *client_daemon);
#endif

void *dehealthy(void *arg);
// void *mqttPing(void *arg);
void *SendLogForMQTT(void *arg);
void* send_buf(void* arg);
void* send_feedback(void* arg);
bool publish_ack = false; // 收到訊息回傳OK的Flag
volatile bool send_buf_flag = false; // 來控制是否需要60秒每秒傳送buf的Flag
volatile bool send_logger_flag = false; // 來控制是否需要60秒每秒傳送buf的Flag

int main(void)
{
    Flash_Data flash_data; 
    ReadFlash(&flash_data); // 調用 ReadFlash 函數初始化全域變數和 Flash_Data
    internet_healthy = online;
    errorCount = 0;
    printf("============== MQTT iot. NOW VERSION === %s ==============\n",VERSION);
    sleep(40);
    printf("iot. GMNO== %s\n",g_mno);
#ifdef DEBUGLPRINT
    printf("iot. Debug Message ON !!!!!!!!!!!!\n");
#endif
#ifdef TLSON
    SSL_load_error_strings();
    ERR_load_BIO_strings();
    OpenSSL_add_all_algorithms();
    SSL_library_init();

// #define ca_file "./cert/ca_certificate.pem"
// #define cert_file "./cert/client_certificate.pem"
// #define key_file "./cert/client_key.pem"
#endif

    strcpy(DeviceInfo.macAddress, (char *)getMACAddress());
    time_t start_time = time(NULL);  // 記錄啟動時間
    MqttExecute();

    while (1)
    {
    usleep(10000); 

       // 計算已運行時間
        time_t current_time = time(NULL);
        double elapsed_hours = difftime(current_time, start_time) / 3600.0;

        // 如果超過7天（168 小時），則關閉程式
        if (elapsed_hours >= 167.98)
        {
            int random_delay = rand() % 20;
            printf("iot. 程式運行時間已達 7天，準備關閉...\n");
            sleep(random_delay);
            exit(0); // 讓 Watchdog 重新啟動
        }
    }

    return 0;
}

void MqttExecute(void)
{
    first_run = flag_yes;
    RawDataInfoFlag = 0;
    bufFlag = 0;
    mqttconnectFlag = connectFail;

#ifdef SEND_LOG_TO_MQTT
    pthread_t Thread_SendLog;
    pthread_create(&Thread_SendLog, NULL, &SendLogForMQTT, NULL);
    pthread_detach(Thread_SendLog);
#endif

    pthread_t Thread_DeHealthy;
    pthread_create(&Thread_DeHealthy, NULL, &dehealthy, NULL);
    pthread_detach(Thread_DeHealthy);
    // #ifdef AUTORECONNECT
    /* build the reconnect_state structure which will be passed to reconnect */
    RealTimeFormat();
    memset(logbuf, 0, lofbufsize);
    sprintf(logbuf, "%s: %s/%s, MQTTComm Start!!! \n", timestring_TimeFormat, __FILE__, __func__);
    WriteToLogFile(logbuf);

    int rec = SetReconnectState(&reconnect_state);
    if (rec == true)
    {
        printf("iot. SetReconnectState success \n");
        mqttconnectFlag = mqttreconnect(&reconnect_state);
        first_run = flag_no;
    }
    else
    {
        printf("iot. SetReconnectState fail \n");
        internet_healthy = offline;
        // 開機offline自己寫
        FILE *fp_SHFInternet;
#ifdef VM
        fp_SHFInternet = fopen(Loacl_name_internet, "w");
#else
        fp_SHFInternet = fopen(UIS_name_internet, "w");
#endif
        // fwrite("off", 4, 1, fp_SHFInternet);
        // sync();
        fprintf(fp_SHFInternet, "off");
        if (fp_SHFInternet != NULL)
            fclose(fp_SHFInternet);
        first_run = flag_no;
    }
    while (rec == false) // 開機是離線,在此等待到連線
    {
#ifdef DEBUGLPRINT
        printf("iot. g\niot. g\niot. g\niot. g\niot. g\niot. g\n");
#endif
        sleep(15); //拉長連線檢測時間
        if (internet_healthy == online) // # 等待連線
        {
            rec = SetReconnectState(&reconnect_state);
            if (rec == true)
            {
                printf("iot.  2. SetReconnectState success \n");
                printf("iot. mqttconnectFlag = %d  ,internet_healthyFlag = %d\n",mqttconnectFlag,internet_healthy);
                mqttconnectFlag = mqttreconnect(&reconnect_state);
                            printf("iot. mqttconnectFlag = %d  ,internet_healthyFlag = %d\n",mqttconnectFlag,internet_healthy);
                mqttconnectFlag = 1 ;
                            printf("iot. mqttconnectFlag = %d  ,internet_healthyFlag = %d\n",mqttconnectFlag,internet_healthy);
                first_run = flag_no;
                break;
            }
            else
            {
                printf("iot.  2. SetReconnectState fail \n");
                internet_healthy = offline;
                // exit(0);  //加上log後不再退出20240528
                memset(logbuf, 0, lofbufsize);
                sprintf(logbuf, "%s: %s/%s/%d, iot.  2. SetReconnectState fail  \n", timestring_TimeFormat, __FILE__, __func__, __LINE__);
                WriteToLogFile(logbuf);
            }
        }
    }
    printf("iot. =========MqttExecute END=============== \n");
    // #else
    // #endif
        sleep(5); //準備開啟監聽
    pthread_t Thread_MQTTListener;
    // int presecond = 3;
    pthread_create(&Thread_MQTTListener, NULL, &mqtt_listener, NULL);
    pthread_detach(Thread_MQTTListener);

}

void publish_callback(void **unused, struct mqtt_response_publish *published)
{

    char *topic_name = (char *)malloc(published->topic_name_size + 1);
    if (!topic_name) 
    {
    fprintf(stderr, "Memory allocation failed\n");
    return;
    }
    memcpy(topic_name, published->topic_name, published->topic_name_size);
    topic_name[published->topic_name_size] = '\0';

    // printf("iot. Received publish('%s'): ", topic_name);

    // 確保 JSON 正確解析
     char *json_str = (char *)malloc(published->application_message_size + 1);
    if (!json_str) {
        fprintf(stderr, "Memory allocation failed\n");
        free(topic_name);
        return;
    }
    memcpy(json_str, published->application_message, published->application_message_size);
    json_str[published->application_message_size] = '\0';


    size_t i;//~! 預留
    uint8_t *app_msg = (uint8_t *)published->application_message;//~! 預留

    printf("\n\n\niot. ~~~~~~!Received publish('%s'): \n", topic_name); //~! 印出TOPIC
    printf("iot. %s\n",json_str);

    // # ====== 取topic action ==============
    char *psave = NULL;
    char *p = strtok_r(topic_name, "/", &psave);
    char getCmd[20] = {0};
    int countAB = 0;
    do
    {
        countAB++;
        // printf("iot. %s\n", p);
        p = strtok_r(NULL, "/", &psave);
#ifdef DEBUGLPRINT
            printf("iot. countAB = %d, segment = %s\n", countAB, p);
#endif
        if (countAB == 3)
            strncpy(getCmd, p, 20);
    } while (p);
    printf("iot. TOPIC getCmd = %s\n", getCmd);

    // # ====== parse Payload ==============
    if (strcmp(getCmd, "action") == 0)
    {
        cJSON *json = cJSON_Parse(json_str);
        if (json != NULL)
        {
            // access the JSON data
            cJSON *pJSONmno = cJSON_GetObjectItemCaseSensitive(json, "MNO");
            if (cJSON_IsString(pJSONmno) && (pJSONmno->valuestring != NULL))
            {
                printf("iot. MNO: %s\n", pJSONmno->valuestring);
            }
            else
            {
                printf("iot. Received publish no mno \n");
                RealTimeFormat();
                memset(logbuf, 0, lofbufsize);
                sprintf(logbuf, "%s: %s/%s/%d, Received publish no mno \n", timestring_TimeFormat, __FILE__, __func__, __LINE__);
                WriteToLogFile(logbuf);
                free(json_str);
                return;
            }
            cJSON *pJSONcommand = NULL;
            cJSON *pJSONurl = NULL;
#ifdef VM
            if (1)
#else
            if (strcmp(pJSONmno->valuestring, g_mno) == 0)
#endif
            {   
                pJSONcommand = cJSON_GetObjectItemCaseSensitive(json, "command");
                if (pJSONcommand == NULL)
                    return;
                if (cJSON_IsString(pJSONcommand) && (pJSONcommand->valuestring != NULL))
                {
                    printf("iot. command: %s\n\n\n", pJSONcommand->valuestring);

                    //逆控指令增加處
                    //重開機指令
                    if (strcmp(pJSONcommand->valuestring, "SKS_SYSTEM_REBOOT") == 0)
                    {
                    //sprintf(logbuf, "%s: %s/%s/%d, Got Reboot Command From MQTT \n", timestring_TimeFormat, __FILE__, __func__, __LINE__);
                    //WriteToLogFile(logbuf);
                    printf("iot. Command SKS_SYSTEM_REBOOT received,  will Reboot system soon.\n\n\n");

                    send_buf_flag = false;     // 放下旗標停止傳送 buf

                    pthread_t send_buf_thread;  // 啟動傳送指令的執行緒來觸發feedback
                    pthread_create(&send_buf_thread, NULL, send_buf, NULL);
                    pthread_detach(send_buf_thread);
                    sleep(5); 
                    system("reboot");
                    }

                    //即時資料傳送指令
                    else if (strcmp(pJSONcommand->valuestring, "SEND_DATA") == 0)
                    {
                    //sprintf(logbuf, "%s: %s/%s/%d, Got Send Data Command FROM IOT \n", timestring_TimeFormat, __FILE__, __func__, __LINE__);
                    //WriteToLogFile(logbuf);
                    printf("iot. Command SKS_SAND_DATA received, starting to send buf every second for ~~~60~~~ seconds.\n\n\n");

                    send_buf_flag = true;  // 設置傳送 buf 的旗標
                    
                    pthread_t send_buf_thread;  // 啟動一個新的執行緒來每秒傳送 buf
                    pthread_create(&send_buf_thread, NULL, send_buf, NULL);
                    pthread_detach(send_buf_thread); 
                    }

                    //停止即時資料傳送指令
                    else if (strcmp(pJSONcommand->valuestring, "SEND_DATA_STOP") == 0)
                    {
                    send_buf_flag = false;     // 放下旗標停止傳送 buf
                    // sprintf(logbuf, "%s: %s/%s/%d, Got STOP to send data Command FROM IOT \n", timestring_TimeFormat, __FILE__, __func__, __LINE__);
                    printf("iot. Command SEND_DATA_STOP received, stopping buf transmission.\n\n\n");
                    // WriteToLogFile(logbuf);
                    
                    pthread_t send_buf_thread;  // 啟動傳送指令的執行緒來觸發feedback
                    pthread_create(&send_buf_thread, NULL, send_buf, NULL);
                    pthread_detach(send_buf_thread);
                    }

                    else if (strcmp(pJSONcommand->valuestring, "TRY_RUN") == 0)
                    {
                    printf("iot. Command TRY_RUN received, try to pass the message.\n");
                    
                    pthread_t send_feedback_thread;  // 啟動傳送指令的執行緒來觸發feedback
                    pthread_create(&send_feedback_thread, NULL, send_feedback, NULL);
                    pthread_detach(send_feedback_thread);
                    }

                    else if (strcmp(pJSONcommand->valuestring, "SYSTEM_READ") == 0)
                    {
                    printf("iot. Command SYSTEM_READ received, try to pass the message.\n");
                    cJSON *pJSONpath = cJSON_GetObjectItemCaseSensitive(json, "path");
                    send_logger_flag = true;
                    if (!cJSON_IsString(pJSONpath) || pJSONpath->valuestring == NULL) 
                    {
                        printf("！！！iot. SYSTEM_READ without path\n");
                        cJSON_Delete(json);
                    }
                    char *log_file_path = strdup(pJSONpath->valuestring);
                    pthread_t send_feedback_thread;  // 啟動傳送指令的執行緒來觸發feedback
                    pthread_create(&send_feedback_thread, NULL, send_feedback, log_file_path);
                    pthread_detach(send_feedback_thread);
                    }

                }

                pJSONurl = cJSON_GetObjectItemCaseSensitive(json, "frm_url");
                if (pJSONurl == NULL)
                    return;
                if (cJSON_IsString(pJSONurl) && (pJSONurl->valuestring != NULL))
                {
                    printf("iot. frm_url: %s\n", pJSONurl->valuestring);

#ifdef VM
                    fp_WriteToSHMMQTT = fopen(Loacl_name, "w");
#else
                    fp_WriteToSHMMQTT = fopen(UIS_name_Upgrade, "w+");
#endif
                    if (fp_WriteToSHMMQTT == NULL) // 20231120
                    {
                        printf("iot. open fail errno = %d reason = %s ---%s\n", errno, strerror(errno), UIS_name_Upgrade);
                        RealTimeFormat();
                        memset(logbuf, 0, lofbufsize);
                        sprintf(logbuf, "%s: %s/%s/%d, open fail errno = %d reason = %s \n", timestring_TimeFormat, __FILE__, __func__, __LINE__, errno, strerror(errno));
                        WriteToLogFile(logbuf);
                        return;
                    }
                    else
                    {
                        fwrite(pJSONurl->valuestring, strlen(pJSONurl->valuestring), 1, fp_WriteToSHMMQTT);
                        sync();
                        fclose(fp_WriteToSHMMQTT);
                    }
                }
            }
            else
            {
                printf("iot. pJSONmno->valuestring = %s, g_mno = %s \n", pJSONmno->valuestring, g_mno);
                // RealTimeFormat();
                // memset(logbuf, 0, lofbufsize);
                // sprintf(logbuf, "%s: %s/%s/%d, mno is different \n", timestring_TimeFormat, __FILE__, __func__, __LINE__);
                // WriteToLogFile(logbuf);
                return;
            }

            cJSON_Delete(json);
        }
        else
        {
            const char *error_ptr = cJSON_GetErrorPtr();
            if (error_ptr != NULL)
            {
                printf("iot. Error: %s\n", error_ptr);
            }
            cJSON_Delete(json);
            perror("wrong command \n");
        }

        // todo print binary code
        // for (i = 0; i < published->application_message_size; i++)
        // {
        //     printf("iot. %02x ", ((uint8_t *)published->application_message)[i]);
        // } //~! 印出MQTT所收到訊息
        // printf("iot. \n ");

        // todo 這裡程式以後就接同TUTK p2p邏輯
        if ((app_msg[0] == 5) & (app_msg[3] == 6))
        {
            printf("iot. \nReceived publish('%s'): ", topic_name); //~! 印出TOPIC

            for (i = 0; i < published->application_message_size; i++)
            {
                printf("iot. %02x ", ((uint8_t *)published->application_message)[i]);
            } //~! 印出MQTT所收到訊息
            printf("iot. \n ");
            // writetoht_flag = true;
        }
        else
        {
            // writetoht_flag = false;
        }
 
        // // write給HT
        // if ((socketDatainfo.conn > -1) & (writetoht_flag))
        // {
        //     write(socketDatainfo.conn, app_msg, 32); // todo 漢唐不會回覆嗎？
        publish_ack = true; //~! ACK舉起  // todo 應該要放送給漢唐後是否成功,不成功送false
        // }
    }
    free(json_str);
    free(topic_name);
    // }
}

void *client_refresher(void *client)
{
    // struct mqtt_client *client = (struct mqtt_client *)arg;  // 轉型為 mqtt_client 指標
        client_daemon_flag = 1;
    while (1)
    {
        mqtt_sync((struct mqtt_client *)client);
        usleep(100000);  // 降低頻率，改為 0.1 秒執行一次
    }
    return NULL;
}

void *mqtt_listener(void *arg)
{
    int presecond_count = 0;
    char pub_topic[100]; // todo 注意長度
    int forloop = 0;
    int conuntA = 0;
    int haveError = 0;
    int retry_count = 0;

    while (1)
    {
        if (publish_ack)
        {
            printf("iot. \n\n Feedback............  OK!\n ");
            publish_ack = false; //~! ACK歸位
        }
#ifdef DEBUGLPRINT
            printf("iot. while Send clock   = %d\n", presecond_count);
#endif
        // 每15分鐘(900檢查一次)
        if ((presecond_count++ % 900) == 0) 
        {
            RealTimeFormat();
            if (internet_healthy != online) // 網路斷線
            {
                RealTimeFormat();
                memset(logbuf, 0, lofbufsize);
                sprintf(logbuf, "%s: %s/%s, Network disconnection ", timestring_TimeFormat, __FILE__, __func__);
                WriteToLogFile(logbuf); 
            }
            else
            {
                deviceNum = ReadAllData();
                if (deviceNum > 0)
                {
                    DeviceInfo = InitialDeviceInfoValue(DeviceInfo);
                    forloop = (deviceNum + 9) / 10; // 避免10個設備多送一空白JSON
                    int b;
                    for (b = 0; b < forloop; b++) // 每10筆傳送
                    {
                        usleep(200000); 
                        int ok = create_JSONobjects(b);
                        if ((buf != NULL) && (ok == 0))
                        {   

#ifdef DEBUGLPRINT
                            printf("iot. %s\n", buf);
#endif
                            printf("iot. Send string length = %u\n", (unsigned)strlen(buf));
                            printf("iot. The total number of times sent out = %d\n", conuntA++);
                            bzero(pub_topic, sizeof(pub_topic));
                            sprintf(pub_topic, "/%s/%s/%s/%s", "iot", "sks", "publish", "rawdatalist");

#ifdef DEBUGLPRINT
                            printf("iot. pub_topic = '%s' \n", pub_topic);
                            printf("iot. mqtt_pub error count = '%d' \n", errorCount);
                            printf("iot. continuous mqtt_pub error count = '%d' \n", errorCount2);
#endif
                            int retry_local = 0;
                            // 進行發送並處理錯誤
                            while (mqtt_publish(&client, pub_topic, buf, strlen(buf), MQTT_PUBLISH_QOS_0) != MQTT_OK)
                            {
                                printf("iot. 884 client.error = %d \n", client.error);
                                fprintf(stderr, "publish error: %s\n", mqtt_error_str(client.error));
                                RealTimeFormat();
                                memset(logbuf, 0, lofbufsize);
                                sprintf(logbuf, "%s: %s/%s, %s \n", timestring_TimeFormat, __FILE__, __func__, mqtt_error_str(client.error));
                                WriteToLogFile(logbuf);


                                if (client.error == MQTT_ERROR_SEND_BUFFER_IS_FULL) {
                                    printf("iot. Buffer full, waiting...\n");
                                    mqtt_sync(&client); // 嘗試同步釋放緩衝
                                    usleep(100 * 1000); // 休息 100ms
                                    if (++retry_local > 5) {
                                        printf("iot. Publish retry exceeded limit.\n");
                                        break;
                                    }
                                } else {
                                    // 其他錯誤如 network error 等
                                    break;
                                }

                                // 設置重試次數和延遲時間
                                retry_count++;
                                if (retry_count > 3) 
                                {
                                    printf("iot. 567 retry count");
                                    exit_reenter(EXIT_FAILURE_mqtt_publish, sockfd, &client_daemon);
                                    // presecond_count = 895; // 設置為 955，這樣加 5秒 後再次達到 900
                                    haveError = 1;
                                    errorCount++; // 錯誤累計
                                    mqttconnectFlag = connectFail;
                                    sleep(60);
                                    break;
                                }
                                sleep(5); // 延遲5秒後重試
                            }
                            retry_count = 0;
                            if (client.error == MQTT_OK)
                            {
                                errorCount2 = 0;
                            }
                            if (buf != NULL && bufFlag) {
                            free(buf);
                            buf = NULL;
                            bufFlag = 0;
                            }
                            usleep(200000); 
                        }
                        else
                        {
                            printf("iot. buf is NULL or Create_JSONobjects fail \n");
                            RealTimeFormat();
                            memset(logbuf, 0, lofbufsize);
                            sprintf(logbuf, "%s: %s/%s, buf is NULL or Create_JSONobjects fail \n", timestring_TimeFormat, __FILE__, __func__);
                            WriteToLogFile(logbuf);
                            if (buf != NULL && bufFlag) {
                            free(buf);
                            buf = NULL;
                            bufFlag = 0;
                            }
                            usleep(200000); 
                        }
                       mqtt_sync(&client);     // 每10筆傳送後清一次緩衝
                       usleep(500000); 
                    }
                    if ((internet_healthy == online) && (mqttconnectFlag == connectFail))
                    {
                        printf("iot. 597Attempting to reconnect MQTT...Flag = %d (0:connectFail,1:connectOK)\n", mqttconnectFlag);
                        mqttconnectFlag = mqttreconnect(&reconnect_state);
                        printf("iot. 599mqttconnectFlag = %d (0:connectFail,1:connectOK)\n", mqttconnectFlag);
                    }

                    if (RawDataInfoFlag)
                    {
                        free(RawDataInfo);
                        RawDataInfoFlag = 0;
                    }
                    if (buf != NULL && bufFlag) {
                        free(buf);
                        printf("iot. 640 FREE FREE FREE FREE FREE\n");
                        buf = NULL;
                        bufFlag = 0;
                    }
                    if (haveError)
                    {
                        errorCount2++;
                    }
                    if (errorCount2 > 3) 
                    {
                        RealTimeFormat();
                        memset(logbuf, 0, lofbufsize);
                        sprintf(logbuf, "%s: %s/%s/%d, Error Count2 > 3\n", timestring_TimeFormat, __FILE__, __func__, __LINE__);
                        WriteToLogFile(logbuf);
                    }
                }
            }
        }
        sleep(1);
    }
    printf("iot. mqtt_listener End\n");
    return NULL;
}

//送訊功能
void* send_buf(void* arg) {
    printf("iot. publish funtion on\n");
    int count = 0;
    char feedback_topic[50];
    snprintf(feedback_topic, sizeof(feedback_topic), "/%s/%s/%s/%s","iot","sks","feedback",g_mno);
    char send_topic[50];
    sprintf(send_topic, "/%s/%s/%s/%s","iot","sks","result",g_mno);

    // 若旗標 false，僅做 200 回應
    if (!send_buf_flag) {
    const char* response = "200";
    printf("iot. Sending feedback to topic: %s\n", response);
    if (mqtt_publish(&client, feedback_topic, response, strlen(response), MQTT_PUBLISH_QOS_0) != MQTT_OK) {
        printf("iot. Failed to publish feedback to feedback topic\n");
        exit_reenter(EXIT_FAILURE_mqtt_publish, sockfd, &client_daemon);
        } else {
        printf("iot. Feedback sent successfully to feedback topic\n");
        }
        return NULL;   // 直接結束函數
    }

    // 若旗標 true，開始傳送資料
    while (count < 60 && send_buf_flag) {
    deviceNum = ReadAllData();
    RealTimeFormat();
    int forloop = (deviceNum + 9) / 10;  // 計算需要的迴圈數，每 10 台設備傳送一次
    int b;

        usleep(200000); 
        for (b = 0; b < forloop; b++) {
            int ok = create_JSONobjects(b);  //有可能
            if ((buf != NULL) && (ok == 0)) {
                #ifdef DEBUGLPRINT
                printf("iot. Send string length = %u\n", (unsigned)strlen(buf));
                printf("iot. pub_topic = '%s' \n", send_topic);
                #endif


                // 傳送 MQTT 訊息並處理錯誤
                usleep(100000); 
                if (mqtt_publish(&client, send_topic, buf, strlen(buf), MQTT_PUBLISH_QOS_0) != MQTT_OK) {
                    #ifdef DEBUGLPRINT
                    printf("iot. Failed to publish, retrying...\n");
                    #endif
                    // 錯誤處理機制可以加上log之類的
                }

                if (buf != NULL && bufFlag) {
                free(buf);
                buf = NULL;
                bufFlag = 0;
                usleep(200000); 
                }
            } else {
                printf("iot. buf is NULL or Create_JSON objects failed \n");
                RealTimeFormat();
                memset(logbuf, 0, lofbufsize);
                sprintf(logbuf, "%s: %s/%s, buf is NULL or Create_JSONobjects fail \n", timestring_TimeFormat, __FILE__, __func__);
                WriteToLogFile(logbuf);
                
                if (buf != NULL && bufFlag) {
                free(buf);
                buf = NULL;
                bufFlag = 0;
                }
            }
        }
        if (RawDataInfoFlag)
        {
            free(RawDataInfo);
            RawDataInfoFlag = 0;
        }
        sleep(2);  // 每2秒傳送一次
        count++;
    }
    

    send_buf_flag = false;  // 傳送完成後重置旗標
    printf("iot. publish funtion off\n\n");
    return NULL;
}

void* send_feedback(void* arg) {
    // printf("iot. feedback funtion on\n");
    char feedback_topic[50];
    snprintf(feedback_topic, sizeof(feedback_topic), "/%s/%s/%s/%s","iot","sks","feedback",g_mno);


    if (send_logger_flag) {
    char *log_file_path = (char *)arg;
    printf("iot. 正在讀取並發送檔案內容: %s\n", log_file_path);

    FILE *fp = fopen(log_file_path, "r");
    if (fp == NULL) 
    {
        printf("iot. 無法開啟檔案: %s\n", log_file_path);
        free(log_file_path);
        return NULL;
    }

    // **讀取檔案內容**
    char logger[MAX_LOGGER_SIZE + 1] = {0};
    size_t bytes_read;
    while ((bytes_read = fread(logger, 1, MAX_LOGGER_SIZE, fp)) > 0) {
        logger[bytes_read] = '\0';
        printf("iot. Read %zu Bytes\n", bytes_read);
    
        if (mqtt_publish(&client, feedback_topic, logger, bytes_read, MQTT_PUBLISH_QOS_0) != MQTT_OK) 
        {
            printf("iot. Can't send data to MQTT\n");
        }
        else 
        {
            printf("iot. Success to send data to MQTT\n");
        }
        sleep(1);  // 避免 MQTT 訊息發送過快
    }           
    free(log_file_path);
    send_logger_flag = false;  
    }
    else{
    const char* response = "222";
    printf("iot. Sending feedback to topic: %s\n", response);
    if (mqtt_publish(&client, feedback_topic, response, strlen(response), MQTT_PUBLISH_QOS_0) != MQTT_OK) {
        printf("iot. Failed to publish feedback to feedback topic\n");
        } else {
        // printf("iot. Feedback sent successfully to feedback topic\n");
        }
    }
    // printf("iot. publish funtion off\n\n");
    return NULL;
}
// 顯示shared memory binary code
// *prog 顯示字串
// *bytes 顯示資料
// n 長度
void display(char *prog, char *bytes, int n)
{
    int i;
    printf("iot. display: %s\n", prog);
    for (i = 0; i < n; i++)
    {
        printf("iot. %02x%c", bytes[i], ((i + 1) % 16) ? ' ' : '\n');
    }
    printf("iot. \n");
}

// hostname to ip
//
int hostname_to_ip(char *hostname, char *ip)
{
    struct hostent *he;
    struct in_addr **addr_list;
    int i;
    #ifdef DEBUGLPRINT 
    printf("iot. hostname = %s\n", hostname);
    #endif

    if ((he = gethostbyname(hostname)) == NULL) // todo gethostbyname這個函數造成記憶體洩漏96byte
    {
        herror("gethostbyname");
        return 0;
    }

    addr_list = (struct in_addr **)he->h_addr_list;

    for (i = 0; addr_list[i] != NULL; i++)
    {
        strcpy(ip, inet_ntoa(*addr_list[i]));
    }
    // printf("iot. ip = %s\n", ip);
    return 1;
}

int mqttreconnect(reconnect_state_t *reconnect_state)
{
    printf("iot. =========mqttreconnect=============== \n");
    #ifdef DEBUGLPRINT 
    printf("iot. reconnect_state->hostname = '%s' \n", reconnect_state->hostname);
    printf("iot. reconnect_state->port = '%s' \n", reconnect_state->port);
    printf("iot. reconnect_state->recv_topic = '%s' \n", reconnect_state->recv_topic);
    printf("iot. reconnect_state->client_id = '%s' \n", reconnect_state->client_id);
    printf("iot. mqttreconnect time = %s\n", timestring_TimeFormat);
    RealTimeFormat();
    #endif
#ifdef TLSON
    if (sockfd != NULL) {
        BIO_free_all(sockfd);
        sockfd = NULL;
    }
    if (ssl_ctx != NULL) {
        SSL_CTX_free(ssl_ctx);
        ssl_ctx = NULL;
    }
    #else
    if (sockfd != -1) {
    close(sockfd);
    sockfd = -1;
    }
#endif
    if (client.error != MQTT_OK) {
        mqtt_disconnect(&client);
        memset(&client, 0, sizeof(client));
    }

    if (RAND_poll() == 0) {
        fprintf(stderr, "iot. Failed to initialize OpenSSL RNG!\n");
        return connectFail;
    }
    unsigned char seed[32];
    if (!RAND_bytes(seed, sizeof(seed))) {
        fprintf(stderr, "iot. OpenSSL RNG failed, adding entropy manually...\n");
        RAND_add(seed, sizeof(seed), sizeof(seed) / 2.0);
    }

    FILE *fp_chkFile;    // 檢查檔案是否存在
#ifdef VM
    fp_chkFile = fopen("./cert/CA_list.pem", "r");
#else
    fp_chkFile = fopen("/var/www/data/CA_list.pem", "r");
#endif
    if (fp_chkFile == NULL)
    {
        printf("iot. Load CA_list fail ,sleep 60 sec\n");
        // fclose(fp_chkFile);
        RealTimeFormat();
        memset(logbuf, 0, lofbufsize);
        CA_ERROR_flag++;
        if(CA_ERROR_flag<3)
        {sprintf(logbuf, "%s: %s/%s/%d, load ca_certificate Failed sleep 60 sec\n", timestring_TimeFormat, __FILE__, __func__, __LINE__);
        sleep(60);
        WriteToLogFile(logbuf);
        };
        exit_reenter(EXIT_FAILURE_load_certificate, sockfd, NULL);
        return connectFail;
    }
    else
    {
        printf("iot. Load CA_list ok\n");
        fclose(fp_chkFile);
    }
#ifdef VM
    fp_chkFile = fopen("./cert/client.pem", "r");
#else
    fp_chkFile = fopen("/var/www/data/client.pem", "r");
#endif
    if (fp_chkFile == NULL)
    {
        printf("iot. Load client fail\n");
        fclose(fp_chkFile);
        RealTimeFormat();
        memset(logbuf, 0, lofbufsize);
        sprintf(logbuf, "%s: %s/%s/%d, load client_certificate Failed\n", timestring_TimeFormat, __FILE__, __func__, __LINE__);
        WriteToLogFile(logbuf);
        exit_reenter(EXIT_FAILURE_load_certificate, sockfd, NULL);
        return connectFail;
    }
    else
    {
        printf("iot. Load client ok\n");
        fclose(fp_chkFile);
    }
#ifdef VM
    fp_chkFile = fopen("./cert/clientkey.pem", "r");
#else
    fp_chkFile = fopen("/var/www/data/clientkey.pem", "r");
#endif
    if (fp_chkFile == NULL)
    {
        printf("iot. Load clientkey fail\n");
        fclose(fp_chkFile);
        RealTimeFormat();
        memset(logbuf, 0, lofbufsize);
        sprintf(logbuf, "%s: %s/%s/%d, load client_key Failed\n", timestring_TimeFormat, __FILE__, __func__, __LINE__);
        WriteToLogFile(logbuf);
        exit_reenter(EXIT_FAILURE_load_certificate, sockfd, NULL);
        return connectFail;
    }
    else
    {
        printf("iot. Load clientkey ok\n");
        fclose(fp_chkFile);
    }
    sleep(3);
    // 非阻塞的 TCP socket
#ifdef TLSON
#ifndef VM
    open_nb_socket(&sockfd, &ssl_ctx, reconnect_state->hostname, reconnect_state->port, "/var/www/data/CA_list.pem", NULL, "/var/www/data/client.pem", "/var/www/data/clientkey.pem");
    // sockfd = open_nb_socket(&ssl_ctx, reconnect_state->hostname, reconnect_state->port, "/var/www/data/CA_list.pem", NULL,"/var/www/data/client.pem", "/var/www/data/clientkey.pem");

#else
    open_nb_socket(&sockfd, &ssl_ctx, reconnect_state->hostname, reconnect_state->port, "./cert/CA_list.pem", NULL, "./cert/client.pem", "./cert/clientkey.pem"); // 憑證放USB
#endif
    if (sockfd == NULL)
    {
        perror("iot.!!! Failed to open socket: \n");
        memset(logbuf, 0, lofbufsize);
        RealTimeFormat();
        sprintf(logbuf, "%s: %s/%s/%d, Failed to open socket \n", timestring_TimeFormat, __FILE__, __func__, __LINE__);
        WriteToLogFile(logbuf);
        sleep(60);
        exit_reenter(EXIT_FAILURE_open_nb_socket, sockfd, NULL);
        return connectFail;
    }
    printf("iot. sockfd = %p \n", sockfd);
    int ack = mqtt_init(&client, sockfd, reconnect_state->sendbuf, reconnect_state->sendbufsz, reconnect_state->recvbuf, reconnect_state->recvbufsz, publish_callback);
    if (ack != 1)
    {
        printf("iot. mqtt_init: %s\n", mqtt_error_str(ack));
        memset(logbuf, 0, lofbufsize);
        RealTimeFormat();
        sprintf(logbuf, "%s: %s/%s/%d, mqtt_init:%s \n", timestring_TimeFormat, __FILE__, __func__, __LINE__, mqtt_error_str(ack));
        WriteToLogFile(logbuf);
        // exit_reenter(EXIT_FAILURE_mqtt_init, sockfd, NULL);
        return connectFail;
    }
    printf("iot. mqtt_init ack = %d \n", ack);
#else
    sockfd = open_nb_socket(reconnect_state->hostname, reconnect_state->port);
    if (sockfd == -1)
    {
        perror("Failed to open socket: ");
        exit(EXIT_FAILURE);
    }
    mqtt_init(&client, sockfd, reconnect_state->sendbuf, reconnect_state->sendbufsz, reconnect_state->recvbuf, reconnect_state->recvbufsz, publish_callback);
#endif

    uint8_t connect_flags = MQTT_CONNECT_CLEAN_SESSION;

    mqtt_connect(&client, reconnect_state->client_id, NULL, NULL, 0, "admin", "admin", connect_flags, 0);   // 20240716keep Alive 數值設為0不隨意斷開連接
    if (client.error != MQTT_OK)
    {
        printf("iot. mqtt_connect: %s\n", mqtt_error_str(client.error));
        RealTimeFormat();
        memset(logbuf, 0, lofbufsize);
        sprintf(logbuf, "%s: %s/%s/%d, mqtt_connect:%s \n", timestring_TimeFormat, __FILE__, __func__, __LINE__, mqtt_error_str(client.error));
        WriteToLogFile(logbuf);
        // exit_reenter(EXIT_FAILURE_mqtt_connect, sockfd, NULL);
        return connectFail;
    }
    printf("iot. mqtt_connect ack = %d \n", client.error);

    client.publish_response_callback_state = &client;

    printf("iot. Connected to MQTT server\n");


    if (pthread_create(&client_daemon, NULL, client_refresher, &client))
    {
        fprintf(stderr, "Failed to start client daemon.\n");
        memset(logbuf, 0, lofbufsize);
        sprintf(logbuf, "%s: %s/%s/%d, pthread_create_daemon: Failed to start client daemon. \n", timestring_TimeFormat, __FILE__, __func__, __LINE__);
        WriteToLogFile(logbuf);
        exit_reenter(EXIT_FAILURE_pthread_create_daemon, sockfd, &client_daemon);
    }
    pthread_detach(client_daemon);

    mqtt_subscribe(&client, reconnect_state->recv_topic, 0);
    if (client.error != MQTT_OK)
    {
        fprintf(stderr, "Subscription error: %s\n", mqtt_error_str(client.error));
        memset(logbuf, 0, lofbufsize);
        sprintf(logbuf, "%s: %s/%s/%d,mqtt_subscribe: %s \n", timestring_TimeFormat, __FILE__, __func__, __LINE__, mqtt_error_str(client.error));
        WriteToLogFile(logbuf);
        exit_reenter(EXIT_FAILURE_mqtt_subscribe, sockfd, &client_daemon);
        BIO_free_all(sockfd);
        sockfd = NULL;
        return connectFail;
    }

    char full_topic[100];
    memset(full_topic, 0, sizeof(full_topic));

    // 手動組出完整 Topic (利用 %.3s 取前三碼)
    sprintf(full_topic, "/%s/%s/%s/%s/%s", "iot", "sks", "syscmd", "action", g_mno);
    mqtt_subscribe(&client, full_topic, 0);

    if (client.error != MQTT_OK)
    {
        // 這裡您可以決定若失敗是否要視為嚴重錯誤，通常印出警告即可
        printf("iot. Warning: Failed to subscribe to full topic: %s\n", full_topic);
    }
    else
    {
        printf("iot. 2. Subscribed to Full Topic: %s\n", full_topic);
    }

    internet_healthy = online;
    printf("iot. Listening for '%s' messages.\n", reconnect_state->recv_topic);
    return connectOK;
}

#ifdef TLSON
void exit_reenter(int status, BIO *sockfd, pthread_t *client_daemon)
{
    printf("iot. EXIT_FAILURE = %d \n", status);
    if ((status == EXIT_FAILURE_mqtt_publish) || (status == EXIT_FAILURE_load_certificate)) // open socket failed,怕連續不間段重連頻繁delay
    {
        sleep(5);
    }

    if (status > EXIT_FAILURE_NULL)
        internet_healthy = offline;
    printf("iot. 987 internet_healthy = %d (0:offline,1:online)\n", internet_healthy);
    if (internet_healthy == offline) // 曾經斷線過後 重新連線
    {
        sleep(60);
        int rec = SetReconnectState(&reconnect_state);
        if (rec == true)
        {
            printf("iot. mqtt reconnect \n");
            mqttreconnect(&reconnect_state);
            first_run = flag_no;
        }
        else
        {
            printf("iot. Set Reconnect State fail \n");
            RealTimeFormat();
            memset(logbuf, 0, lofbufsize);
            sprintf(logbuf, "%s: %s/%s/%d, exit_reenter \n", timestring_TimeFormat, __FILE__, __func__, __LINE__);
            WriteToLogFile(logbuf);
            internet_healthy = offline;
        }
    }
}
#else
void exit_reenter(int status, int sockfd, pthread_t *client_daemon)
{
    if (sockfd != -1)
        close(sockfd);
    if (client_daemon != NULL)
        pthread_cancel(*client_daemon);
    exit(status);
}
#endif

void *dehealthy(void *arg)
{
    FILE *fp_SHFInternet;
    int iSize;
    char *buffer;
    int result;
    while (1)
    { 
        if (first_run == flag_no) // 不是開機第一次
        {
        printf("iot. ===============7 minutes health detection=================\n");
        result = system("ping -c 1 -W 2 8.8.8.8 > /dev/null 2>&1");

        if (result != 0)  // Ping 8.8.8.8 失敗，改測試 168.95.1.1
        {
            sleep(3);
            #ifdef DEBUGLPRINT 
            printf("iot. ping 8.8.8.8 failed, trying 168.95.1.1...\n");
            #endif 
            result = system("ping -c 1 -W 2 168.95.1.1 > /dev/null 2>&1");

            if (result != 0)  // Ping 168.95.1.1 也失敗，改用 curl 測試
            {
                sleep(3);
                #ifdef DEBUGLPRINT 
                printf("iot. ping 168.95.1.1 failed, trying curl...\n");
                #endif 
                result = system("curl -I --connect-timeout 5 https://www.google.com > /dev/null 2>&1");

                if (result != 0)  // curl 也失敗
                {
                    sleep(3); 
                    #ifdef DEBUGLPRINT 
                    printf("iot. offline (ping 8.8.8.8, ping 168.95.1.1, and curl all failed)\n");
                    #endif
                        fp_SHFInternet = fopen(UIS_name_internet, "r+");
                        if (fp_SHFInternet == NULL) {
                            fp_SHFInternet = fopen(UIS_name_internet, "w+"); // 如果文件不存在，則創建
                        }
                        if (fp_SHFInternet == NULL) {
                            perror("fopen failed");
                            return NULL;
                        }

                        fseek(fp_SHFInternet, 0, SEEK_END);
                        iSize = ftell(fp_SHFInternet);
                        rewind(fp_SHFInternet);

                        if (iSize <= 0) {
                            #ifdef DEBUGLPRINT
                            printf("iot. 檔案為空，設定為 offline\n");
                            #endif
                            fclose(fp_SHFInternet);
                            internet_healthy = offline;
                            sleep(120); 
                            continue;
                        }

                        buffer = (char *)malloc(iSize + 1);
                        if (buffer == NULL) {
                            #ifdef DEBUGLPRINT
                            printf("iot. 記憶體分配失敗，設定為 offline\n");
                            #endif
                            fclose(fp_SHFInternet);
                            internet_healthy = offline;
                            sleep(120); 
                            continue;
                        }

                        result = fread(buffer, 1, iSize, fp_SHFInternet);
                        buffer[iSize] = '\0'; // 確保字串結束

                        if (result == 0) {
                            #ifdef DEBUGLPRINT
                            printf("iot. 檔案讀取失敗，buffer 內容為空\n");
                            #endif
                            fclose(fp_SHFInternet);
                            free(buffer);
                            internet_healthy = offline;
                            continue;
                        }

                        if (strncmp(buffer, "off", 3) == 0) {
                            printf("iot. offline \n");
                            internet_healthy = offline;
                            mqttconnectFlag = connectFail; //因為中途斷線偵測不到 所以手動調整程Fail
                        } else {
                            printf("iot. online \n");
                            internet_healthy = online;
                        }

                        free(buffer);
                        fclose(fp_SHFInternet);

                        // 清空文件
                        fp_SHFInternet = fopen(UIS_name_internet, "w");
                        if (fp_SHFInternet != NULL) {
                            fclose(fp_SHFInternet);
                        }
                }
                else  // curl 成功
                {
                    #ifdef DEBUGLPRINT
                    printf("iot. online (curl success)\n");
                    #endif
                    internet_healthy = online;
                }
            }
            else  // Ping 168.95.1.1 成功
            {
                #ifdef DEBUGLPRINT
                printf("iot. online (ping 168.95.1.1 success)\n");
                #endif
                internet_healthy = online;
            }
        }
        else  // Ping 8.8.8.8 成功
        {
            #ifdef DEBUGLPRINT
            printf("iot. online (ping 8.8.8.8 success)\n");
            #endif
            internet_healthy = online;
        }
        mqtt_ping(&client);
        sleep(3);
        if (internet_healthy == online && client.error != MQTT_OK)  // MQTT 連線已丟失
        {
            printf("iot. MQTT ping failed: %s\n", mqtt_error_str(client.error));
            mqttconnectFlag = connectFail;  // 更新 MQTT 連線狀態
        }

        printf("iot. mqttconnectFlag = %d  ,internet_healthyFlag = %d\n",mqttconnectFlag,internet_healthy);

        // 如果網路恢復，但 MQTT 連線失敗，則重新嘗試連線
        if ((internet_healthy == online) && (mqttconnectFlag == connectFail))
        {
            printf("iot. Attempting to reconnect MQTT...\n");
            mqttconnectFlag = mqttreconnect(&reconnect_state);
            printf("iot. mqttconnectFlag = %d  ,internet_healthyFlag = %d\n",mqttconnectFlag,internet_healthy);
            sleep(60);
            errorCount3++;
            if (errorCount3>=3)
            {
            printf("iot.1185 MQTT ping errorCount3: %d\n",errorCount3);
            RealTimeFormat();
            memset(logbuf, 0, lofbufsize);
            sprintf(logbuf, "%s: %s/%s/%d, 1189 exit_reenter \n", timestring_TimeFormat, __FILE__, __func__, __LINE__);
            WriteToLogFile(logbuf);
            exit(0); // 讓 Watchdog 重新啟動
            }
        }

        if (internet_healthy == offline)
        {
            printf("iot.Connect Fail, will retry after 8 min...\n");
            sleep(420);
            mqttconnectFlag = mqttreconnect(&reconnect_state);
            printf("iot. mqttconnectFlag = %d (0:connectFail,1:connectOK)\n", mqttconnectFlag);
        }
        }
        sleep(420); // 每7分鐘執行一次
    }
}



// 設定連線資訊
int SetReconnectState(reconnect_state_t *reconnect_state)
{
    printf("iot. =========SetReconnectState=========== \n");
    char flag_return = 0;
    char client_id[40];
#ifndef IOTMQTT
    char *hostname = "rabbitmq.sksiot.com"; // 測試區
        printf("iot. ~測試區rabbitmq~   ~測試區rabbitmq~    ~測試區rabbitmq~\n");
#else
    char *hostname = "rabbitmq-prod.sksiot.com"; // 正式區
#endif
    char addr[100];
    memset(addr, 0, strlen(addr));
    if (hostname_to_ip(hostname, addr) == 0)
    {
        // exit_reenter(2, sockfd, &client_daemon);
        printf("iot. hostname_to_ip fail\n");
        RealTimeFormat();
        memset(logbuf, 0, lofbufsize);
        sprintf(logbuf, "%s: %s/%s/%d, hostname_to_ip \n", timestring_TimeFormat, __FILE__, __func__, __LINE__);
        WriteToLogFile(logbuf);
        // exit_reenter(EXIT_FAILURE_hostname_to_ip, sockfd, &client_daemon);
        flag_return = 1;
    }

    // const char* topic = "UIS/0692986/#";
    // const char *client_id = "NULL1"; // client ID 重複會當機
    strcpy(client_id, DeviceInfo.macAddress);
    char topic[100];        // todo 注意長度
    uint8_t sendbuf[30000]; // todo 99 sensor 會不夠
    uint8_t recvbuf[30000];
    bzero(topic, strlen((const char *)topic));
    sprintf(topic, "/%s/%s/%s/%s/%.3s", "iot", "sks", "syscmd", "action",g_mno);
    #ifdef DEBUGLPRINT 
    printf("iot. ip address = %s \n", addr);
    #endif
    printf("iot. topic = '%s' \n", topic);

#ifdef TLSON
    const char *port = "8883";
#else
    const char *port = "1883";
#endif
    reconnect_state->hostname = addr;
    reconnect_state->port = port;
    reconnect_state->recv_topic = topic;
    reconnect_state->sendbuf = sendbuf;
    reconnect_state->sendbufsz = sizeof(sendbuf);
    reconnect_state->recvbuf = recvbuf;
    reconnect_state->recvbufsz = sizeof(recvbuf);
    reconnect_state->client_id = client_id;

    if (flag_return == 1)
        return false;
    else
        return true;
}

// 每5秒判斷一次logfile並透過mqtt送出log
void *SendLogForMQTT(void *arg)
{
    char pub_topic_log[100];
    FILE *fp_RWLogFile;
    char chunk[256];

    // 產生測試log用
    // int count = 0;
    // while (1)
    // {
    //     RealTimeFormat();
    //     memset(logbuf, 0, lofbufsize);
    //     sprintf(logbuf, "%s: %s/%s/%d, test%d \n", timestring_TimeFormat, __FILE__, __func__, __LINE__, count++);
    //     WriteToLogFile(logbuf);
    //     sleep(2);
    // }

    while (1)
    {
        if (internet_healthy == online)
        {
#ifdef VM
            fp_RWLogFile = fopen(Loacl_name_log, "r");
#else
            fp_RWLogFile = fopen(UIS_name_log, "r");
#endif
            if (fp_RWLogFile == NULL)
            {
                printf("iot. open fail errno = %d reason = %s \n", errno, strerror(errno));
                RealTimeFormat();
                memset(logbuf, 0, lofbufsize);
                sprintf(logbuf, "%s: %s/%s/%d, open fail errno = %d reason = %s \n", timestring_TimeFormat, __FILE__, __func__, __LINE__, errno, strerror(errno));
                WriteToLogFile(logbuf);
            }
            else
            {
                if (client.error == MQTT_OK)
                {
                    while (fgets(chunk, sizeof(chunk), fp_RWLogFile) != NULL)
                    {
                        // fputs(chunk, stdout);
                        // fputs("|*\n", stdout);
                        bzero(pub_topic_log, strlen((const char *)pub_topic_log));
                        sprintf(pub_topic_log, "/%s/%s/%s/%s", "iot", "sks", "log", g_mno);
                        // printf("iot. pub_topic_log = '%s' \n", pub_topic_log);
                        // printf("iot. chunk size = %ld, chunk = %s \n", strlen(chunk), chunk);
                        mqtt_publish(&client, pub_topic_log, chunk, strlen(chunk), MQTT_PUBLISH_QOS_0);
                        mqttconnectFlag = connectFail;
                        printf("iot. \n\n");
                    }
                    fclose(fp_RWLogFile);

// 送完清除
#ifdef VM
                    fp_RWLogFile = fopen(Loacl_name_log, "w");
#else
                    fp_RWLogFile = fopen(UIS_name_log, "w");
#endif
                    fclose(fp_RWLogFile);
                }
            }
        }
        sleep(5);
    }
}
