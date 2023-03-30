#pragma once

#include"logstream.h"
#include"../timestamp.h"
class Logger
{
public:
	//日志级别
	enum class LogLevel
	{
        TRACE,
		DEBUG,	//调试使用
		INFO,	//信息
		WARN,	//警告
		ERROR,	//错误
		FATAL,	//致命
		NUM_LOG_LEVELS,
	};

    //有多中级别，输出就有对应的输出。
	Logger(const char* FileName,int line,LogLevel level,const char* funcName);
    Logger(const char* file, int line);
    Logger(const char* file, int line, LogLevel level);
    Logger(const char* file, int line, bool toAbort);

	~Logger();

    // 获取日志流对象
    //LogStream& Stream() { return stream_; }
    LogStream& Stream() { return impl_.stream_; }
    // 
    // 获取全局日志等级，并非当前对象的等级
    static LogLevel GlobalLogLevel();
    static std::string LogFileName() { return log_file_basename_; }
    using OutputFunc=void (*)(const char* msg, int len);
    using FlushFunc=void (*)();
    // 设置输出函数
    static void SetOutput(OutputFunc);
    // 设置冲刷函数
    static void SetFlush(FlushFunc);

private:
    //为什么需要这个Impl类？？？ 因为不同的日志等级就会输出对应不同的字段个数。
    //那么Logger的构造函数就会有多个，构造函数如上面的所示。
    //那么每个构造函数也需要这样写
    //Logger::Logger(const char* basename, int line, Logger::LogLevel level, const char* funcName)
    //    :stream_()
    //    , level_(level)
    //    , basename_(basename)
    //    , line_(line)
    //    , time_(Timestamp::now())
    //{
    //    formatTime();	//时间输出
    //    CurrentThread::tid();	//更新线程
    //    stream_ << T(CurrentThread::tidString(), CurrentThread::tidStringLength());
    //    stream_ << T(g_loglevel_name[static_cast<int>(level_)], 6);	//日志等级的字符串是定长的

    //    stream_ << funcName << "():";
    //}
    //这样就是超级繁琐了啦，所以现在添加一个Impl类，让Impl类的构造函数做了这些工作，之后只需在Logger::Logger()的构造函数中构造Impl类就行，
    //这样就超级轻松的啦。
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
        int line_;          //行数 ，即是__LINE__
        std::string filename_;   //写日志消息说在的源文件，使用该函数的文件名(即是 __FILE__ )
    };

    Impl impl_;

    static std::string log_file_basename_;    //日志的基础名字
    ////格式化时间 "%4d%2d%2d %2d:%2d:%2d",并把事件通过strea_<<写入到日志流对象内的缓冲区
    //void formatTime();

    //// 日志流 ，就可以使用 "<<" 输出日志
    //LogStream stream_;
    //// 日志等级
    //LogLevel level_;
    //// 使用该函数的文件名(即是 __FILE__ )
    //const char* filename_;
    //// 行数, 表明在第几行代码出记录的消息
    //int line_;
    //Timestamp time_;
    ////日志文件名字
    ////static std::string logFileName_;
    //std::string basename_;
};


//这里是不同的日志等级就会有对应的输出，有些等级会输出多些字段，有些等级的输出字段会少些
// 宏定义，在代码中include该头文件即可使用： LOG_INFO << "输出数据";
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