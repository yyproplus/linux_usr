#ifndef SOCKET_CREATE_H
#define SOCKET_CREATE_H
#include <mutex>
#include <thread>
#include <fcntl.h>
#include <sys/select.h>
#include <netinet/in.h>  // 定义了 sockaddr_in 结构
#include <arpa/inet.h>   // 定义了 inet_pton 和 inet_ntop 等函数
#include <errno.h>
#include <condition_variable>
#include "DataTransport.h"
#include "Queue.h"
#include "Common.h"
#include "UdpEvent.h"
#define BUFFER_SIZE 1024
#define QUEUE_COUNT_MAX 100
class Server:public Common
{
public:
    Server();
    virtual ~Server();
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
    SelectModeReturnCode SelectNoblockMode();
    int SelectCheckSocketStatus();
    int SendFixData(uint8_t pdata);
private:
    int server_fd_;
    int bind_socket_;
    struct sockaddr_in address_;
    int port_;
    int socket_type_;
    std::mutex read_lock_;
    std::mutex write_lock_;
    fd_set read_fds_, write_fds_;
    int max_fd_;
    TcpConnectStatus tcp_connect_status_;
    /* Server(const Server&) = delete; // 允许拷贝构造
    Server& operator=(const Server&) = delete; // 禁用赋值操作 */
};
#endif