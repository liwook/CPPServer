#pragma once
#include"logstream.h"
#include<vector>
#include<string>
#include<memory>
#include<condition_variable>
#include<thread>

//异步日志，单独开一个线程来负责输出日志
class AsyncLogger
{
public:
	// 参数为日志文件名，默认每隔3秒冲刷缓冲区到磁盘文件中
	//AsyncLogger(const std::string fileName, int flushInterval = 3);
    AsyncLogger(const std::string basename, off_t rollSize, int flushInterval = 3);


    ~AsyncLogger()
    {
        if (is_running_){
            stop();
        }
    }

    void Append(const char* logline, int len);

    //开始异步日志线程
    void start()
    {
        is_running_ = true;
        thread_ = std::thread([this]() {ThreadFunc(); });
    }

    void stop()
    {
        is_running_ = false;
        cond_.notify_one();// 停止时notify以便将残留在缓存区中的数据输出
        thread_.join();
    }

public:
    void ThreadFunc();  //后端日志线程函数

    //固定大小的缓冲区
    using Buffer = FixedBuffer<KLargeBuffer>;
    using BufferPtr = std::unique_ptr<Buffer>;
    using BufferVector = std::vector<std::unique_ptr<Buffer>>;

    const int flushInterval_;   //需要冲刷的时间间隔
    bool is_running_;
    //std::string fileName_;      //日志文件名
    std::string basename_;      //日志文件的基础名

    const off_t rollsize_;

    std::thread thread_;    //log后端线程

    //保护下面的四个变量
    std::mutex mutex_;
    std::condition_variable cond_;

    BufferPtr currentBuffer_;   //当前的缓冲区指针
    BufferPtr nextBuffer_;      //下一个可用的缓冲区指针
    BufferVector buffers_;      //准备交付给后端log线程使用的缓冲区vector
};