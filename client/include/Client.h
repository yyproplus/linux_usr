#ifndef CLIENT_H_
#define CLIENT_H_
#include <thread>
#include <mutex>
#include <fcntl.h>
#include <sys/select.h>
#include <errno.h>
#include <netinet/in.h>  // 定义了 sockaddr_in 结构
#include <arpa/inet.h>   // 定义了 inet_pton 和 inet_ntop 等函数
#include "Common.h"
#define BUFFER_SIZE 1024
class Client:public Common
{
private:
    /* data */
public:
    Client(/* args */);
    ~Client();
    virtual int Init(SocketType socket_type,int port)override;
    virtual int SocketCreate()override;
    virtual int BindSocket()override;
    virtual int ConnectStart()override;
    virtual int Read(void* data,int MaxCount)override;
    virtual int Write(const void *buf, size_t len, int flags)override;
    virtual int SocketDestory()override;
    virtual int GetSocketBuff()override;
    virtual int SetSocketBuff()override;
    virtual int SetSocketMessageSysTimeout(int send_timeout,int rec_timeout)override;
    virtual int SetSocketNonblock()override;
    virtual int GetProcessFdCountMax()override;
    int SendFixData(uint8_t pdata);
    SelectModeReturnCode SelectNoblockMode();
    int PollNoblockMode();
    int EpollNoblockMode();
private:
    int client_sock_;
    std::thread client_thread_;
    struct sockaddr_in client_addr_;
    uint8_t buffer_[BUFFER_SIZE];
    bool thread_exit=false;
    int port_;
    int socket_type_;
    std::mutex read_lock_;
    std::mutex write_lock_;
    typedef int (Client::*SetNoblockFun)();
    SetNoblockFun  SetNoblockCall[3];
    fd_set read_fds_, write_fds_;
    int max_fd_;
};
#endif//CLIENT_H_