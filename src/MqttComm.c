/* MQTT runtime entry and worker logic */

#include "common.h"
#include "MqttComm.h"
#include "mqtt.h"

#ifdef VM
#include <openssl/ssl.h>
#else
#include <openssl/ssl.h>
#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif

#ifdef TLSON
#include "../templates/openssl_sockets.h"
#else
#include "../templates/posix_sockets.h"
#endif

// 全域變數宣告
extern char *buf;
extern int bufFlag;
extern char g_mno[30];
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
int client_daemon_flag = 0;
int CA_ERROR_flag = 0; 

/**
 * 以下為執行緒與核心函數宣告
 */
/* 處理收到 MQTT 訊息 (PUBLISH) 時的回呼函數 */
void publish_callback(void **unused, struct mqtt_response_publish *published);

/* 負責定期調用 mqtt_sync，保持 MQTT client 的收發緩衝區順暢運行 */
void *client_refresher(void *client);

/* 當發生嚴重錯誤時，安全關閉 socket 與執行緒並準備重啟連線 */
#ifdef TLSON
void exit_reenter(int status, BIO *sockfd, pthread_t *client_daemon);
#else
void exit_reenter(int status, int sockfd, pthread_t *client_daemon);
#endif

/* 網路健康度監控執行緒：定期檢查外網連線，並觸發重連機制 */
void *dehealthy(void *arg);
/* 選用的日誌發送執行緒 (需開啟 SEND_LOG_TO_MQTT) */
void *SendLogForMQTT(void *arg);
/* 密集傳送資料的執行緒 (用於 SEND_DATA 指令) */
void* send_buf(void* arg);
/* 傳送指令執行結果或日誌檔回饋的執行緒 */
void* send_feedback(void* arg);

// 執行緒控制標籤
bool publish_ack = false;
volatile bool send_buf_flag = false;
volatile bool send_logger_flag = false;

/**
 * @brief 程式進入點：讀取設定、啟動 MQTT 核心，並設有 7 天定時重啟的安全機制。
 */
int main(void)
{
    Flash_Data flash_data; 
    ReadFlash(&flash_data); // 讀取 Flash 內的設定資料並存入 Flash_Data
    internet_healthy = online;
    errorCount = 0;
    
    printf("============== MQTT iot. NOW VERSION === %s ==============\n",VERSION);
    sleep(40); // 確保系統網路與周邊已啟動完畢
    printf("iot. GMNO== %s\n",g_mno);

#ifdef DEBUGLPRINT
    printf("iot. Debug Message ON !!!!!!!!!!!!\n");
#endif

#ifdef TLSON
    // 初始化 OpenSSL 函式庫
    SSL_load_error_strings();
    ERR_load_BIO_strings();
    OpenSSL_add_all_algorithms();
    SSL_library_init();
#endif

    // 取得並記錄設備 MAC 地址
    strcpy(DeviceInfo.macAddress, (char *)getMACAddress());
    time_t start_time = time(NULL);  // 記錄程式啟動時間
    
    // 啟動 MQTT 核心邏輯
    MqttExecute();

    // 軟體 Watchdog 迴圈：定期檢查程式運行時間
    while (1)
    {
        sleep(10); 
        time_t current_time = time(NULL);
        double elapsed_hours = difftime(current_time, start_time) / 3600.0;
        
        // 若程式運行超過約 7 天 (167.98 小時)，進行隨機延遲後重啟，避免記憶體洩漏或未知當機
        if (elapsed_hours >= 24)
        {
            int random_delay = rand() % 20;
            printf("iot. Program has been running for 24 hours, preparing to restart...\n");
            sleep(random_delay);
            exit(0); // 退出讓外部的 Watchdog 或守護進程將其重啟
        }
    }

    return 0;
}

/**
 * @brief 初始化連線狀態，並啟動背景監控與 MQTT 資料接收/發送執行緒。
 */
