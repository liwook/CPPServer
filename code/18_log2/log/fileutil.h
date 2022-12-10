#pragma once

#include<string>

//底层控制磁盘文件输出的类
class AppendFile
{
public:
	explicit AppendFile(const std::string& file_name);
	~AppendFile();

	//真正的添加数据到磁盘文件，调用fwrite()
	void Append(const char* str, size_t len);

	//冲刷缓冲区的内容写入到磁盘文件
	void Flush();

	off_t writtenBytes() const { return writtenBytes_; }

private:
	//文件描述符
	FILE* fp_;
	//给fp_使用的缓冲区
	char buffer_[1024 * 64];
	//往磁盘文件中已写入的字节大小
	off_t writtenBytes_;

};