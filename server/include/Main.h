#ifndef MAIN_H_
#define MAIN_H_
#include <stdio.h>
#include <mutex>
#include <memory>
#include "Common.h"
#include "TcpLayer.h"
class Main
{
public:
    Main()
    {
        memset(&tcp_server_,0,sizeof(TcpLayer));
        printf("Main()\n");
    }
    ~Main(){
        printf("~Main()\n");
    }
    void MainInit();
    void AbnormalSignalCapture();
    void LogDegreeSet();
    void UdpStart(int port);
    void UdpExit();
    void TcpStart(int port);
    void TcpExit();
public:
    TcpLayer tcp_server_;
    std::mutex tcp_handle_mutex_;
private:
};
void TcpEntrance();
void TcpExit();
#endif//MAIN_H_