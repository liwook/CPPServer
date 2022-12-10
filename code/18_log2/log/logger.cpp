#include"logger.h"
#include<thread>
#include<memory>
#include<assert.h>
#include"asynclogger.h"
#include<mutex>
#include"../timestamp.h"

static std::unique_ptr<AsyncLogger> asyncLogger;
static std::once_flag g_once_flag;

//��ʼ����־�ȼ�
Logger::LogLevel InitLogLevel()
{
	if(getenv("LGG_DEBUF"))
		return Logger::LogLevel::DEBUG;
	else
		return Logger::LogLevel::INFO;
}

//��־�ȼ��ַ������飬���������
const char* g_loglevel_name[static_cast<int>(Logger::LogLevel::NUM_LOG_LEVELS)] =
{
	"DEBUG ",
	"INFO  ",
	"WARN  ",
	"ERROR ",
	"FATAL "
};

//ȫ�ֱ���:��־�ȼ�
Logger::LogLevel g_LogLevel=InitLogLevel();

// ��ȡȫ����־�ȼ������ǵ�ǰ����ĵȼ�
Logger::LogLevel Logger::GlobalLogLevel()
{
	return g_LogLevel;
}


// Ĭ������������������׼���
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

//����������Ļ�������
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
	formatTime();	//ʱ�����
	//�����־�ȼ�
	stream_ <<' '<< g_loglevel_name[static_cast<int>(level_)] << funcName << "():";
}

// ȫ�ֱ������������
Logger::OutputFunc g_output = AsyncOutput;
Logger::~Logger()
{
	stream_ << " - " << filename_ << " : " << line_ << '\n';
	const LogStream::Buffer& buf(Stream().buffer());
	g_output(buf.Data(), buf.Length());	//����ʹ����AsyncOutput
}

void Logger::formatTime()
{
	const char* now_time = Timestamp::now().toFormattedString().c_str();
	stream_ << now_time;
}