void MqttExecute(void)
{
    first_run = flag_yes;
    RawDataInfoFlag = 0;
    bufFlag = 0;
    mqttconnectFlag = connectFail;

#ifdef SEND_LOG_TO_MQTT
    // 啟動傳送日誌的背景執行緒
    pthread_t Thread_SendLog;
    pthread_create(&Thread_SendLog, NULL, &SendLogForMQTT, NULL);
    pthread_detach(Thread_SendLog);
#endif

    // 啟動網路健康度監控執行緒
    pthread_t Thread_DeHealthy;
    pthread_create(&Thread_DeHealthy, NULL, &dehealthy, NULL);
    pthread_detach(Thread_DeHealthy);
    
    // 初始化連線結構並寫入啟動日誌
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
        // 網路未就緒，記錄狀態並將標籤設為 offline
        printf("iot. SetReconnectState fail \n");
        internet_healthy = offline;
        FILE *fp_SHFInternet;
#ifdef VM
        fp_SHFInternet = fopen(Loacl_name_internet, "w");
#else
        fp_SHFInternet = fopen(UIS_name_internet, "w");
#endif
        fprintf(fp_SHFInternet, "off");
        if (fp_SHFInternet != NULL)
            fclose(fp_SHFInternet);
        first_run = flag_no;
    }
    
    // 若初始狀態尚未建立連線，則在此阻塞等待網路恢復
    while (rec == false) 
    {
#ifdef DEBUGLPRINT
        printf("iot. 等待網路恢復...\n");
#endif
        sleep(15); // 每 15 秒檢查一次
        if (internet_healthy == online)
        {
            rec = SetReconnectState(&reconnect_state);
            if (rec == true)
            {
                printf("iot.  2. SetReconnectState success \n");
                mqttconnectFlag = mqttreconnect(&reconnect_state);
                mqttconnectFlag = 1 ;
                first_run = flag_no;
                break; // 成功連線，跳出等待迴圈
            }
            else
            {
                // 仍無法設定重連狀態，記錄錯誤日誌
                printf("iot.  2. SetReconnectState fail \n");
                internet_healthy = offline;
                memset(logbuf, 0, lofbufsize);
                sprintf(logbuf, "%s: %s/%s/%d, iot.  2. SetReconnectState fail  \n", timestring_TimeFormat, __FILE__, __func__, __LINE__);
                WriteToLogFile(logbuf);
            }
        }
    }
    printf("iot. =========MqttExecute END=============== \n");
    sleep(5); // 緩衝時間，確保環境穩定
    
    // 啟動 MQTT 監聽與定期發送資料的主執行緒
    pthread_t Thread_MQTTListener;
    pthread_create(&Thread_MQTTListener, NULL, &mqtt_listener, NULL);
    pthread_detach(Thread_MQTTListener);
}

/**
 * @brief MQTT 訊息接收回呼函數：解析 Topic 與 Payload(JSON)，並指派對應的控制動作。
 */
