#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <memory>
#include <signal.h>
#include "Log.h"
#include "Common.h"
#include "Main.h"
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

void Main::MainInit()
{
    AbnormalSignalCapture();
    LogDegreeSet();
}

void Main::AbnormalSignalCapture()
{
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    signal(SIGPIPE, handle_signal);
    signal(SIGSEGV, handle_signal);
}

void Main::LogDegreeSet()
{
    SetLogDegree(LOG_LEVEL_DEBUG);
}

int main() {

    Main *main_obj=new Main();
    main_obj->MainInit();
    //提交测试2
    main_obj->tcp_client_.TcpLayerInit(TCP_SOCK_STREAM_,8080);
    while(1){
        sleep(1);
    }
    if(main_obj){
        delete main_obj;
        main_obj=nullptr;
    }
    return 0;
}