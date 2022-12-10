#include"logger.h"
#include<thread>
#include<memory>
#include<assert.h>
#include"asynclogger.h"
#include<mutex>
#include"../timestamp.h"

static std::unique_ptr<AsyncLogger> asyncLogger;
static std::once_flag g_once_flag;

//初始化日志等级
Logger::LogLevel InitLogLevel()
{
	if(getenv("LGG_DEBUF"))
		return Logger::LogLevel::DEBUG;
	else
		return Logger::LogLevel::INFO;
}

//日志等级字符串数组，用于输出的
const char* g_loglevel_name[static_cast<int>(Logger::LogLevel::NUM_LOG_LEVELS)] =
{
	"DEBUG ",
	"INFO  ",
	"WARN  ",
	"ERROR ",
	"FATAL "
};

//全局变量:日志等级
Logger::LogLevel g_LogLevel=InitLogLevel();

// 获取全局日志等级，并非当前对象的等级
Logger::LogLevel Logger::GlobalLogLevel()
{
	return g_LogLevel;
}


// 默认输出函数，输出至标准输出
void DefaultOutput(const char* msg, int len)
{
	fwrite(msg, 1, len, stdout);
}


std::string Logger::logFileName_ = "./li22.log";
void OnceInit()
{
	asyncLogger = std::make_unique<AsyncLogger>(Logger::LogFileName());
	asyncLogger->start();
}

//输出到给定的缓冲区中
void AsyncOutput(const char* logline, int len)
{
	std::call_once(g_once_flag, OnceInit);
	asyncLogger->Append(logline, len);
}



Logger::Logger(const char* FileName, int line, Logger::LogLevel level, const char* funcName)
	:stream_()
	,level_(level)
	,filename_(FileName)
	,line_(line)
{
	formatTime();	//时间输出
	//输出日志等级
	stream_ <<' '<< g_loglevel_name[static_cast<int>(level_)] << funcName << "():";
}

// 全局变量：输出函数
Logger::OutputFunc g_output = AsyncOutput;
Logger::~Logger()
{
	stream_ << " - " << filename_ << " : " << line_ << '\n';
	const LogStream::Buffer& buf(Stream().buffer());
	g_output(buf.Data(), buf.Length());	//就是使用了AsyncOutput
}

void Logger::formatTime()
{
	const char* now_time = Timestamp::now().toFormattedString().c_str();
	stream_ << now_time;
}