#include"logger.h"
#include<thread>
#include<memory>
#include<assert.h>
#include"../timestamp.h"

//初始化日志等级
Logger::LogLevel InitLogLevel()
{
	if(getenv("LGG_DEBUF"))
		return Logger::LogLevel::DEBUG;
	else
		return Logger::LogLevel::INFO;
}

//全局变量:日志等级
Logger::LogLevel g_LogLevel=InitLogLevel();

// 获取全局日志等级，并非当前对象的等级
Logger::LogLevel Logger::GlobalLogLevel()
{
	return g_LogLevel;
}

//日志等级字符串数组
const char* g_loglevel_name[static_cast<int>(Logger::LogLevel::NUM_LOG_LEVELS)] =
{
	"DEBUG ",
	"INFO  ",
	"WARN  ",
	"ERROR ",
	"FATAL "
};

Logger::Logger(const char* FileName, int line, Logger::LogLevel level, const char* funcName)
	:stream_()
	,level_(level)
	,filename_(FileName)
	,line_(line)
{
	formatTime();	
	//输出日志等级
	stream_ <<" "<< g_loglevel_name[static_cast<int>(level_)] << funcName << "():";
}
Logger::~Logger()
{
	stream_ << " - " << filename_ << " : " << line_ << '\n';
	const LogStream::Buffer& buf(Stream().buffer());
	DefaultOutput(buf.Data(), buf.Length());
}

void Logger::formatTime()
{
	const char* now_time= Timestamp::now().toFormattedString().c_str();
	stream_ << now_time;
}