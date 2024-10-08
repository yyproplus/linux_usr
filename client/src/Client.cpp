#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>
#include <shared_mutex>
#include <sys/time.h>
#include <sys/types.h>
#include <poll.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <sys/select.h>
#include "Client.h"
#include "Log.h"
#define PORT 65432
Client::Client(/* args */)
{
    printf("Client()\n");
}

Client::~Client()
{
    printf("~Client()\n");
    SocketDestory();
}

int Client::Init(SocketType socket_type,int port)
{
    socket_type_=socket_type;
    port_=port;
    int ret=SocketCreate();
    if(ret<0){
        printf("SocketCreate failed\n");
        return -1;
    }
#ifndef BLOCK_SOCKET
    ret=SetSocketNonblock();
    if(ret<0){
        printf("SetSocketNonblock failed\n");
        return -1;
    }
#endif
    ret=BindSocket();
    if(ret<0){
        printf("BindSocket failed\n");
        return -1;
    }
    ret=ConnectStart();
#ifdef SOCKET_BLOCK
    if(ret<0){
        printf("ConnectStart failed\n");
        return -1;
    }
#endif
    return 0;
}

int Client::SocketCreate()
{
     // 创建套接字
    if ((client_sock_ = socket(AF_INET, socket_type_, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
    return 0;
}

int Client::BindSocket()
{
    client_addr_.sin_family = AF_INET;
    client_addr_.sin_port = htons(port_);
    
    // 将IP地址从文本转换为二进制形式
    if (inet_pton(AF_INET, "192.168.31.127", &client_addr_.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    return 0;
}

int Client::ConnectStart()
{
    // 尝试连接服务器
    if (connect(client_sock_, (struct sockaddr *)&client_addr_, sizeof(client_addr_)) < 0) {
        if (errno == EINPROGRESS) {
            // 连接正在进行，使用 select 等待连接完成
            fd_set write_fds;
            FD_ZERO(&write_fds);
            FD_SET(client_sock_, &write_fds);
            struct timeval timeout;
            timeout.tv_sec = 5;  // 等待5秒
            timeout.tv_usec = 0;
            int activity = select(client_sock_ + 1, NULL, &write_fds, NULL, &timeout);
            if (activity > 0) {
                // 检查连接是否成功
                int so_error;
                socklen_t len = sizeof(so_error);
                getsockopt(client_sock_, SOL_SOCKET, SO_ERROR, &so_error, &len);
                if (so_error == 0) {
                    printf("Connected successfully!\n");
                } else {
                    printf("Connection failed: %s\n", strerror(so_error));
                    return -1;
                }
            } else {
                printf("Connection timed out.\n");
                return -1;
            }
        } else {
            perror("connect failed");
            return -1;
        }
    }
    return 0;
}

int Client::Read(void *data, int MaxCount)
{
    std::unique_lock<std::mutex> lock(read_lock_,std::defer_lock);
    read_lock_.lock();
    int ret=read(client_sock_,data, MaxCount);
    read_lock_.unlock();
    return ret;
}

int Client::Write(const void *buf, size_t len, int flags)
{
    std::unique_lock<std::mutex> lock(write_lock_);
    int ret=send(client_sock_,buf, len, flags);
    return ret;
}

int Client::SocketDestory()
{
    close(client_sock_);
    return 0;
}

int Client::GetSocketBuff()
{
    int sndbuf;
    int recbuf;
    socklen_t len = sizeof(sndbuf);
    if (getsockopt(client_sock_, SOL_SOCKET, SO_SNDBUF, &sndbuf, &len) == 0) {
        printf(" Send buffer size: %d bytes\n", sndbuf);
    } else {
        perror("getsockopt");
    }
    if (getsockopt(client_sock_, SOL_SOCKET, SO_RCVBUF, &recbuf, &len) == 0) {
        printf(" receive buffer size: %d bytes\n", recbuf);
    } else {
        perror("getsockopt");
    }
    return 0;
}

int Client::SetSocketBuff()
{
    int size = 65536;  // 64KB
    if (setsockopt(client_sock_, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size)) != 0) {
        perror("send setsockopt");
        return -1;
    }
    if (setsockopt(client_sock_, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size)) != 0) {
        perror("receive setsockopt");
        return -1;
    }
    return 0;
}

int Client::SetSocketMessageSysTimeout(int send_timeout, int rec_timeout)
{
    struct timeval timeout1;
    timeout1.tv_sec = rec_timeout;  // 设置超时时间为5秒
    timeout1.tv_usec = 0;

    struct timeval timeout2;
    timeout2.tv_sec = send_timeout;  // 设置超时时间为5秒
    timeout2.tv_usec = 0;
    // 设置接收超时
    setsockopt(client_sock_, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout1, sizeof(timeout1));

    // 设置发送超时
    setsockopt(client_sock_, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout2, sizeof(timeout2));
    return 0;
}

int Client::SetSocketNonblock()
{
    int flags = fcntl(client_sock_, F_GETFL, 0);
    if (fcntl(client_sock_, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("client_sock_ SetSocketNonblock failed\n");
        return -1;
    }
    return 0;
}

int Client::GetProcessFdCountMax()
{
    struct rlimit limit;
    if (getrlimit(RLIMIT_NOFILE, &limit) == 0) {
        /* printf("Current file descriptor limit: %d\n",limit.rlim_cur);
        printf("Maximum allowed file descriptors: %d\n",limit.rlim_max); */
        if (max_fd_ >= limit.rlim_cur) {
            perror("Error: max_fd exceeds current file descriptor limit.\n");
            return -1;
        }
    } else {
        perror("getrlimit error");
        return -1;
    }
    return 0;
}

int Client::SendFixData(uint8_t pdata)
{
    uint8_t data[]={0x55,0xAA,0x05,0x01,0x00};
    data[3]=pdata;
    int len=sizeof(data);
    uint8_t checkValue=0x00;
    for(int i=0;i<len-1;i++)
    checkValue^=data[i];
    data[len-1]=checkValue;
    return Write(data,len,0);
}

SelectModeReturnCode Client::SelectNoblockMode()
{
    FD_ZERO(&read_fds_);
    FD_ZERO(&write_fds_);

    // 将sockfd添加到读和写的文件描述符集中
    FD_SET(client_sock_, &read_fds_);
    FD_SET(client_sock_, &write_fds_);
    max_fd_ = client_sock_;

    // 设置select超时时间
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    if(max_fd_>=FD_SETSIZE||GetProcessFdCountMax()<0){
        printf("System fd no enough\n");
        return SELECT_ERROR;
    }
    int activity = select(max_fd_ + 1, &read_fds_, &write_fds_, NULL, &timeout);

    if (activity < 0 && errno != EINTR) {
        perror("select error\n");
        return SELECT_ERROR;
    }
    int read_status=FD_ISSET(client_sock_,&read_fds_);
    int write_status=FD_ISSET(client_sock_,&write_fds_);
    if(read_status&&write_status){
        return SELECT_RW;
    }else if(read_status){
        return SELECT_READ_ONLY;
    }else if(write_status){
        return SELECT_WRITE_ONLY;
    }else{
        return SELECT_NORW;
    }
    return SELECT_SUCCESS;
}

int Client::PollNoblockMode()
{
    struct pollfd fds[1];
    int timeout = 5000; // 5秒超时

     // 配置pollfd结构体
    fds[0].fd = client_sock_;
    fds[0].events = POLLIN | POLLOUT; // 监听可读和可写事件
    fds[0].revents = 0;
    int ret = poll(fds, 1, timeout);
    if (ret < 0) {
        perror("poll error\n");
        return SELECT_ERROR;
    } else if (ret == 0) {
        // 超时处理
        printf("Poll timeout\n");
        return SELECT_BUFF_FULL;
    }

    if (fds[0].revents & POLLIN) {
        // 套接字可读，接收数据
        memset(buffer_, 0, BUFFER_SIZE);
        int bytes_received = recv(client_sock_, buffer_, sizeof(buffer_), 0);
        if (bytes_received > 0) {
            printf("Received: %s\n", buffer_);
        } else if (bytes_received == 0) {
            // 连接关闭
            printf("Server closed the connection\n");
            return SELECT_REMOTE_SOCKET_CLOSE_WITH_EPIPE;
        } else {
            perror("recv error");
            return SELECT_ERROR;
        }
    }

    if (fds[0].revents & POLLOUT) {
        // 套接字可写，发送数据
        const char *msg = "Hello, Server!";
        int bytes_sent = send(client_sock_, msg, strlen(msg), 0);
        if (bytes_sent < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // 发送缓冲区满，稍后重试
               return SELECT_BUFF_FULL;
            } else {
                perror("send error");
                 return SELECT_ERROR;
            }
        } else {
            printf("Sent: %s\n", msg);
        }
    }
    return SELECT_SUCCESS;
}

int Client::EpollNoblockMode()
{
    return 0;
}
