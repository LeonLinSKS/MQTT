//gcc -I ./include ReadFlash.c -g -o ReadFlash

// msdk-linux-gcc -I ./include -g -Wall -O2 ReadFlash.c libapmib.so -lm -L . -o ReadFlash
#include "common.h"
#include "ReadFlash.h" // todo 這裡要注意重複引用問題

char g_mno[30] = "0780000";

#ifdef DEBUG
int main(void)
{
    Flash_Data *p_flash_data;
    p_flash_data = (Flash_Data *)malloc(sizeof(Flash_Data));
    memset(p_flash_data->apmib_mno, 0, sizeof(p_flash_data->apmib_mno));
    memset(p_flash_data->apmib_UID, 0, sizeof(p_flash_data->apmib_UID));
    memset(p_flash_data->apmib_gw, 0, sizeof(p_flash_data->apmib_gw));
    memset(p_flash_data->UCN_TOTAL, 0, sizeof(p_flash_data->UCN_TOTAL));
    memset(p_flash_data->UCR_ALL, 0, sizeof(p_flash_data->UCR_ALL));
    memset(p_flash_data->UCR4, 0, sizeof(p_flash_data->UCR4));
    memset(p_flash_data->UCR, 0, sizeof(p_flash_data->UCR));
    memset(p_flash_data->UCR2, 0, sizeof(p_flash_data->UCR2));
    memset(p_flash_data->UCR3, 0, sizeof(p_flash_data->UCR3));

    ReadFlash(p_flash_data);
    PrintFlashDataMessage(p_flash_data);
    strcpy(g_mno, p_flash_data->apmib_mno);
    free(p_flash_data);
    return 0;
}
#endif

void ReadFlash(Flash_Data *p_flash_data)
{
#ifndef VM
    apmib_init();
    apmib_get(MIB_USER_PASSWORD, (void *)p_flash_data->apmib_gw);
    apmib_get(MIB_SNMP_RW_COMMUNITY, (void *)p_flash_data->apmib_mno);
    apmib_get(MIB_PPP_PASSWORD4, (void *)p_flash_data->apmib_UID);
    apmib_get(MIB_PPP_USER_NAME2, (void *)p_flash_data->UCN_TOTAL);
    apmib_get(MIB_PPP_USER_NAME4, &p_flash_data->UCR4);
    apmib_get(MIB_DDNS_DOMAIN_NAME, &p_flash_data->UCR_ALL);
    apmib_get(MIB_DDNS_USER, &p_flash_data->UCR);
    apmib_get(MIB_DDNS_PASSWORD, &p_flash_data->UCR2);
    apmib_get(MIB_PPP_PASSWORD2, &p_flash_data->UCR3);
    strcpy(g_mno, p_flash_data->apmib_mno);
#else
    DefaultData(p_flash_data);
    strcpy(g_mno, p_flash_data->apmib_mno);
#endif
}

void DefaultData(Flash_Data *p_flash_data)
{
    // memset(p_flash_data->apmib_mno, 0x31, sizeof(p_flash_data->apmib_mno));
    sprintf(p_flash_data->apmib_mno, "06919999");
    // memset(p_flash_data->apmib_UID, 0x32, sizeof(p_flash_data->apmib_UID));
    sprintf(p_flash_data->apmib_UID, "12345678900987654321");
    // memset(p_flash_data->apmib_gw, 0x33, sizeof(p_flash_data->apmib_gw));
    sprintf(p_flash_data->apmib_gw, "1234567890");
    memset(p_flash_data->UCN_TOTAL, 0x34, sizeof(p_flash_data->UCN_TOTAL));
    memset(p_flash_data->UCR_ALL, 0x35, sizeof(p_flash_data->UCR_ALL));
    memset(p_flash_data->UCR4, 0x36, sizeof(p_flash_data->UCR4));
    memset(p_flash_data->UCR, 0x37, sizeof(p_flash_data->UCR));
    memset(p_flash_data->UCR2, 0x38, sizeof(p_flash_data->UCR2));
    memset(p_flash_data->UCR3, 0x39, sizeof(p_flash_data->UCR3));
    p_flash_data->apmib_mno[sizeof(p_flash_data->apmib_mno) - 1] = '\0'; // todo 加在最後,讀出真實長度可能會不對
    p_flash_data->apmib_UID[sizeof(p_flash_data->apmib_UID) - 1] = '\0';
    p_flash_data->apmib_gw[sizeof(p_flash_data->apmib_gw) - 1] = '\0';
    p_flash_data->UCN_TOTAL[sizeof(p_flash_data->UCN_TOTAL) - 1] = '\0';
    p_flash_data->UCR_ALL[sizeof(p_flash_data->UCR_ALL) - 1] = '\0';
    p_flash_data->UCR4[sizeof(p_flash_data->UCR4) - 1] = '\0';
    p_flash_data->UCR[sizeof(p_flash_data->UCR) - 1] = '\0';
    p_flash_data->UCR2[sizeof(p_flash_data->UCR2) - 1] = '\0';
    p_flash_data->UCR3[sizeof(p_flash_data->UCR3) - 1] = '\0';
}

void PrintFlashDataMessage(Flash_Data *p_flash_data)
{
    printf("iot. \n%s: %s (%d)pmib_mno = %s\n", __FILE__, __func__, __LINE__, p_flash_data->apmib_mno);
    printf("iot. \n%s: %s (%d)apmib_gw = %s\n", __FILE__, __func__, __LINE__, p_flash_data->apmib_gw);
    printf("iot. \n%s: %s (%d)apmib_UID = %s\n", __FILE__, __func__, __LINE__, p_flash_data->apmib_UID);
    printf("iot. \n%s: %s (%d)UCN_TOTAL = %s\n", __FILE__, __func__, __LINE__, p_flash_data->UCN_TOTAL);
    printf("iot. \n%s: %s (%d)UCR_ALL = %s\n", __FILE__, __func__, __LINE__, p_flash_data->UCR_ALL);
    printf("iot. \n%s: %s (%d)UCR4 = %s\n", __FILE__, __func__, __LINE__, p_flash_data->UCR4);
    printf("iot. \n%s: %s (%d)UCR = %s\n", __FILE__, __func__, __LINE__, p_flash_data->UCR);
    printf("iot. \n%s: %s (%d)UCR2 = %s\n", __FILE__, __func__, __LINE__, p_flash_data->UCR2);
    printf("iot. \n%s: %s (%d)UCR3 = %s\n", __FILE__, __func__, __LINE__, p_flash_data->UCR3);
}
