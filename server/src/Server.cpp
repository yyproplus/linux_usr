#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <pthread.h>
#include <poll.h>
#include <sys/resource.h>
#include "Server.h"
#include "Common.h"
#include "Log.h"
#include "UdpEvent.h"
#define PORT 65432

Server::Server():socket_type_(0)
{
    printf("Server()\n");
}

Server::~Server() {
    printf("~Server()\n");
}

int Server::Init(SocketType socket_type,int port)
{
    socket_type_=socket_type;
    port_=port;
    int ret=SocketCreate();
    if(ret<0)return -1;
    ret=SetSocketNonblock();
    if(ret<0)return -1;
    ret=BindSocket();
    if(ret<0)return -1;
    if(socket_type_==TCP_SOCK_STREAM_)
    ret=ConnectStart();
    else{
        bind_socket_=server_fd_;//udp连接不需要监听
    }
#ifdef SOCKET_BLOCK
    if(ret<0)return -1;
#endif
    return 0;
}

int Server::SocketCreate()
{
    if ((server_fd_ = socket(AF_INET, socket_type_, 0)) == 0) {
        perror("socket failed\n");
        return -1;
    }
    //LOG_DEBUG("server_fd_=%d",server_fd_);
    return 0;
}

int Server::BindSocket()
{
    address_.sin_family = AF_INET;
    address_.sin_addr.s_addr = INADDR_ANY;
    address_.sin_port = htons(port_);
    if (bind(server_fd_, (struct sockaddr *)&address_, sizeof(address_)) < 0) {
        perror("bind failed\n");
        close(server_fd_);
        return -1;
    }
    if (listen(server_fd_, 3) < 0) {
        perror("listen failed");
        close(server_fd_);
        return -1;
    }
    return 0;
}

int Server::ConnectStart()
{
    return SelectCheckSocketStatus();
}

int Server::SocketDestory()
{
    close(bind_socket_);
    if(socket_type_==UDP_SOCK_DGRAM_)return 0;
    close(server_fd_);
    return 0;
}

int Server::Read(void *data, int MaxCount)
{
    std::lock_guard<std::mutex> lock(read_lock_);
    int valread = read(bind_socket_, data, MaxCount);
    return valread;
}

int Server::Write(const void *buf, size_t len, int flags)
{
    std::unique_lock<std::mutex> lock(write_lock_,std::defer_lock);
    write_lock_.lock();
    int ret=send(bind_socket_, buf, len, flags);
    write_lock_.unlock();
    if(ret<0)return -1;
    return 0;
}


SelectModeReturnCode Server::SelectNoblockMode()
{
    FD_ZERO(&read_fds_);
    FD_ZERO(&write_fds_);

    // 将sockfd添加到读和写的文件描述符集中
    FD_SET(bind_socket_, &read_fds_);
    FD_SET(bind_socket_, &write_fds_);
    max_fd_ = bind_socket_;

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
    int read_status=FD_ISSET(bind_socket_,&read_fds_);
    int write_status=FD_ISSET(bind_socket_,&write_fds_);
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

int Server::SelectCheckSocketStatus()
{
    int addrlen=sizeof(address_);
    FD_ZERO(&read_fds_);
    FD_SET(server_fd_, &read_fds_);

    struct timeval timeout;
    timeout.tv_sec = 5;   // 等待5秒
    timeout.tv_usec = 0;

    // 检查是否有新连接
    int activity = select(server_fd_ + 1, &read_fds_, NULL, NULL, &timeout);
//这个非阻塞5s，查询该tcp socket是否可用
    if (activity < 0 && errno != EINTR) {
        perror("select 错误");
    }
    if (activity == 0) {
        std::cerr << "select 超时，没有新连接。" << std::endl;
        tcp_connect_status_ = TCP_CONNECT_TIMEOUT;
        return -1;
    }
    if (FD_ISSET(server_fd_, &read_fds_)) {
        // 有新的连接，调用 accept
        //LOG_DEBUG("server_fd_=%d,len=%d,address_.sin_port=%d,activity=%d",server_fd_,addrlen,address_.sin_port,activity);
        bind_socket_ = accept(server_fd_, (struct sockaddr *)&address_, (socklen_t*)&addrlen);

        if (bind_socket_ < 0) {
            perror("accept 错误");
            // 打印错误码
            std::cerr << "错误代码: " <<strerror(errno) << std::endl; 
            tcp_connect_status_ = TCP_CONNECT_FAILED;
            close(server_fd_);
            return -1;
        }
        // 成功处理连接
        tcp_connect_status_ = TCP_CONNECT_SUCCESS;
    }else {
        tcp_connect_status_=TCP_CONNECT_TIMEOUT;
        return -1;
    }
    return 0;
}

int Server::GetSocketBuff()
{
    int sndbuf;
    int recbuf;
    socklen_t len = sizeof(sndbuf);
    if (getsockopt(server_fd_, SOL_SOCKET, SO_SNDBUF, &sndbuf, &len) == 0) {
        printf("Send buffer size: %d bytes\n", sndbuf);
    } else {
        perror("getsockopt");
    }
    if (getsockopt(server_fd_, SOL_SOCKET, SO_RCVBUF, &recbuf, &len) == 0) {
        printf(" receive buffer size: %d bytes\n", recbuf);
    } else {
        perror("getsockopt");
    }
    return 0;
}

int Server::SetSocketBuff()
{
    int size = 65536;  // 64KB
    if (setsockopt(server_fd_, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size)) != 0) {
        perror("Send setsockopt");
        return -1;
    }
    if (setsockopt(server_fd_, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size)) != 0) {
        perror("Receive setsockopt");
        return -1;
    }
    return 0;
}

int Server::SetSocketMessageSysTimeout(int send_timeout,int rec_timeout)
{
    struct timeval timeout1;
    timeout1.tv_sec = rec_timeout;  // 设置超时时间为5秒
    timeout1.tv_usec = 0;

    struct timeval timeout2;
    timeout2.tv_sec = send_timeout;  // 设置超时时间为5秒
    timeout2.tv_usec = 0;
    // 设置接收超时
    setsockopt(server_fd_, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout1, sizeof(timeout1));

    // 设置发送超时
    setsockopt(server_fd_, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout2, sizeof(timeout2));
    return 0;
}

int Server::SetSocketNonblock()
{
    int flags = fcntl(server_fd_, F_GETFL, 0);
    if (fcntl(server_fd_, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("server_fd_ SetSocketNonblock failed\n");
        return -1;
    }
    return 0;
}

int Server::GetProcessFdCountMax()
{
    struct rlimit limit;
    if (getrlimit(RLIMIT_NOFILE, &limit) == 0) {
        /* printf("Current file descriptor limit: %d\n",limit.rlim_cur);
        printf("Maximum allowed file descriptors: %d\n",limit.rlim_max); */
        if (server_fd_ >= limit.rlim_cur) {
            perror("Error: max_fd exceeds current file descriptor limit.\n");
            return -1;
        }
    } else {
        perror("getrlimit error");
        return -1;
    }
    return 0;
}
