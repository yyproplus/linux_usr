#ifndef TCP_H_
#define TCP_H_
#include "Common.h"
#include "Client.h"
#include "ThreadManager.h"
#include "Queue.h"
#include "DataTransport.h"

class TcpLayer
{
public:
    TcpLayer();
    ~TcpLayer();
    int TcpLayerInit(SocketType socket_type,int port);
    void TcpLayerRevThread();
    void TcPLayerSendThread();
    void TcpLayerDataParseThread();
    void TcpLayerThreadResourceDestroy();
private:
    Client client_;
    ThreadManager *tcp_rec_thread_;
    ThreadManager *tcp_dataParse_thread_;
    ThreadManager *tcp_send_thread_;
    uint8_t buffer_[BUFFER_SIZE];
    Queue<DataTransport> client_queue_list_;
    int len_;
    uint8_t pdata[512];
    int rest_len_;
    int frame_len_min_;
    SelectModeReturnCode select_status_;
};
#endif//UDP_H_

//1、初始化接口
//socket创建、绑定地址和端口号、连接

//2、创建接收数据线程

//3、创建queue

//4、创建解析数据线程