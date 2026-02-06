// gcc RealTime.c -g -I ./include -o RealTime
// valgrind --leak-check=full ./RealTime

#include "common.h"
#include "RealTime.h"

#ifdef DEBUG
int main()
{
    RealTimeFormat();
    printf("iot. %s \n", timestring_TimeFormat);
    return 0;
}
#endif

char *RealTimeFormat()
{ // 時間處理程式
    time_t timep_TimeFormat;
    struct tm *p_TimeFormat;

    time(&timep_TimeFormat);
    p_TimeFormat = localtime(&timep_TimeFormat); /*取得當地時間*/
    sprintf(timestring_TimeFormat, "%d-%02d-%02d %02d:%02d:%02d", (1900 + p_TimeFormat->tm_year), (1 + p_TimeFormat->tm_mon), p_TimeFormat->tm_mday, p_TimeFormat->tm_hour, p_TimeFormat->tm_min, p_TimeFormat->tm_sec);
    // printf("iot. %s \n", timestring_TimeFormat);
    timestr_TimeFormat = timestring_TimeFormat;
    return timestr_TimeFormat;
}