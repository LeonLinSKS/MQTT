
// gcc -g -I ./include testPthread.c -L /usr/local/lib -lm -lpthread -lssl -lcrypto -o testPthread

#include <stdio.h>  // printf...
#include <stdlib.h> // for exit(0);
#include <string.h> // memset,strcpy...
#include <errno.h>  // errno
#include <pthread.h>
#include <signal.h>

pthread_t client_daemon;
int client_daemon_flag = 0; // # 1:表示創建過

int ret_temp = pthread_kill(client_daemon, 0);
if (ret_temp == 0)
{
    printf("run pthread_cancel\n");
    pthread_cancel(client_daemon);
}