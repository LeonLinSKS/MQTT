
// todo 需要梳理避免重複include
#pragma once
#ifndef READZONESTATUS_H
#define READZONESTATUS_H

typedef struct
{
    unsigned char DateCode[17];
    unsigned char pad[3];
    unsigned char value[14];
    unsigned char pad2[2];
} Zone_Device_Status_RD;

int ReadZoneStatusSize(void);
int ReadZoneStatus(Zone_Device_Status_RD *zone_status);
void PrintZoneStatus(Zone_Device_Status_RD *p_zone_device_status);

#endif
