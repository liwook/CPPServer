#include"logger.h"
#include<thread>
#include<memory>
#include<assert.h>
#include"asynclogger.h"
#include"../currentthread.h"
#include<mutex>
#include<string.h>

static std::unique_ptr<AsyncLogger> asyncLogger;
static std::once_flag g_once_flag;

thread_local char t_time[64];	//当前线程的时间戳字符串的日期和时间
thread_local time_t t_lastSecond;	//当前线程的最新的日志消息的秒数

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


// 默认输出函数，输出至标准输出
void DefaultOutput(const char* msg, int len)
{
	fwrite(msg, 1, len, stdout);
}

// 默认冲刷函数，冲刷标准输出流
void DefaultFlush()
{
	fflush(stdout);
}

//std::string Logger::logFileName_ = "../li22.log";
std::string Logger::log_file_basename_ = "./li22";	//后面会去获取主机名等去合成一个日志文件名

void OnceInit()
{
	asyncLogger = std::make_unique<AsyncLogger>(Logger::LogFileName(),1024*1024*50);//rollsize=100MB
	asyncLogger->start();
}

void AsyncOutput(const char* logline, int len)
{
	std::call_once(g_once_flag, OnceInit);
	printf("AsyncOutput() call_once...\n");
	asyncLogger->Append(logline, len);
}

// 全局变量：输出函数
Logger::OutputFunc g_output = AsyncOutput;
// 全局变量：冲刷函数
Logger::FlushFunc  g_flush = DefaultFlush;

// helper class for known string length at compile time(编译时间）
class T
{
public:
	T(const char* str, unsigned len)
		:str_(str),
		len_(len)
	{
	}

	const char* str_;
	const unsigned len_;
};

inline LogStream& operator<<(LogStream& s, T v)
{
	s.Append(v.str_, v.len_);
	return s;
}

//日志等级字符串数组，用于输出的
const char* g_loglevel_name[static_cast<int>(Logger::LogLevel::NUM_LOG_LEVELS)] =
{
	"TRACE ",
	"DEBUG ",
	"INFO  ",
	"WARN  ",
	"ERROR ",
	"FATAL "
};

//Logger::Logger(const char* basename, int line, Logger::LogLevel level, const char* funcName)
//	:stream_()
//	,level_(level)
//	, basename_(basename)
//	,line_(line)
//	,time_(Timestamp::now())
//{
//	formatTime();	//时间输出
//	CurrentThread::tid();	//更新线程
//	stream_ << T(CurrentThread::tidString(), CurrentThread::tidStringLength());
//	stream_ << T(g_loglevel_name[static_cast<int>(level_)], 6);	//日志等级的字符串是定长的
//
//	stream_ << funcName << "():";
//}
// 
//多个级别，输出就有对应的输出。
Logger::Logger(const char* FileName, int line, LogLevel level, const char* funcName)
	:impl_(level, FileName,line)
{
	impl_.stream_ << funcName << ' ';
}

Logger::Logger(const char* file, int line)
	:impl_(LogLevel::INFO,file,line)
{
}
Logger::Logger(const char* file, int line, LogLevel level)
	:impl_(level,file,line)
{
}
Logger::Logger(const char* file, int line, bool toAbort)
	:impl_(toAbort?LogLevel::FATAL:LogLevel::ERROR,file,line)
{
}

Logger::~Logger()
{
	//stream_ << " - " << basename_ << " : " << line_ << '\n';
	impl_.finish();
	const LogStream::Buffer& buf(Stream().buffer());
	g_output(buf.Data(), buf.Length());
}




Logger::Impl::Impl(LogLevel level, const std::string& file, int line)
	: time_(Timestamp::now())
	, stream_()
	, level_(level)
	, line_(line)
	, filename_(file)
{
	formatTime();	//时间输出
	CurrentThread::tid();	//更新线程
	stream_ << T(CurrentThread::tidString(), CurrentThread::tidStringLength());	//输出线程id
	stream_ << T(g_loglevel_name[static_cast<int>(level_)], 6);	//日志等级的字符串是定长的
}

void Logger::Impl::formatTime()
{
	int64_t microSecondsSinceEpoch = time_.microSecondsSinceEpoch();
	//得到秒数
	time_t seconds = static_cast<time_t>(microSecondsSinceEpoch / Timestamp::kMicroSecondsPerSecond);
	//得到其微妙
	int microseconds = static_cast<time_t>(microSecondsSinceEpoch % Timestamp::kMicroSecondsPerSecond);
	if (seconds != t_lastSecond) {	//秒数不相等，说明也要格式化秒数
		t_lastSecond = seconds;
		//struct tm tm_time;
		//gmtime_r(&seconds, &tm_time);//这是转换为格林尼治标准时间，和北京时间不一样
		struct tm tm_time;
		memset(&tm_time, 0, sizeof(tm_time));
		localtime_r(&seconds,&tm_time);

		int len = snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d",
			tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
			tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
		assert(len == 17);
	}
	char buf[12] = { 0 };
	int lenMicro =sprintf(buf, ".%06d ", microseconds);
	stream_ << T(t_time, 17) << T(buf, lenMicro);
}

void Logger::Impl::finish()
{
	stream_ << " - " << filename_ << ':' << line_ << '\n';
}

// 设置输出函数
void Logger::SetOutput(OutputFunc out)
{
	g_output = out;
}
// 设置冲刷函数
void Logger::SetFlush(FlushFunc flush)
{
	g_flush = flush;
}