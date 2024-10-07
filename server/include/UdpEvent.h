#ifndef UDP_EVENT_H
#define UDP_EVENT_H
#include <stdio.h>
#include <memory>
#include "Common.h"
#include "Log.h"
class UdpEventBase
{
public:
    UdpEventBase(){
        printf("UdpEventBase()\n");
    }
    virtual ~UdpEventBase(){
        printf("~UdpEventBase()\n");
    }
    virtual void EventDealWith()=0;
private:
};

class UdpEventPullSteam:public UdpEventBase
{
public:
    UdpEventPullSteam(){
        printf("UdpEventPullSteam()\n");
    }
    ~UdpEventPullSteam(){
        printf("~UdpEventPullSteam()\n");
    }
    virtual void EventDealWith()override{
        printf("Server start push stream\n");
    }
};

class UdpEventStopPullSteam:public UdpEventBase
{
public:
    UdpEventStopPullSteam(){
        printf("UdpEventStopPullSteam()\n");
    }
    ~UdpEventStopPullSteam(){
        printf("~UdpEventStopPullSteam()\n");
    }
    virtual void EventDealWith()override{
        printf("Server stop stream\n");
    }
};

class Context
{
private:
    /* data */
   std::shared_ptr<UdpEventBase> udp_event_base_ptr_;
public:
    Context(std::shared_ptr<UdpEventBase> udp_event_base_ptr):udp_event_base_ptr_(udp_event_base_ptr){
        printf("Context()\n");
    }
    ~Context(){
        printf("~Context()\n");
    }
    void SetNewContext(std::shared_ptr<UdpEventBase> udp_event_base_ptr){
        udp_event_base_ptr_=udp_event_base_ptr;
    }
    void ExcuteNewContext(){
        if(!udp_event_base_ptr_){
            LOG_ERROR("udp_event_base_ptr_ is nullptr\n");
            return ;
        }
        udp_event_base_ptr_->EventDealWith();//这里
    }
};
#endif//UDP_EVENT_H