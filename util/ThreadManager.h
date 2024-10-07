#ifndef THREAD_MANAGER_
#define THREAD_MANAGER_
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <stdio.h>

class ThreadManager
{
public:
    ThreadManager():isRuning(false),workerThread(nullptr)
    {
        printf("Thread()\n");
    }
    ~ThreadManager()
    {
        printf("~Thread()\n");
        ThreadDestroy();
    }
    void ThreadCreate(std::function<void()> task){
        if(isRuning){
            printf("Thread is already running.\n");
            return;
        }
        isRuning=true;
        workerThread=new std::thread([this,task](){
            while(isRuning){
                task();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        });
    }
    void ThreadDestroy(){
        if(isRuning){
            isRuning=false;
            if(workerThread&&workerThread->joinable()){
                workerThread->join();
            }
            delete workerThread;
            workerThread=nullptr;
        }
    }
private:
    std::atomic<bool> isRuning;
    std::thread* workerThread;
};
#endif //THREAD_MANAGER_