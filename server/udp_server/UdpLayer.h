#ifndef UDP_LAYER_H
#define UDP_LAYER_H
#include "Common.h"
#include "Server.h"
#include "ThreadManager.h"
#include "Queue.h"
#include "DataTransport.h"
class UdpLayer
{
public:
    UdpLayer();
    ~UdpLayer();
    int UdpLayerInit(SocketType socket_type,int port);
    void UdpLayerRevThread();
    void UdpLayerDataParseThread();
    void UdpLayerThreadResourceDestroy();
private:
    Server server_;
    ThreadManager *udp_rec_thread_;
    ThreadManager *udp_dataParse_thread_;
    uint8_t buffer_[BUFFER_SIZE];
    Queue<DataTransport> server_queue_list_;
    int len_=0;
    uint8_t pdata[512]={0};
    int rest_len_=0;
    int frame_len_min_=5;
};
#endif//UDP_LAYER_H