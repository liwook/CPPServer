#pragma once
#include<memory>
#include<string>
#include"fileutil.h"
#include<time.h>
//class AppendFile;

class LogFile
{
public:
	//LogFile(const std::string& file_name, int flushEveryN=1024);
	LogFile(const std::string& basename,off_t rollSize,int flushInterval_seconds =3, int flushEveryN = 1024);


	//添加日志消息到file_的缓冲区
	void Append(const char* str, int len);

	void Flush();	//冲刷file_的缓冲区

	bool rollFile();	//滚动日志的函数
private:
	std::string getLogFileName(const std::string& basename, time_t* now);

	//日志文件名
	//const std::string filename_;
	const std::string basename_;
	const off_t rollsize_;		//滚动日志的大小
	const int flush_interval_seonds_;	//需要冲刷的时间间隔
	const int flush_everyN_;	//每写flushEveryN_条日志消息就强制冲刷file_的缓冲区内容到磁盘中
	int count_;	//计算写了多少条日志消息（即是调用Append()的次数)
	std::unique_ptr<AppendFile> file_;

	time_t start_of_period_;//日志文件开始写的时间
	time_t last_roll_;		//最新一次日志滚动的时间
	time_t last_flush_;		//最新一次冲刷日志的时间
	const static int kRollPerSeconds_ = 60 * 60 * 24;	//一天中的秒数
};
