
# include<stdio.h>
# include<stdlib.h>
# include<string.h>
# include<unistd.h>
# include<sys/socket.h>
# include<arpa/inet.h>
# include<netinet/in.h>
# include<signal.h>


# define MAX_BUF_LEN 512 
 

// gcc cJSON.c Client2.c -o Client.o  -lm
// 

/*处理系统调用中产生的错误*/
void error_print(char * ptr)
{
        perror(ptr);
        exit(EXIT_FAILURE);
}
/*处理通信结束时回调函数接收到的信号*/
void quit_tranmission(int sig)
{
    printf("recv a quit signal = %d\n",sig);
    exit(EXIT_SUCCESS);
}

int main(void)
{

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
        error_print("socket");
    struct sockaddr_in servaddr;
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = 1235;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int conn;
    if((conn = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) < 0)
        error_print("connect");

    pid_t pid;
    pid = fork();
    if(pid == -1){
        error_print("fork");
    }

    if(pid == 0){
        char recv_buf[MAX_BUF_LEN] = {0};
        while(1){
            bzero(recv_buf,sizeof(recv_buf));
            int ret = read(sockfd, recv_buf, sizeof(recv_buf)); 
            if(ret == -1)
                error_print("read");
            else if(ret == 0){
                printf("server is close!\n");
                break;//子进程收到服务器端退出的信息（服务器Ctrl+C结束通信进程，read函数返回值为0，退出循环）
            }
            printf("ret = %d \n", ret);
            fputs(recv_buf,stdout);/*将收到的信息输出到标准输出stdout上*/
            printf("\n");

        }
        close(sockfd);/*子进程退出，通信结束关闭套接字*/
        kill(getppid(),SIGUSR1);/*子进程结束，也要向父进程发出一个信号告诉父进程终止接收，否则父进程一直会等待输入*/
        exit(EXIT_SUCCESS);/*子进程正常退出结束，向父进程返回EXIT_SUCCESS*/
    }
    else{
        signal(SIGUSR1,quit_tranmission);/*回调函数处理通信中断*/
        char send_buf[MAX_BUF_LEN] = {0};
        /*如果服务器Ctrl+C结束通信进程，fgets获取的就是NULL，否则就进入循环正常发送数据*/
        while(fgets(send_buf,sizeof(send_buf), stdin) != NULL){
            int set = write(sockfd, send_buf, strlen(send_buf));/*将send_buf缓冲区的数据发送给对端服务器*/
            if(set < 0)
                error_print("write");
            bzero(send_buf,strlen(send_buf));
        }
        close(sockfd);/*通信结束，关闭套接字*/
    }
    return 0;
}

