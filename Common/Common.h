#ifndef COMMON_H_
#define COMMON_H_
#include <cstddef>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include "DataType.h"
typedef enum SocketType_{
    TCP_SOCK_STREAM_=SOCK_STREAM,
    UDP_SOCK_DGRAM_=SOCK_DGRAM,
}SocketType;
typedef enum NonblockMode_{
    SELECT_MODE=0,
    POLL_MODE=1,
    EPOLL_MODE=2
}NonblockMode;
typedef enum SelectModeReturnCode_{
    SELECT_ERROR=-1,
    SELECT_SUCCESS=0,
    SELECT_BUFF_FULL=1,
    SELECT_REMOTE_SOCKET_CLOSE_WITH_EPIPE=3,//EPIPE表示远程端已经关闭了连接（即已经发送了FIN包）
    SELECT_ECONNRESET=4,//远端重置，程序崩溃或者网络中断
    SELECT_ENOBUFS=5,//系统内存不足
    SELECT_BULL_NULL=6,//读缓冲区没有数据
    SELECT_EINTR_CONTINUE=7,//信号中断,继续重试
    SELECT_READ_ONLY=8,
    SELECT_WRITE_ONLY=9,
    SELECT_RW=10,
    SELECT_NORW=11,
}SelectModeReturnCode;
typedef enum UdpEventMessageQueue_{
    REQUEST_PUSH_STEAM_EVENT=0x01,
    REQUEST_STOP_PUSH_STEAM_EVENT=0x02,
    REQUEST_PULL_STEAM_EVENT=0x03,

}UdpEventMessageQueue;
typedef enum TcpConnectStatus_{
    TCP_WAITCONNECT=0x01,
    TCP_CONNECT_FAILED=0x02,
    TCP_CONNECT_SUCCESS=0x03,
    TCP_DISCONNECT=0x04,
    TCP_CONNECT_TIMEOUT=0x05,
}TcpConnectStatus;

class Common
{
public:
    Common();
    virtual ~Common();
    virtual int Init(SocketType socket_type,int port)=0;
    virtual int SocketCreate()=0;
    virtual int BindSocket()=0;
    virtual int ConnectStart()=0;
    virtual int Read(void* data,int MaxCount)=0;
    virtual int Write(const void *buf, size_t len, int flags)=0;
    virtual int SocketDestory()=0;
    virtual int GetSocketBuff()=0;
    virtual int SetSocketBuff()=0;
    virtual int SetSocketMessageSysTimeout(int send_timeout,int rec_timeout)=0;
    virtual int SetSocketNonblock()=0;
    virtual int GetProcessFdCountMax()=0;
    void SetSocketType(SocketType this_socket_type){
        this_socket_type_=this_socket_type;
    }
private:
    SocketType this_socket_type_;
};
#endif//COMMON_H_