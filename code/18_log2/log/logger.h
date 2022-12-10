#pragma once

#include"logstream.h"

class Logger
{
public:
	//日志级别
	enum class LogLevel
	{
		DEBUG,	//调试使用
		INFO,	//信息
		WARN,	//警告
		ERROR,	//错误
		FATAL,	//致命
		NUM_LOG_LEVELS,
	};

	Logger(const char* FileName,int line,LogLevel level,const char* funcName);
	~Logger();

    // 获取日志流对象
    LogStream& Stream() { return stream_; }
    // 获取全局日志等级，并非当前对象的等级
    static LogLevel GlobalLogLevel();
    static std::string LogFileName() { return logFileName_; }

    using OutputFunc=void (*)(const char* msg, int len);

private:
    //格式化时间 "%4d%2d%2d %2d:%2d:%2d",并把时间通过strea_<<写入到日志流对象内的缓冲区
    void formatTime();

    // 日志流 ，就可以使用 "<<" 输出日志
    LogStream stream_;
    // 日志等级
    LogLevel level_;
    // 使用该函数的文件名(即是 __FILE__ )
    const char* filename_;
    // 行数, 表明在第几行代码出记录的消息
    int line_;
    //日志文件名字
    static std::string logFileName_;
};

// 宏定义，在代码中include该头文件即可使用： LOG_INFO << "输出数据";
#define LOG_DEBUG if (Logger::GlobalLogLevel() == Logger::LogLevel::DEBUG) Logger(__FILE__, __LINE__, Logger::LogLevel::DEBUG, __func__).Stream()
#define LOG_INFO  if (Logger::GlobalLogLevel() <= Logger::LogLevel::INFO)  Logger(__FILE__, __LINE__, Logger::LogLevel::INFO, __func__).Stream()
#define LOG_WARN  Logger(__FILE__, __LINE__, Logger::LogLevel::WARN, __func__).Stream()
#define LOG_ERROR Logger(__FILE__, __LINE__, Logger::LogLevel::ERROR, __func__).Stream()
#define LOG_FATAL Logger(__FILE__, __LINE__, Logger::LogLevel::FATAL, __func__).Stream()

