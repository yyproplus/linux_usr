#ifndef QUEUE_H_
#define QUEUE_H_
#include <iostream>
#include <queue>
#include <mutex>
#include <memory>
template <typename T>
class Queue
{
public:
    Queue(int MaxFrameCount):max_queue_count_(MaxFrameCount)
    {
        printf("Queue()\n");
    }

    ~Queue()
    {
        printf("~Queue()\n");
    }

    int Push(T *data)
    {
        if(!data)return -1;
        if(Size()>=max_queue_count_){
            printf("queue_list_ FULL\n");
            return -1;
        }
        std::shared_ptr<T> dest_data=std::make_shared<T>(*data);
        std::lock_guard<std::mutex> lock(queue_mutex_);
        queue_list_.push(dest_data);
        return 0;
    }

    T* Read()
    {
        std::shared_ptr<T> data;
        if(!Empty()){
            std::lock_guard<std::mutex> lock(queue_mutex_);
            data=queue_list_.front();
        }
        return data.get();//对，不进行封装的，要判断，访问stl容易都要判断的，我一般会自己封装一下，对，没怎么用过没封装的，一般封装对大小进行限制，要不然内存很容易不够，
    }//有些场景是只读，并不一定pop，只是拿出来用一下，不销毁，可能后面会继续用，那read，不是不行了
    // 那就分情况看了 刚才那个地方是用一下还是啥，那个是用一下就不要了 那就不管是不是空都要pop，不太可能为空啊
    void Pop()
    {
        if(!Empty()){
            std::lock_guard<std::mutex> lock(queue_mutex_);
            queue_list_.pop();
        }
    }

    int Size()
    {
        int size=0;
        std::unique_lock<std::mutex> lock(queue_mutex_);
        size=queue_list_.size();
        return size;
    }

    bool Empty()
    {
        bool ret=false;
        std::unique_lock<std::mutex> lock(queue_mutex_,std::defer_lock);
        queue_mutex_.lock();
        ret=(!queue_list_.empty())?false:true;
        queue_mutex_.unlock();
        return ret;
    }

    bool Destroy(){
        std::unique_lock<std::mutex> lock(queue_mutex_);
        while (!queue_list_.empty())
        {
            queue_list_.pop();
        }
    }
private:
    int max_queue_count_;
    std::queue<std::shared_ptr<T>> queue_list_;
    std::mutex queue_mutex_;
};
#endif//QUEUE_H_