#include <stdio.h>
#include "TcpLayer.h"
#include "Log.h"
#include "ThreadManager.h"
TcpLayer::TcpLayer():server_queue_list_(100)
{
    memset(&server_,0,sizeof(Server));
    printf("TcpLayer()\n");
}

TcpLayer::~TcpLayer()
{
    printf("~TcpLayer()\n");
    TcpLayerThreadResourceDestroy();
}

int TcpLayer::TcpLayerInit(SocketType socket_type,int port)
{
    int ret=server_.Init(socket_type,port);
    if(ret<0){
        LOG_ERROR("server_.Init error");
        return ret;
    }
    tcp_rec_thread_=new ThreadManager();
    tcp_rec_thread_->ThreadCreate(std::bind(&TcpLayer::TcpLayerRevThread,this));
    tcp_dataParse_thread_=new ThreadManager();
    tcp_dataParse_thread_->ThreadCreate(std::bind(&TcpLayer::TcpLayerDataParseThread,this));
    return 0;
}

void TcpLayer::TcpLayerRevThread()
{
    SelectModeReturnCode select_status_=server_.SelectNoblockMode();
    if(select_status_==SELECT_READ_ONLY||select_status_==SELECT_RW){
        memset(pdata,0,sizeof(pdata));
        len_=server_.Read(pdata,sizeof(pdata));
        if (len_ < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // 套接字暂时没有数据可读，稍后重试
                perror("SELECT_BULL_NULL\n");
            } else if (errno == ECONNRESET) {
                // 连接被重置，关闭套接字并清理资源
                perror("SELECT_ECONNRESET\n");
                TcpLayerThreadResourceDestroy();
            } else if (errno == EINTR) {
                // 被信号中断，通常需要重试
                perror("SELECT_EINTR_CONTINUE\n");
                //continue; // 或者重新执行 recv 操作
            } else {
                // 其他错误，处理或记录错误信息
                perror("Read SELECT_ERROR\n");
                TcpLayerThreadResourceDestroy();
            }
        } else if (len_ == 0) {
            // 连接关闭，处理连接关闭的情况
            perror("SELECT_REMOTE_SOCKET_CLOSE_WITH_EPIPE\n");
            TcpLayerThreadResourceDestroy();
        } else if(len_>0||(rest_len_>=frame_len_min_)){
            if((rest_len_+len_)<sizeof(buffer_)){
                memcpy(buffer_+rest_len_,pdata,len_);
                rest_len_+=len_;
                uint8_t check_value=0;
                for(int i=0;i<rest_len_;i++)
                if(buffer_[i]==0x55&&buffer_[i+1]==0xAA){
                    if((rest_len_-i)>=buffer_[i+2]&&buffer_[i+2]){
                        for(int j=i;j<buffer_[i+2]+i;j++){
                            check_value^=buffer_[j];
                        }
                        if(!check_value){
                            DataTransport data(buffer_[i+2]);
                            memcpy(data.data_.get(),buffer_+i,buffer_[i+2]);
                            data.len_=buffer_[i+2];
                            server_queue_list_.Push(&data);
                        }else{
                            LOG_ERROR("check error");
                        }
                        //printf("i=%d,buffer_[i+2]=%d,rest_len=%d\n",i,buffer_[i+2],rest_len);
                        memcpy(buffer_,buffer_+i+buffer_[i+2],rest_len_-(buffer_[i+2]+i));
                        rest_len_-=(buffer_[i+2]+i);
                    }
                }
            }
            else {
                LOG_ERROR("buffer_ FULL");
                memset(buffer_,0,sizeof(buffer_));
                rest_len_=0;
            }
        }
    }
}

void TcpLayer::TcpLayerDataParseThread()
{
    const DataTransport *data=server_queue_list_.Read();
    if(data){
        if(data->data_[3]==REQUEST_PUSH_STEAM_EVENT){
           LOG_INFO("REQUEST_PUSH_STEAM_EVENT\n");
        }
        server_queue_list_.Pop();
    }
}

void TcpLayer::TcpLayerThreadResourceDestroy()
{
    if(tcp_rec_thread_){
        delete tcp_rec_thread_;
        tcp_rec_thread_=nullptr;
    }
    if(tcp_dataParse_thread_){
        delete tcp_dataParse_thread_;
        tcp_dataParse_thread_=nullptr;
    }
}
