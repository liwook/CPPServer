#include "Thread.h"

//std::atomic_int Thread::numCreated_=0;//初始化

Thread::Thread(ThreadFunc func)//构造函数
    : started_(false)
    , joined_(false)
    , func_(std::move(func))
{
}

Thread::~Thread()//析构函数
{
    if (started_ && !joined_)//线程已经运行起来并且不是工作线程join
    {
        thread_->detach();
        //thread类提供的设置分离线程的方法，成了1个守护线程，当主线程结束，守护线程自动结束
    }
}

void Thread::start()//一个Thread对象，记录的就是一个新线程的详细信息
{
    started_ = true;
    //开启线程
    thread_ =std::make_shared<std::thread>([this]() {
        //开启一个新线程，专门执行该线程函数
        func_();
    });
}

void Thread::join()
{
    joined_ = true;
    thread_->join();
}
