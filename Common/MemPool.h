#ifndef MEMPOOL_H
#define MEMPOOL_H
#include <queue>
#include <memory>
#include <mutex>
#include <stdio.h>
#include "DataTransport.h"
#define MAX_MEM_POOL_COUNT 2000
std::vector<std::shared_ptr<DataTransport>> create_mem_queue_;
std::deque<std::shared_ptr<DataTransport>> recycle_mem_queue_;
std::mutex recycle_mem_mutex;
std::mutex create_mem_mutex;
class MemPool
{
private:
    /* data */
public:
    MemPool(DataTransport &recycle):recycle_(recycle)
    {
        printf("MemPool()\n");
    }

    ~MemPool()
    {
        printf("~MemPool()\n");
    }

    int CreateMemPool()
    {
        std::shared_ptr<DataTransport> mem_ptr;
        std::unique_lock<std::mutex> lock(recycle_mem_mutex);
        if(!recycle_mem_queue_.empty()){
            mem_ptr=recycle_mem_queue_.front();
        }else{
            mem_ptr=std::make_shared<DataTransport>();
            recycle_mem_queue_.push_back(mem_ptr);
        }
        create_mem_queue_.push_back(mem_ptr);
        return 0;
    }

    int RecycleMemPool(std::shared_ptr<DataTransport> recycle_ptr)
    {
        std::unique_lock<std::mutex> lock(recycle_mem_mutex);
        if(recycle_mem_queue_.size()<MAX_MEM_POOL_COUNT){
        }
        else{
            recycle_mem_queue_.pop_front();
        }
        recycle_mem_queue_.push_back(recycle_ptr);
        return 0;
    }

private:
    DataTransport recycle_;
};

#endif//MEMPOOL_H