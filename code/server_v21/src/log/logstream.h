#pragma once

#include<string>
#include<string.h>

//��������С
const int KSmallBuffer = 1024;
const int KLargeBuffer = 1024 * 1000;

//������ģ���࣬ӭ�������־����
//��СΪ���SIZE��char���͵�����
template<int SIZE>
class FixedBuffer
{
public:
	FixedBuffer() :cur_(data_) {}
	~FixedBuffer(){}
	//�����������������
	void  Append(const char* buf, size_t len)
	{
		if (Available() > static_cast<int>(len)) {
			memcpy(cur_, buf, len);
			AppendComplete(len);
		}
	}
	//��������ݺ󣬸���curָ���λ��
	void AppendComplete(size_t len) { cur_ += len; }
	//�������
	void Menset() { memset(data_, 0, sizeof(data_)); }
	//��������
	void Reset() { cur_ = data_; }
	//��ȡ����
	const char* Data()const { return data_; }

	int Length()const { return static_cast<int>(cur_ - data_); }
	//��ȡ��ǰ����β��ָ�룬���ǻ�ȡcur_
	char* Current() { return cur_; }
	//��ȡʣ��Ŀ��ÿռ��С
	int Available()const { return static_cast<int>(End() - cur_); }
private:
	//��ȡĩβָ��
	const char* End()const { return data_ + sizeof(data_); }

	//ģ��
	// data_                cur_
	// 	 |   (�Ѵ�ŵ�����)  | ʣ��Ļ��ɴ�ŵĴ�С
	// 	 ��������������������������������������������
	//ʵ�ʴ�ŵ�����
	char data_[SIZE];

	//��ǰ����β��ָ��
	char* cur_;
};


//��־����
class LogStream
{
public:
	//�̶���С�Ļ�����
	using Buffer = FixedBuffer<KSmallBuffer> ;
	// ���������������
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

	//�����������������
	void Append(const char* data, int len) { buffer_.Append(data, len); }

	//��ȡ�������Ķ���
	const Buffer& buffer()const { return buffer_; }
private:
	// ��ʽ����������Ϊ�ַ����������������
	template<class T>
	void FormatInteger(T);

	//������ �Լ������Bufferģ����
	Buffer buffer_;

	//����תΪ�ַ��������ռ�õ�����ֽ���
	static const int KMaxNumbericSize = 48;
};