void publish_callback(void **unused, struct mqtt_response_publish *published)
{
    // 配置記憶體並複製 Topic 字串
    char *topic_name = (char *)malloc(published->topic_name_size + 1);
    if (!topic_name) 
    {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }
    memcpy(topic_name, published->topic_name, published->topic_name_size);
    topic_name[published->topic_name_size] = '\0';
    
    // 配置記憶體並複製 Payload (JSON) 字串
    char *json_str = (char *)malloc(published->application_message_size + 1);
    if (!json_str) {
        fprintf(stderr, "Memory allocation failed\n");
        free(topic_name);
        return;
    }
    memcpy(json_str, published->application_message, published->application_message_size);
    json_str[published->application_message_size] = '\0';

    size_t i;
    uint8_t *app_msg = (uint8_t *)published->application_message;

    printf("\n\n\niot. ~~~~~~!Received publish('%s'): \n", topic_name);
    printf("iot. %s\n",json_str);
    
    // 分割 Topic，提取控制指令目標 (例如 action)
    char *psave = NULL;
    char *p = strtok_r(topic_name, "/", &psave);
    char getCmd[20] = {0};
    int countAB = 0;
    do
    {
        countAB++;
        p = strtok_r(NULL, "/", &psave);
#ifdef DEBUGLPRINT
        printf("iot. countAB = %d, segment = %s\n", countAB, p);
#endif
        if (countAB == 3) // 取得 Topic 第四段 (index 3) 作為指令識別
            strncpy(getCmd, p, 20);
    } while (p);
    
    printf("iot. TOPIC getCmd = %s\n", getCmd);
    
    // 若為動作指令 (action)，則解析 JSON 內容
    if (strcmp(getCmd, "action") == 0)
    {
        cJSON *json = cJSON_Parse(json_str);
        if (json != NULL)
        {
            // 檢查 MNO (設備編號) 是否吻合
            cJSON *pJSONmno = cJSON_GetObjectItemCaseSensitive(json, "MNO");
            if (cJSON_IsString(pJSONmno) && (pJSONmno->valuestring != NULL))
            {
                printf("iot. MNO: %s\n", pJSONmno->valuestring);
            }
            else
            {
                // MNO 不存在則不處理，寫入日誌
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
            if (1) // 虛擬機環境下略過設備編號檢查
#else
            if (strcmp(pJSONmno->valuestring, g_mno) == 0) // 確認是發給本機的指令
#endif
            {   
                // 處理不同種類的命令
                pJSONcommand = cJSON_GetObjectItemCaseSensitive(json, "command");
                if (pJSONcommand == NULL) return;
                
                if (cJSON_IsString(pJSONcommand) && (pJSONcommand->valuestring != NULL))
                {
                    printf("iot. command: %s\n\n\n", pJSONcommand->valuestring);
                    
                    // 指令：重啟系統
                    if (strcmp(pJSONcommand->valuestring, "SKS_SYSTEM_REBOOT") == 0)
                    {
                        printf("iot. Command SKS_SYSTEM_REBOOT received,  will Reboot system soon.\n\n\n");
                        send_buf_flag = false;     // 關閉資料傳送機制
                        
                        // 發送回饋後等待 5 秒並執行硬體重啟
                        pthread_t send_buf_thread;  
                        pthread_create(&send_buf_thread, NULL, send_buf, NULL);
                        pthread_detach(send_buf_thread);
                        sleep(5); 
                        system("reboot");
                    }
                    // 指令：啟動資料密集傳輸模式 (60秒)
                    else if (strcmp(pJSONcommand->valuestring, "SEND_DATA") == 0)
                    {
                        printf("iot. Command SKS_SAND_DATA received, starting to send buf every second for ~~~60~~~ seconds.\n\n\n");
                        send_buf_flag = true;  // 設置傳送旗標
                        
                        pthread_t send_buf_thread;  
                        pthread_create(&send_buf_thread, NULL, send_buf, NULL);
                        pthread_detach(send_buf_thread); 
                    }
                    // 指令：停止資料密集傳輸
                    else if (strcmp(pJSONcommand->valuestring, "SEND_DATA_STOP") == 0)
                    {
                        send_buf_flag = false;     // 取消傳送旗標
                        printf("iot. Command SEND_DATA_STOP received, stopping buf transmission.\n\n\n");
                        
                        pthread_t send_buf_thread;  
                        pthread_create(&send_buf_thread, NULL, send_buf, NULL);
                        pthread_detach(send_buf_thread);
                    }
                    // 指令：系統測試連線
                    else if (strcmp(pJSONcommand->valuestring, "TRY_RUN") == 0)
                    {
                        printf("iot. Command TRY_RUN received, try to pass the message.\n");
                        pthread_t send_feedback_thread;  
                        pthread_create(&send_feedback_thread, NULL, send_feedback, NULL);
                        pthread_detach(send_feedback_thread);
                    }
                    // 指令：讀取並上傳系統日誌
                    else if (strcmp(pJSONcommand->valuestring, "SYSTEM_READ") == 0)
                    {
                        printf("iot. Command SYSTEM_READ received, try to pass the message.\n");
                        cJSON *pJSONpath = cJSON_GetObjectItemCaseSensitive(json, "path");
                        send_logger_flag = true;
                        
                        if (!cJSON_IsString(pJSONpath) || pJSONpath->valuestring == NULL) 
                        {
                            printf("iot. SYSTEM_READ without path\n");
                            cJSON_Delete(json);
                        }
                        
                        char *log_file_path = strdup(pJSONpath->valuestring);
                        pthread_t send_feedback_thread;  
                        pthread_create(&send_feedback_thread, NULL, send_feedback, log_file_path);
                        pthread_detach(send_feedback_thread);
                    }
                }

                // 處理 Firmware URL 下載請求
                pJSONurl = cJSON_GetObjectItemCaseSensitive(json, "frm_url");
                if (pJSONurl == NULL) return;
                
                if (cJSON_IsString(pJSONurl) && (pJSONurl->valuestring != NULL))
                {
                    printf("iot. frm_url: %s\n", pJSONurl->valuestring);

#ifdef VM
                    fp_WriteToSHMMQTT = fopen(Loacl_name, "w");
#else
                    fp_WriteToSHMMQTT = fopen(UIS_name_Upgrade, "w+");
#endif
                    if (fp_WriteToSHMMQTT == NULL)
                    {
                        // 無法開啟升級設定檔
                        printf("iot. open fail errno = %d reason = %s ---%s\n", errno, strerror(errno), UIS_name_Upgrade);
                        RealTimeFormat();
                        memset(logbuf, 0, lofbufsize);
                        sprintf(logbuf, "%s: %s/%s/%d, open fail errno = %d reason = %s \n", timestring_TimeFormat, __FILE__, __func__, __LINE__, errno, strerror(errno));
                        WriteToLogFile(logbuf);
                        return;
                    }
                    else
                    {
                        // 寫入下載 URL 並同步至磁碟
                        fwrite(pJSONurl->valuestring, strlen(pJSONurl->valuestring), 1, fp_WriteToSHMMQTT);
                        sync();
                        fclose(fp_WriteToSHMMQTT);
                    }
                }
            }
            else
            {
                printf("iot. pJSONmno->valuestring = %s, g_mno = %s \n", pJSONmno->valuestring, g_mno);
                return;
            }

            cJSON_Delete(json);
        }
        else
        {
            // 解析 JSON 失敗處理
            const char *error_ptr = cJSON_GetErrorPtr();
            if (error_ptr != NULL)
            {
                printf("iot. Error: %s\n", error_ptr);
            }
            cJSON_Delete(json);
            perror("wrong command \n");
        }
        
        // 特殊的 debug message 印出 (依據 Payload 前幾個 Byte)
        if ((app_msg[0] == 5) & (app_msg[3] == 6))
        {
            printf("iot. \nReceived publish('%s'): ", topic_name); 
            for (i = 0; i < published->application_message_size; i++)
            {
                printf("iot. %02x ", ((uint8_t *)published->application_message)[i]);
            }
            printf("iot. \n ");
        }

        publish_ack = true; // 註記已收到且解析完成，準備於主迴圈清理或回應
    }
    
    // 釋放記憶體
    free(json_str);
    free(topic_name);
}

/**
 * @brief MQTT 背景心跳執行緒：不斷呼叫 mqtt_sync 以接收資料並維持連線活躍。
 */
void *client_refresher(void *client)
{
    client_daemon_flag = 1;
    while (1)
    {
        mqtt_sync((struct mqtt_client *)client);
        usleep(100000);  // 休息 100ms，以防佔用過多 CPU
    }
    return NULL;
}

/**
 * @brief 負責打包並定期發布感測器資料至 MQTT Broker，同時處理傳輸異常。
 */
void *mqtt_listener(void *arg)
{
    int presecond_count = 0;
    char pub_topic[100]; 
    int forloop = 0;
    int conuntA = 0;
    int haveError = 0;
    int retry_count = 0;

    while (1)
    {
        if (publish_ack)
        {
            printf("iot. \n\n Feedback............  OK!\n ");
            publish_ack = false; // 清除 ACK 標籤
        }
        
#ifdef DEBUGLPRINT
        printf("iot. while Send clock   = %d\n", presecond_count);
#endif

        // 每 900 次迴圈觸發一次資料打包上傳 (若 sleep(1) 約為 15 分鐘一次，視外部如何調用)
        if ((presecond_count++ % 900) == 0) 
        {
            RealTimeFormat();
            if (internet_healthy != online) 
            {
                // 網路斷線，記錄日誌但不發送
                RealTimeFormat();
                memset(logbuf, 0, lofbufsize);
                sprintf(logbuf, "%s: %s/%s, Network disconnection ", timestring_TimeFormat, __FILE__, __func__);
                WriteToLogFile(logbuf); 
            }
            else
            {
                // 讀取所有設備資料並準備打包
                deviceNum = ReadAllData();
                if (deviceNum > 0)
                {
                    DeviceInfo = InitialDeviceInfoValue(DeviceInfo);
                    forloop = (deviceNum + 9) / 10; // 每 10 個設備資料打包成一包 JSON
                    int b;
                    for (b = 0; b < forloop; b++) 
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

                            int retry_local = 0;
                            // 嘗試發送，失敗則重試
                            while (mqtt_publish(&client, pub_topic, buf, strlen(buf), MQTT_PUBLISH_QOS_0) != MQTT_OK)
                            {
                                printf("iot. 884 client.error = %d \n", client.error);
                                fprintf(stderr, "publish error: %s\n", mqtt_error_str(client.error));
                                
                                RealTimeFormat();
                                memset(logbuf, 0, lofbufsize);
                                sprintf(logbuf, "%s: %s/%s, %s \n", timestring_TimeFormat, __FILE__, __func__, mqtt_error_str(client.error));
                                WriteToLogFile(logbuf);

                                // 若發送緩衝區滿了，同步並稍候重試
                                if (client.error == MQTT_ERROR_SEND_BUFFER_IS_FULL) {
                                    printf("iot. Buffer full, waiting...\n");
                                    mqtt_sync(&client); 
                                    usleep(100 * 1000); // 延遲 100ms
                                    if (++retry_local > 5) {
                                        printf("iot. Publish retry exceeded limit.\n");
                                        break;
                                    }
                                } else {
                                    break; // 其他錯誤直接跳出
                                }
                                
                                retry_count++;
                                if (retry_count > 3) 
                                {
                                    // 重試超過三次，視為嚴重的連線異常，中斷並觸發重連
                                    printf("iot. 567 retry count");
                                    exit_reenter(EXIT_FAILURE_mqtt_publish, sockfd, &client_daemon);
                                    haveError = 1;
                                    errorCount++; 
                                    mqttconnectFlag = connectFail;
                                    sleep(60);
                                    break;
                                }
                                sleep(5); // 休息 5 秒後重試
                            }
                            
                            retry_count = 0;
                            if (client.error == MQTT_OK)
                            {
                                errorCount2 = 0; // 發送成功，清除連續錯誤計數
                            }
                            
                            // 發送完畢釋放 JSON 緩衝區
                            if (buf != NULL && bufFlag) {
                                free(buf);
                                buf = NULL;
                                bufFlag = 0;
                            }
                            usleep(200000); 
                        }
                        else
                        {
                            // 打包 JSON 失敗的錯誤處理
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
                        // 每次發送間隙做一次同步操作
                        mqtt_sync(&client);    
                        usleep(500000); 
                    }
                    
                    // 若有網卻顯示連接失敗，觸發重連
                    if ((internet_healthy == online) && (mqttconnectFlag == connectFail))
                    {
                        printf("iot. 597Attempting to reconnect MQTT...Flag = %d (0:connectFail,1:connectOK)\n", mqttconnectFlag);
                        mqttconnectFlag = mqttreconnect(&reconnect_state);
                        printf("iot. 599mqttconnectFlag = %d (0:connectFail,1:connectOK)\n", mqttconnectFlag);
                    }

                    // 釋放原始資料緩衝
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

/**
 * @brief 密集上傳資料任務。當收到 SEND_DATA 時觸發，會在 60 秒內持續打包上傳。
 */
void* send_buf(void* arg) {
    printf("iot. publish funtion on\n");
    int count = 0;
    char feedback_topic[50];
    snprintf(feedback_topic, sizeof(feedback_topic), "/%s/%s/%s/%s","iot","sks","feedback",g_mno);
    char send_topic[50];
    sprintf(send_topic, "/%s/%s/%s/%s","iot","sks","result",g_mno);
    
    // 若為停止指令 (SEND_DATA_STOP)，則只發送 200 回應
    if (!send_buf_flag) {
        const char* response = "200";
        printf("iot. Sending feedback to topic: %s\n", response);
        if (mqtt_publish(&client, feedback_topic, response, strlen(response), MQTT_PUBLISH_QOS_0) != MQTT_OK) {
            printf("iot. Failed to publish feedback to feedback topic\n");
            exit_reenter(EXIT_FAILURE_mqtt_publish, sockfd, &client_daemon);
        } else {
            printf("iot. Feedback sent successfully to feedback topic\n");
        }
        return NULL; 
    }
    
    // 執行 60 次 (預期約 60 秒) 的密集上傳
    while (count < 60 && send_buf_flag) {
        deviceNum = ReadAllData();
        RealTimeFormat();
        int forloop = (deviceNum + 9) / 10;  // 每 10 個設備包一組
        int b;

        usleep(200000); 
        for (b = 0; b < forloop; b++) {
            int ok = create_JSONobjects(b); 
            if ((buf != NULL) && (ok == 0)) {
                #ifdef DEBUGLPRINT
                printf("iot. Send string length = %u\n", (unsigned)strlen(buf));
                printf("iot. pub_topic = '%s' \n", send_topic);
                #endif
                usleep(100000); 
                
                if (mqtt_publish(&client, send_topic, buf, strlen(buf), MQTT_PUBLISH_QOS_0) != MQTT_OK) {
                    #ifdef DEBUGLPRINT
                    printf("iot. Failed to publish, retrying...\n");
                    #endif
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
        sleep(2);  // 每次間隔約 2 秒
        count++;
    }
    
    send_buf_flag = false;  // 時間到自動停止
    printf("iot. publish funtion off\n\n");
    return NULL;
}

/**
 * @brief 回報執行緒：用於回應遠端指令，或將指定的檔案(日誌檔)分段發送到 MQTT 主題。
 */
void* send_feedback(void* arg) {
    char feedback_topic[50];
    snprintf(feedback_topic, sizeof(feedback_topic), "/%s/%s/%s/%s","iot","sks","feedback",g_mno);

    if (send_logger_flag) {
        // 模式：發送本機日誌檔
        char *log_file_path = (char *)arg;
        printf("iot. 正在讀取並回傳日誌檔案: %s\n", log_file_path);

        FILE *fp = fopen(log_file_path, "r");
        if (fp == NULL) 
        {
            printf("iot. 無法開啟檔案: %s\n", log_file_path);
            free(log_file_path);
            return NULL;
        }
        
        // 批次讀取檔案並發送
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
            sleep(1);  // 休息避免擠爆 MQTT 緩衝
        }           
        free(log_file_path);
        send_logger_flag = false;  
    }
    else {
        // 模式：僅發送 ACK 確認訊號 (222)
        const char* response = "222";
        printf("iot. Sending feedback to topic: %s\n", response);
        if (mqtt_publish(&client, feedback_topic, response, strlen(response), MQTT_PUBLISH_QOS_0) != MQTT_OK) {
            printf("iot. Failed to publish feedback to feedback topic\n");
        } else {
            // 發送成功
        }
    }
    return NULL;
}

/**
 * @brief 除錯工具：以 Hex (16進位) 格式印出位元組陣列
 */
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

/**
 * @brief DNS 轉換：將 Hostname (域名) 轉成 IPv4 字串
 */
int hostname_to_ip(char *hostname, char *ip)
{
    struct hostent *he;
    struct in_addr **addr_list;
    int i;
    #ifdef DEBUGLPRINT 
    printf("iot. hostname = %s\n", hostname);
    #endif

    if ((he = gethostbyname(hostname)) == NULL) 
    {
        herror("gethostbyname");
        return 0;
    }

    addr_list = (struct in_addr **)he->h_addr_list;

    for (i = 0; addr_list[i] != NULL; i++)
    {
        strcpy(ip, inet_ntoa(*addr_list[i]));
    }
    return 1;
}

/**
 * @brief MQTT 重連程序。涵蓋憑證檢查、TLS/SSL Socket 建立、MQTT 登入及訂閱設定。
 */
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

    // 清理舊有的 socket / SSL 結構
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

    // 中斷並重置 MQTT 客戶端狀態
    if (client.error != MQTT_OK) {
        mqtt_disconnect(&client);
        memset(&client, 0, sizeof(client));
    }

    // 初始化隨機數生成器 (SSL 需要)
    if (RAND_poll() == 0) {
        fprintf(stderr, "iot. Failed to initialize OpenSSL RNG!\n");
        return connectFail;
    }
    unsigned char seed[32];
    if (!RAND_bytes(seed, sizeof(seed))) {
        fprintf(stderr, "iot. OpenSSL RNG failed, adding entropy manually...\n");
        RAND_add(seed, sizeof(seed), sizeof(seed) / 2.0);
    }

    // 檢查憑證是否存在 (CA, client, clientkey)
    FILE *fp_chkFile;    
#ifdef VM
    fp_chkFile = fopen("./cert/CA_list.pem", "r");
#else
    fp_chkFile = fopen("/var/www/data/CA_list.pem", "r");
#endif
    if (fp_chkFile == NULL)
    {
        printf("iot. Load CA_list fail ,sleep 60 sec\n");
        RealTimeFormat();
        memset(logbuf, 0, lofbufsize);
        CA_ERROR_flag++;
        if(CA_ERROR_flag<3)
        {
            sprintf(logbuf, "%s: %s/%s/%d, load ca_certificate Failed sleep 60 sec\n", timestring_TimeFormat, __FILE__, __func__, __LINE__);
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

    // 建立 Socket 連線
#ifdef TLSON
#ifndef VM
    open_nb_socket(&sockfd, &ssl_ctx, reconnect_state->hostname, reconnect_state->port, "/var/www/data/CA_list.pem", NULL, "/var/www/data/client.pem", "/var/www/data/clientkey.pem");
#else
    open_nb_socket(&sockfd, &ssl_ctx, reconnect_state->hostname, reconnect_state->port, "./cert/CA_list.pem", NULL, "./cert/client.pem", "./cert/clientkey.pem");
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
    // 初始化 MQTT client 結構
    int ack = mqtt_init(&client, sockfd, reconnect_state->sendbuf, reconnect_state->sendbufsz, reconnect_state->recvbuf, reconnect_state->recvbufsz, publish_callback);
    if (ack != 1)
    {
        printf("iot. mqtt_init: %s\n", mqtt_error_str(ack));
        memset(logbuf, 0, lofbufsize);
        RealTimeFormat();
        sprintf(logbuf, "%s: %s/%s/%d, mqtt_init:%s \n", timestring_TimeFormat, __FILE__, __func__, __LINE__, mqtt_error_str(ack));
        WriteToLogFile(logbuf);
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

    // 發送 CONNECT 封包
    uint8_t connect_flags = MQTT_CONNECT_CLEAN_SESSION;
    mqtt_connect(&client, reconnect_state->client_id, NULL, NULL, 0, "admin", "admin", connect_flags, 0);   
    
    if (client.error != MQTT_OK)
    {
        printf("iot. mqtt_connect: %s\n", mqtt_error_str(client.error));
        RealTimeFormat();
        memset(logbuf, 0, lofbufsize);
        sprintf(logbuf, "%s: %s/%s/%d, mqtt_connect:%s \n", timestring_TimeFormat, __FILE__, __func__, __LINE__, mqtt_error_str(client.error));
        WriteToLogFile(logbuf);
        return connectFail;
    }
    printf("iot. mqtt_connect ack = %d \n", client.error);

    client.publish_response_callback_state = &client;
    printf("iot. Connected to MQTT server\n");

    // 重啟心跳守護執行緒
    if (pthread_create(&client_daemon, NULL, client_refresher, &client))
    {
        fprintf(stderr, "Failed to start client daemon.\n");
        memset(logbuf, 0, lofbufsize);
        sprintf(logbuf, "%s: %s/%s/%d, pthread_create_daemon: Failed to start client daemon. \n", timestring_TimeFormat, __FILE__, __func__, __LINE__);
        WriteToLogFile(logbuf);
        exit_reenter(EXIT_FAILURE_pthread_create_daemon, sockfd, &client_daemon);
    }
    pthread_detach(client_daemon);

    // 訂閱主控制 Topic
    mqtt_subscribe(&client, reconnect_state->recv_topic, 0);
    if (client.error != MQTT_OK)
    {
        fprintf(stderr, "Subscription error: %s\n", mqtt_error_str(client.error));
        memset(logbuf, 0, lofbufsize);
        sprintf(logbuf, "%s: %s/%s/%d,mqtt_subscribe: %s \n", timestring_TimeFormat, __FILE__, __func__, __LINE__, mqtt_error_str(client.error));
        WriteToLogFile(logbuf);
        exit_reenter(EXIT_FAILURE_mqtt_subscribe, sockfd, &client_daemon);
        
#ifdef TLSON
        BIO_free_all(sockfd);
        sockfd = NULL;
#endif
        return connectFail;
    }

    // 訂閱系統專屬指令 Topic
    char full_topic[100];
    memset(full_topic, 0, sizeof(full_topic));
    sprintf(full_topic, "/%s/%s/%s/%s/%s", "iot", "sks", "syscmd", "action", g_mno);
    mqtt_subscribe(&client, full_topic, 0);

    if (client.error != MQTT_OK)
    {
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

/**
 * @brief 錯誤與異常退出處理，根據狀態代碼決定是否休息及重連。
 */
#ifdef TLSON
void exit_reenter(int status, BIO *sockfd, pthread_t *client_daemon)
{
    printf("iot. EXIT_FAILURE = %d \n", status);
    
    // 若為傳送失敗或憑證讀取錯誤，延遲避免 CPU 空轉
    if ((status == EXIT_FAILURE_mqtt_publish) || (status == EXIT_FAILURE_load_certificate)) 
    {
        sleep(5);
    }

    if (status > EXIT_FAILURE_NULL)
        internet_healthy = offline;
        
    printf("iot. 987 internet_healthy = %d (0:offline,1:online)\n", internet_healthy);
    
    if (internet_healthy == offline) 
    {
        sleep(60); // 斷線後休息 60 秒再試
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

/**
 * @brief 網路健康度監測執行緒：使用 ping 與 curl 來判定外網連接，以修改 internet_healthy 狀態。
 */
void *dehealthy(void *arg)
{
    FILE *fp_SHFInternet;
    int iSize;
    char *buffer;
    int result;
    
    while (1)
    { 
        if (first_run == flag_no) // 確保初始化完成才開始監測
        {
            printf("iot. ===============7 minutes health detection=================\n");
            // 第一步：Ping Google DNS
            result = system("ping -c 1 -W 2 8.8.8.8 > /dev/null 2>&1");

            if (result != 0) 
            {
                sleep(3);
                #ifdef DEBUGLPRINT 
                printf("iot. ping 8.8.8.8 failed, trying 168.95.1.1...\n");
                #endif 
                // 第二步：Ping 中華電信 DNS
                result = system("ping -c 1 -W 2 168.95.1.1 > /dev/null 2>&1");

                if (result != 0) 
                {
                    sleep(3);
                    #ifdef DEBUGLPRINT 
                    printf("iot. ping 168.95.1.1 failed, trying curl...\n");
                    #endif 
                    // 第三步：直接用 Curl 測試 Google 首頁
                    result = system("curl -I --connect-timeout 5 https://www.google.com > /dev/null 2>&1");

                    if (result != 0) 
                    {
                        sleep(3); 
                        #ifdef DEBUGLPRINT 
                        printf("iot. offline (ping 8.8.8.8, ping 168.95.1.1, and curl all failed)\n");
                        #endif
                        
                        // 三種測試皆失敗，寫入實體檔案記錄並設為斷線
                        fp_SHFInternet = fopen(UIS_name_internet, "r+");
                        if (fp_SHFInternet == NULL) {
                            fp_SHFInternet = fopen(UIS_name_internet, "w+"); // 不存在則創建
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
                            printf("iot. 網路狀態紀錄檔為空，設定為 offline\n");
                            #endif
                            fclose(fp_SHFInternet);
                            internet_healthy = offline;
                            sleep(120); 
                            continue;
                        }

                        buffer = (char *)malloc(iSize + 1);
                        if (buffer == NULL) {
                            #ifdef DEBUGLPRINT
                            printf("iot. 記憶體分配失敗，預設為 offline\n");
                            #endif
                            fclose(fp_SHFInternet);
                            internet_healthy = offline;
                            sleep(120); 
                            continue;
                        }

                        result = fread(buffer, 1, iSize, fp_SHFInternet);
                        buffer[iSize] = '\0'; // 字串結尾

                        if (result == 0) {
                            #ifdef DEBUGLPRINT
                            printf("iot. 紀錄檔讀取失敗，buffer 內容為空\n");
                            #endif
                            fclose(fp_SHFInternet);
                            free(buffer);
                            internet_healthy = offline;
                            continue;
                        }

                        if (strncmp(buffer, "off", 3) == 0) {
                            printf("iot. offline \n");
                            internet_healthy = offline;
                            mqttconnectFlag = connectFail; // 因網路不通判定重連失敗
                        } else {
                            printf("iot. online \n");
                            internet_healthy = online;
                        }

                        free(buffer);
                        fclose(fp_SHFInternet);
                        
                        // 清空檔案內容
                        fp_SHFInternet = fopen(UIS_name_internet, "w");
                        if (fp_SHFInternet != NULL) {
                            fclose(fp_SHFInternet);
                        }
                    }
                    else
                    {
                        #ifdef DEBUGLPRINT
                        printf("iot. online (curl success)\n");
                        #endif
                        internet_healthy = online;
                    }
                }
                else
                {
                    #ifdef DEBUGLPRINT
                    printf("iot. online (ping 168.95.1.1 success)\n");
                    #endif
                    internet_healthy = online;
                }
            }
            else
            {
                #ifdef DEBUGLPRINT
                printf("iot. online (ping 8.8.8.8 success)\n");
                #endif
                internet_healthy = online;
            }
            
            // 對 MQTT Server 發出 Ping 確保通道存活
            mqtt_ping(&client);
            sleep(3);
            if (internet_healthy == online && client.error != MQTT_OK)
            {
                printf("iot. MQTT ping failed: %s\n", mqtt_error_str(client.error));
                mqttconnectFlag = connectFail;  // 重新觸發 MQTT 握手
            }

            printf("iot. mqttconnectFlag = %d  ,internet_healthyFlag = %d\n",mqttconnectFlag,internet_healthy);
            
            // 網路正常但 MQTT 斷線時觸發重連
            if ((internet_healthy == online) && (mqttconnectFlag == connectFail))
            {
                printf("iot. Attempting to reconnect MQTT...\n");
                mqttconnectFlag = mqttreconnect(&reconnect_state);
                printf("iot. mqttconnectFlag = %d  ,internet_healthyFlag = %d\n",mqttconnectFlag,internet_healthy);
                sleep(60);
                
                errorCount3++;
                // 重連連續失敗 3 次則自動重啟進程
                if (errorCount3>=3)
                {
                    printf("iot.1185 MQTT ping errorCount3: %d\n",errorCount3);
                    RealTimeFormat();
                    memset(logbuf, 0, lofbufsize);
                    sprintf(logbuf, "%s: %s/%s/%d, 1189 exit_reenter \n", timestring_TimeFormat, __FILE__, __func__, __LINE__);
                    WriteToLogFile(logbuf);
                    exit(0); // 退出讓外部 Watchdog 重啟程式
                }
            }

            // 完全斷線狀態時的休息策略
            if (internet_healthy == offline)
            {
                printf("iot.Connect Fail, will retry after 8 min...\n");
                sleep(420);
                mqttconnectFlag = mqttreconnect(&reconnect_state);
                printf("iot. mqttconnectFlag = %d (0:connectFail,1:connectOK)\n", mqttconnectFlag);
            }
        }
        sleep(420); // 預設每 7 分鐘執行一次完整監測
    }
}

/**
 * @brief 設定重連狀態參數。準備好 Broker 地址、各種 Topic 以及記憶體緩衝區配置。
 */
int SetReconnectState(reconnect_state_t *reconnect_state)
{
    printf("iot. =========SetReconnectState=========== \n");
    char flag_return = 0;
    char client_id[40];
    
#ifndef IOTMQTT
    char *hostname = "rabbitmq.sksiot.com"; // 測試機
    printf("iot. ~測試機 rabbitmq~   ~測試機 rabbitmq~    ~測試機 rabbitmq~\n");
#else
    char *hostname = "rabbitmq-prod.sksiot.com"; // 正式機
#endif

    char addr[100];
    memset(addr, 0, strlen(addr));
    
    if (hostname_to_ip(hostname, addr) == 0)
    {
        printf("iot. hostname_to_ip fail\n");
        RealTimeFormat();
        memset(logbuf, 0, lofbufsize);
        sprintf(logbuf, "%s: %s/%s/%d, hostname_to_ip \n", timestring_TimeFormat, __FILE__, __func__, __LINE__);
        WriteToLogFile(logbuf);
        flag_return = 1;
    }
    
    // 使用設備 MAC 地址做為 Client ID
    strcpy(client_id, DeviceInfo.macAddress);
    char topic[100];        
    uint8_t sendbuf[30000]; // 發送緩衝區
    uint8_t recvbuf[30000]; // 接收緩衝區
    
    bzero(topic, strlen((const char *)topic));
    sprintf(topic, "/%s/%s/%s/%s/%.3s", "iot", "sks", "syscmd", "action",g_mno);
    
    #ifdef DEBUGLPRINT 
    printf("iot. ip address = %s \n", addr);
    #endif
    printf("iot. topic = '%s' \n", topic);

#ifdef TLSON
    const char *port = "8883"; // SSL/TLS 預設通訊埠
#else
    const char *port = "1883"; // 一般通訊埠
#endif

    // 指派指標到結構中
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

/**
 * @brief 將指定的日誌檔內容，自動發布到 MQTT Broker (需在編譯時開啟 SEND_LOG_TO_MQTT)。
 */
void *SendLogForMQTT(void *arg)
{
    char pub_topic_log[100];
    FILE *fp_RWLogFile;
    char chunk[256];

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
                    // 逐行讀取檔案並上傳
                    while (fgets(chunk, sizeof(chunk), fp_RWLogFile) != NULL)
                    {
                        bzero(pub_topic_log, strlen((const char *)pub_topic_log));
                        sprintf(pub_topic_log, "/%s/%s/%s/%s", "iot", "sks", "log", g_mno);
                        mqtt_publish(&client, pub_topic_log, chunk, strlen(chunk), MQTT_PUBLISH_QOS_0);
                        
                        mqttconnectFlag = connectFail; // 註解：此處將連線狀態重設的邏輯需注意是否為刻意為之
                        printf("iot. \n\n");
                    }
                    fclose(fp_RWLogFile);
                    
                    // 發送完畢後清空檔案
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