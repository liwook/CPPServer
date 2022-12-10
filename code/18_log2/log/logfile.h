#pragma once
#include<memory>
#include<string>
#include"fileutil.h"
//class AppendFile;

class LogFile
{
public:
	LogFile(const std::string& file_name, int flushEveryN=1024);
	//添加日志消息到file_的缓冲区
	void Append(const char* str, int len);

	void Flush();	//冲刷file_的缓冲区
private:
	//日志文件名
	const std::string filename_;
	const int flushEveryN_;	//每写flushEveryN_条日志消息就强制冲刷file_的缓冲区内容到磁盘中
	int count_;	//计算写了多少条日志消息（即是调用Append()的次数)
	std::unique_ptr<AppendFile> file_;
};
