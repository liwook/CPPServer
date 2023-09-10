#pragma once
#include"logstream.h"
#include<vector>
#include<string>
#include<memory>
#include<condition_variable>
#include<thread>

//�첽��־��������һ���߳������������־
class AsyncLogger
{
public:
	// ����Ϊ��־�ļ�����Ĭ��ÿ��3���ˢ�������������ļ���
	//AsyncLogger(const std::string fileName, int flushInterval = 3);
    AsyncLogger(const std::string basename, off_t rollSize, int flushInterval = 3);


    ~AsyncLogger()
    {
        if (is_running_){
            stop();
        }
    }

    void Append(const char* logline, int len);

    //��ʼ�첽��־�߳�
    void start()
    {
        is_running_ = true;
        thread_ = std::thread([this]() {ThreadFunc(); });
    }

    void stop()
    {
        is_running_ = false;
        cond_.notify_one();// ֹͣʱnotify�Ա㽫�����ڻ������е��������
        thread_.join();
    }

public:
    void ThreadFunc();  //�����־�̺߳���

    //�̶���С�Ļ�����
    using Buffer = FixedBuffer<KLargeBuffer>;
    using BufferPtr = std::unique_ptr<Buffer>;
    using BufferVector = std::vector<std::unique_ptr<Buffer>>;

    const int flushInterval_;   //��Ҫ��ˢ��ʱ����
    bool is_running_;
    //std::string fileName_;      //��־�ļ���
    std::string basename_;      //��־�ļ��Ļ�����

    const off_t rollsize_;

    std::thread thread_;    //log����߳�
    

    //����������ĸ�����
    std::mutex mutex_;
    std::condition_variable cond_;

    BufferPtr currentBuffer_;   //��ǰ�Ļ�����ָ��
    BufferPtr nextBuffer_;      //��һ�����õĻ�����ָ��
    BufferVector buffers_;      //׼�����������log�߳�ʹ�õĻ�����vector
};