#include "common.h"
#include "LogManage.h"

#ifdef DEBUG
int main(void)
{
    sprintf(logbuf, "123test\n");
    echo_FileToUSB(logbuf);
    return 0;
}
#endif

// agent
int echo_FileToUSB(char *data)
{
    int ret;
    char echo_buf[lofbufsize];

#ifdef VM
    ret = system("cp A/sks.log B/sks.log");
    sprintf(echo_buf, "echo \"%s\" >> B/sks.log &", data);
    system(echo_buf);
    system("cp B/sks.log A/sks.log");
    sync();
#else
    ret = system("cp /tmp/usb/sda1/log/sks.log /var/www/sks.log");
    sprintf(echo_buf, "echo \"%s\" >> /var/www/sks.log &", data);
    system(echo_buf);
    system("cp /var/www/sks.log /tmp/usb/sda1/log/sks.log");
    sync();
#endif

    if (ret != 0) // # 不等於零 表示有錯誤(判斷檔案是否存在? 目前給agent做)
    {
        printf("iot. error: cp /tmp/usb/sda1/log/sks.log /var/www/sks.log");
    }

    return 0;
}