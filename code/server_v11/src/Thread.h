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
    using ThreadFunc = std::function<void()>;//�̺߳����ĺ�������  �����ͺ������󣬾Ϳ��Դ��� 

    explicit Thread(ThreadFunc);//���캯�� 
    ~Thread();//�������� 

    void start();//������ǰ�߳� 
    void join();//��ǰ�̵߳ȴ������߳�������������ȥ 
    std::thread::id getId()const { return std::this_thread::get_id(); }
    bool started() const { return started_; }
private:
    bool started_;//������ǰ�߳� 
    bool joined_;//��ǰ�̵߳ȴ������߳�������������ȥ 
    std::shared_ptr<std::thread> thread_;//�Լ����ƿ��̶߳��������ʱ�� 
    std::thread::id id_;
    ThreadFunc func_;//�洢�̺߳��� 

};
