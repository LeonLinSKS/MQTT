#pragma once
#ifndef READFLASH_H
#define READFLASH_H

#include "apmib.h"

typedef struct FlashData
{
    char apmib_mno[30];
    char apmib_UID[30];
    char apmib_gw[30];
    char UCN_TOTAL[128];
    char UCR_ALL[64];
    char UCR4[64];
    char UCR[64];
    char UCR2[64];
    char UCR3[64];
} Flash_Data;

void ReadFlash(Flash_Data *p_flash_data);
void DefaultData(Flash_Data *p_flash_data);
void PrintFlashDataMessage(Flash_Data *p_flash_data);


#endif
