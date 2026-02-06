# 新堡科 物聯網管理案

<img width="593" alt="image" src="https://github.com/kevins821026/etechpro2023/assets/124337402/6b89b297-fc7b-41ba-8311-74383e363c16">

2023.06.08 基本檔案放置

2023/07/18 add file "ReadZoneStatus, ReadZoneMerge, ReadFlash" (Eric)

# MQTT發送指令 範例如下:
重開機指令:

{"time":"2024-08-02 14:43:28.493","command":"SKS_SYSTEM_REBOOT","MNO":"0790003"}

版更指令:

{"time":"2024-08-02 14:43:28.493","command":"FRM_OTA","frm_url":"http://wlsecurity.sks.com.tw/Firmware/FDL.ashx?id=C","MNO":"0790003"}

送資料指令:
{"time":"2024-11-19 18:23:26.863","command":"SEND_DATA","MNO":"0790004"}

停送資料指令:
{"time":"2024-11-19 18:23:26.863","command":"SEND_DATA_STOP","MNO":"0790004"}
