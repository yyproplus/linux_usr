#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include "Common.h"
#include "Client.h"
#include "Log.h"
#define PORT 65432
#define BUFFER_SIZE 1024
// 信号处理函数
void handle_signal(int signal) {
    switch (signal) {
        case SIGINT:
            printf("收到 SIGINT (中断信号)\n");
            break;
        case SIGTERM:
            printf("收到 SIGTERM (终止信号)\n");
            break;
        case SIGPIPE:
        {
            printf("收到 SIGPIPE (终止信号)\n");
            break;
        }
        case SIGSEGV:
        {
            printf("收到 SIGSEGV (终止信号)\n");
            break;
        }
        default:
            printf("收到未知信号: %d\n", signal);
            break;
    }
    fflush(nullptr);
    // 执行清理工作
    // 关闭文件描述符、释放资源等
    exit(0); // 退出程序
}
int main() 
{
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    signal(SIGPIPE, handle_signal);
    signal(SIGSEGV, handle_signal);
    LOG_INFO("Client start");
    Common *client=new Client;
    Client* ptr=dynamic_cast<Client*>(client);
    ptr->ClientRun(1025,client->UDP_SOCK_DGRAM_);

    Common *client1=new Client;
    Client* ptr1=dynamic_cast<Client*>(client1);
    ptr1->ClientRun(8080,client1->TCP_SOCK_STREAM_);
    while(1){
         if(ptr->GetThreadExitFlag()){
            delete client;
            break;
        }
        if(ptr1->GetThreadExitFlag()){
            delete client1;
            //delete client1;
            //break;
        }
        sleep(1);
    }
    return 0;
}