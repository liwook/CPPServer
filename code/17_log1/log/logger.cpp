#include"logger.h"
#include<thread>
#include<memory>
#include<assert.h>
#include"../timestamp.h"

//��ʼ����־�ȼ�
Logger::LogLevel InitLogLevel()
{
	if(getenv("LGG_DEBUF"))
		return Logger::LogLevel::DEBUG;
	else
		return Logger::LogLevel::INFO;
}

//ȫ�ֱ���:��־�ȼ�
Logger::LogLevel g_LogLevel=InitLogLevel();

// ��ȡȫ����־�ȼ������ǵ�ǰ����ĵȼ�
Logger::LogLevel Logger::GlobalLogLevel()
{
	return g_LogLevel;
}

//��־�ȼ��ַ�������
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
	//�����־�ȼ�
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