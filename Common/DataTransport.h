#ifndef DATATRANSPORT_H_
#define DATATRANSPORT_H_
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#define MALLOC_SIZE 10
template<typename T>
std::shared_ptr<T[]> make_shared_array(std::size_t size) {
    return std::shared_ptr<T[]>(new T[size]);
}

class DataTransport
{
public:
    DataTransport(int len):len_(len),data_(std::shared_ptr<uint8_t[]>(new uint8_t[len_]))
    {
       // printf("DataTransport()\n");
        memset(data_.get(),0,len_);
    }
    DataTransport(const DataTransport &data){
       // printf("DataTransport(const DataTransport &data)\n");
        data_=std::shared_ptr<uint8_t[]>(new uint8_t[data.len_]);
        this->len_=data.len_;
        memcpy(data_.get(),data.data_.get(),data.len_);
    }
    DataTransport& operator=(const DataTransport &data){
        if(&data==this)return *this;
        data_=std::shared_ptr<uint8_t[]>(new uint8_t[data.len_]);
        this->len_=data.len_;
        memcpy(data_.get(),data.data_.get(),data.len_);
        return *this;
    }
    ~DataTransport(){
        //printf("~DataTransport()\n");
    }
    int len_=0;
    std::shared_ptr<uint8_t[]> data_;
private:
};
#endif//DATATRANSPORT_H_