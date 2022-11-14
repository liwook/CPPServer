#include "Thread.h"

//std::atomic_int Thread::numCreated_=0;//��ʼ��

Thread::Thread(ThreadFunc func)//���캯��
    : started_(false)
    , joined_(false)
    , func_(std::move(func))
{
}

Thread::~Thread()//��������
{
    if (started_ && !joined_)//�߳��Ѿ������������Ҳ��ǹ����߳�join
    {
        thread_->detach();
        //thread���ṩ�����÷����̵߳ķ���������1���ػ��̣߳������߳̽������ػ��߳��Զ�����
    }
}

void Thread::start()//һ��Thread���󣬼�¼�ľ���һ�����̵߳���ϸ��Ϣ
{
    started_ = true;
    //�����߳�
    thread_ =std::make_shared<std::thread>([this]() {
        //����һ�����̣߳�ר��ִ�и��̺߳���
        func_();
    });
}

void Thread::join()
{
    joined_ = true;
    thread_->join();
}
