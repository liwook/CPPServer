#pragma once

#include <functional>
#include <thread>
#include <memory>
#include <unistd.h>
#include <string>
#include <atomic>

class Thread
{
public:
    using ThreadFunc = std::function<void()>;//线程函数的函数类型  绑定器和函数对象，就可以传参 

    explicit Thread(ThreadFunc);//构造函数 
    ~Thread();//析构函数 

    void start();//启动当前线程 
    void join();//当前线程等待其他线程完了再运行下去 
    std::thread::id getId()const { return std::this_thread::get_id(); }
    bool started() const { return started_; }
private:
    bool started_;//启动当前线程 
    bool joined_;//当前线程等待其他线程完了再运行下去 
    std::shared_ptr<std::thread> thread_;//自己来掌控线程对象产生的时机 
    std::thread::id id_;
    ThreadFunc func_;//存储线程函数 

};
