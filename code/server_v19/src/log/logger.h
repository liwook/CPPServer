#pragma once

#include"logstream.h"
#include"../timestamp.h"
class Logger
{
public:
	//��־����
	enum class LogLevel
	{
        TRACE,
		DEBUG,	//����ʹ��
		INFO,	//��Ϣ
		WARN,	//����
		ERROR,	//����
		FATAL,	//����
		NUM_LOG_LEVELS,
	};

    //�ж��м���������ж�Ӧ�������
	Logger(const char* FileName,int line,LogLevel level,const char* funcName);
    Logger(const char* file, int line);
    Logger(const char* file, int line, LogLevel level);
    Logger(const char* file, int line, bool toAbort);

	~Logger();

    // ��ȡ��־������
    //LogStream& Stream() { return stream_; }
    LogStream& Stream() { return impl_.stream_; }
    // 
    // ��ȡȫ����־�ȼ������ǵ�ǰ����ĵȼ�
    static LogLevel GlobalLogLevel();
    static std::string LogFileName() { return log_file_basename_; }
    using OutputFunc=void (*)(const char* msg, int len);
    using FlushFunc=void (*)();
    // �����������
    static void SetOutput(OutputFunc);
    // ���ó�ˢ����
    static void SetFlush(FlushFunc);

private:
    //Ϊʲô��Ҫ���Impl�ࣿ���� ��Ϊ��ͬ����־�ȼ��ͻ������Ӧ��ͬ���ֶθ�����
    //��ôLogger�Ĺ��캯���ͻ��ж�������캯�����������ʾ��
    //��ôÿ�����캯��Ҳ��Ҫ����д
    //Logger::Logger(const char* basename, int line, Logger::LogLevel level, const char* funcName)
    //    :stream_()
    //    , level_(level)
    //    , basename_(basename)
    //    , line_(line)
    //    , time_(Timestamp::now())
    //{
    //    formatTime();	//ʱ�����
    //    CurrentThread::tid();	//�����߳�
    //    stream_ << T(CurrentThread::tidString(), CurrentThread::tidStringLength());
    //    stream_ << T(g_loglevel_name[static_cast<int>(level_)], 6);	//��־�ȼ����ַ����Ƕ�����

    //    stream_ << funcName << "():";
    //}
    //�������ǳ������������������������һ��Impl�࣬��Impl��Ĺ��캯��������Щ������֮��ֻ����Logger::Logger()�Ĺ��캯���й���Impl����У�
    //�����ͳ������ɵ�����
    class Impl
    {
    public:
        using  LogLevel = Logger::LogLevel;
        Impl(LogLevel level, const std::string& file, int line);
        void formatTime();
        void finish();

        Timestamp time_;
        LogStream stream_;
        LogLevel level_;
        int line_;          //���� ������__LINE__
        std::string filename_;   //д��־��Ϣ˵�ڵ�Դ�ļ���ʹ�øú������ļ���(���� __FILE__ )
    };

    Impl impl_;

    static std::string log_file_basename_;    //��־�Ļ�������
    ////��ʽ��ʱ�� "%4d%2d%2d %2d:%2d:%2d",�����¼�ͨ��strea_<<д�뵽��־�������ڵĻ�����
    //void formatTime();

    //// ��־�� ���Ϳ���ʹ�� "<<" �����־
    //LogStream stream_;
    //// ��־�ȼ�
    //LogLevel level_;
    //// ʹ�øú������ļ���(���� __FILE__ )
    //const char* filename_;
    //// ����, �����ڵڼ��д������¼����Ϣ
    //int line_;
    //Timestamp time_;
    ////��־�ļ�����
    ////static std::string logFileName_;
    //std::string basename_;
};


//�����ǲ�ͬ����־�ȼ��ͻ��ж�Ӧ���������Щ�ȼ��������Щ�ֶΣ���Щ�ȼ�������ֶλ���Щ
// �궨�壬�ڴ�����include��ͷ�ļ�����ʹ�ã� LOG_INFO << "�������";
#define LOG_TRACE if (Logger::GlobalLogLevel() <= Logger::LogLevel::TRACE) \
  Logger(__FILE__, __LINE__, Logger::LogLevel::TRACE, __func__).Stream()
#define LOG_DEBUG if (Logger::GlobalLogLevel() <= Logger::LogLevel::DEBUG) \
  Logger(__FILE__, __LINE__, Logger::LogLevel::DEBUG, __func__).Stream()
#define LOG_INFO if (Logger::GlobalLogLevel() <= Logger::LogLevel::INFO) \
  Logger(__FILE__, __LINE__).Stream()

#define LOG_WARN Logger(__FILE__, __LINE__, Logger::LogLevel::WARN).Stream()
#define LOG_ERROR Logger(__FILE__, __LINE__, Logger::LogLevel::ERROR).Stream()
#define LOG_FATAL Logger(__FILE__, __LINE__, Logger::LogLevel::FATAL).Stream()
#define LOG_SYSERR Logger(__FILE__, __LINE__, false).Stream()
#define LOG_SYSFATAL Logger(__FILE__, __LINE__, true).Stream()