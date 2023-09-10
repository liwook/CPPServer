#include"fileutil.h"
#include<stdio.h>
#include<string.h>
#include"../log/logger.h"

AppendFile::AppendFile(const std::string& file_name)
	:fp_(fopen(file_name.c_str(),"ae"))
	,writtenBytes_(0)
{
	if (fp_ == nullptr) {
		//printf("log file open failed: errno = %d reason = %s \n", errno, strerror(errno));
		LOG_ERROR << "log file open failed: errno =" << errno << " reason = " << strerror(errno);
	}
	else {
		setbuffer(fp_, buffer_, sizeof(buffer_));
	}
}
AppendFile::~AppendFile()
{
	if (fp_)
		fclose(fp_);
}

//真正的添加数据到磁盘文件，调用fwrite()
void AppendFile::Append(const char* logline, size_t len)
{
	//可能数据没法一次完全写入，所以这样做的
	size_t written = 0;
	while (written != len) {
		auto remain = len - written;
		size_t n = fwrite_unlocked(logline+written, 1, remain, fp_);	//这是fwrite()的无锁版本
		if (n != remain) {
			int err = ferror(fp_);
			if (err) {
				fprintf(stderr, "AppendFile::append() failed %s\n", strerror(err));
				break;
			}
		}
		written += n;
	}
	writtenBytes_ += written;

}

//冲刷缓冲区的内容写入到磁盘文件
void AppendFile::Flush()
{
	//功能是冲洗流中的信息，该函数通常用于处理磁盘文件。fflush()会强迫将缓冲区内的数据写回参数stream 指定的文件中。
	fflush(fp_);
}