#pragma once

#include"logstream.h"

class Logger
{
public:
	//��־����
	enum class LogLevel
	{
		DEBUG,	//����ʹ��
		INFO,	//��Ϣ
		WARN,	//����
		ERROR,	//����
		FATAL,	//����
		NUM_LOG_LEVELS,
	};

	Logger(const char* FileName,int line,LogLevel level,const char* funcName);
	~Logger();

    // ��ȡ��־������
    LogStream& Stream() { return stream_; }
    // ��ȡȫ����־�ȼ������ǵ�ǰ����ĵȼ�
    static LogLevel GlobalLogLevel();
    static std::string LogFileName() { return logFileName_; }

    using OutputFunc=void (*)(const char* msg, int len);

private:
    //��ʽ��ʱ�� "%4d%2d%2d %2d:%2d:%2d",����ʱ��ͨ��strea_<<д�뵽��־�������ڵĻ�����
    void formatTime();

    // ��־�� ���Ϳ���ʹ�� "<<" �����־
    LogStream stream_;
    // ��־�ȼ�
    LogLevel level_;
    // ʹ�øú������ļ���(���� __FILE__ )
    const char* filename_;
    // ����, �����ڵڼ��д������¼����Ϣ
    int line_;
    //��־�ļ�����
    static std::string logFileName_;
};

// �궨�壬�ڴ�����include��ͷ�ļ�����ʹ�ã� LOG_INFO << "�������";
#define LOG_DEBUG if (Logger::GlobalLogLevel() == Logger::LogLevel::DEBUG) Logger(__FILE__, __LINE__, Logger::LogLevel::DEBUG, __func__).Stream()
#define LOG_INFO  if (Logger::GlobalLogLevel() <= Logger::LogLevel::INFO)  Logger(__FILE__, __LINE__, Logger::LogLevel::INFO, __func__).Stream()
#define LOG_WARN  Logger(__FILE__, __LINE__, Logger::LogLevel::WARN, __func__).Stream()
#define LOG_ERROR Logger(__FILE__, __LINE__, Logger::LogLevel::ERROR, __func__).Stream()
#define LOG_FATAL Logger(__FILE__, __LINE__, Logger::LogLevel::FATAL, __func__).Stream()

