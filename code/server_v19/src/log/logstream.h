#pragma once

#include<string>
#include<string.h>

//缓冲区大小
const int KSmallBuffer = 1024;
const int KLargeBuffer = 1024 * 1000;

//缓冲区模板类，迎来存放日志数据
//大小为存放SIZE个char类型的数组
template<int SIZE>
class FixedBuffer
{
public:
	FixedBuffer() :cur_(data_) {}
	~FixedBuffer(){}
	//往缓冲区中添加数据
	void  Append(const char* buf, size_t len)
	{
		if (Available() > static_cast<int>(len)) {
			memcpy(cur_, buf, len);
			AppendComplete(len);
		}
	}
	//添加完数据后，更新cur指针的位置
	void AppendComplete(size_t len) { cur_ += len; }
	//清空数据
	void Menset() { memset(data_, 0, sizeof(data_)); }
	//重置数据
	void Reset() { cur_ = data_; }
	//获取数据
	const char* Data()const { return data_; }

	int Length()const { return static_cast<int>(cur_ - data_); }
	//获取当前数据尾的指针，即是获取cur_
	char* Current() { return cur_; }
	//获取剩余的可用空间大小
	int Available()const { return static_cast<int>(End() - cur_); }
private:
	//获取末尾指针
	const char* End()const { return data_ + sizeof(data_); }

	//模型
	// data_                cur_
	// 	 |   (已存放的数据)  | 剩余的还可存放的大小
	// 	 ——————————————————————
	//实际存放的数据
	char data_[SIZE];

	//当前数据尾的指针
	char* cur_;
};


//日志流类
class LogStream
{
public:
	//固定大小的缓冲区
	using Buffer = FixedBuffer<KSmallBuffer> ;
	// 重载流输出操作符
	LogStream& operator<<(bool v)
	{
		buffer_.Append(v ? "1" : "0", 1);
		return *this;
	}
	LogStream& operator<<(short);
	LogStream& operator<<(int);
	LogStream& operator<<(long);
	LogStream& operator<<(long long);
	LogStream& operator<<(float);
	LogStream& operator<<(double);
	LogStream& operator<<(char);
	LogStream& operator<<(const char*);
	LogStream& operator<<(const std::string&);

	//把数据输出到缓冲区
	void Append(const char* data, int len) { buffer_.Append(data, len); }

	//获取缓冲区的对象
	const Buffer& buffer()const { return buffer_; }
private:
	// 格式化数字类型为字符串并输出到缓冲区
	template<class T>
	void FormatInteger(T);

	//缓冲区 自己定义的Buffer模板类
	Buffer buffer_;

	//数字转为字符串后可以占用的最大字节数
	static const int KMaxNumbericSize = 48;
};


