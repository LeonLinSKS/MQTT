# MQTT 代理程式說明（MqttComm.c）

本文件對應 `MqttComm.c`，說明程式流程、執行緒分工、MQTT 指令與重連機制。

## 1. 程式定位

`MqttComm` 是常駐的 MQTT Agent，主要功能：

- 讀取設備識別與 Flash 參數
- 連線到 MQTT Broker
- 訂閱控制指令 Topic
- 週期上傳資料
- 依指令執行回傳、送資料、重啟、讀檔回傳
- 網路/連線異常時自動重連

## 2. 啟動流程

入口是 `main()`：

1. 呼叫 `ReadFlash()` 讀取設備資訊與 `g_mno`
2. 呼叫 `MqttExecute()` 建立連線狀態並啟動背景執行緒
3. 主迴圈保持常駐，並在約 7 天後主動 `exit(0)` 交給 watchdog 重啟

## 3. 核心執行緒

### `client_refresher`

- 持續呼叫 `mqtt_sync()`
- 負責 MQTT 收送封包與 callback 驅動

### `mqtt_listener`

- 週期讀取資料、組 JSON、發布到資料 Topic
- 內含發送失敗重試與錯誤計數

### `dehealthy`

- 週期檢查網路（ping/curl）
- 更新 `internet_healthy`
- 檢查 `client.error`，需要時觸發 `mqttreconnect()`

### 其他條件執行緒

- `send_buf`：指令觸發後，短時間連續送資料
- `send_feedback`：回傳固定碼或檔案內容
- `SendLogForMQTT`：可選，將 log 轉發到 MQTT

## 4. MQTT 指令處理（`publish_callback`）

程式會解析 Topic 與 JSON payload，重點指令如下：

- `SKS_SYSTEM_REBOOT`：回傳後重開機
- `SEND_DATA`：啟動 burst data send
- `SEND_DATA_STOP`：停止 burst data send
- `TRY_RUN`：送回測試回覆
- `SYSTEM_READ`：讀指定檔案並分段回傳
- `frm_url`：寫入升級 URL 到指定檔案

## 5. 重連流程

### `SetReconnectState`

- 解析 broker domain -> IP
- 準備 `client_id`、topic、send/recv buffer
- 填入 `reconnect_state_t`

### `mqttreconnect`

- 清理舊 socket/SSL 狀態
- 驗證憑證檔
- 重新建立 MQTT 連線與訂閱
- 啟動 `client_refresher`

## 6. 主要全域狀態

- `client`：MQTT client 實體
- `internet_healthy`：網路狀態（online/offline）
- `mqttconnectFlag`：MQTT 連線狀態
- `send_buf_flag`：是否連續送資料
- `send_logger_flag`：是否回傳檔案內容
- `reconnect_state`：重連參數集合
- `buf` / `bufFlag`：由 `create_objects.c` 產生的 JSON payload 緩衝

## 7. 建議維護方向

目前可運作，但若要提升穩定性，建議：

1. 對 `client` 與 `buf` 增加 mutex 保護
2. 將 `publish_callback` 的指令分發拆成獨立函式
3. 將重試策略集中管理（retry/backoff）
4. 減少 shell command 依賴，改用程式內檢測機制

## 8. 編譯

使用專案根目錄 `Makefile`：

```bash
make clean
make
```

輸出檔案：

- `fw_data/MqttComm`
