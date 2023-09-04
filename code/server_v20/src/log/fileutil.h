#pragma once

#include<string>

//�ײ���ƴ����ļ��������
class AppendFile
{
public:
	explicit AppendFile(const std::string& file_name);
	~AppendFile();

	//������������ݵ������ļ�������fwrite()
	void Append(const char* str, size_t len);

	//��ˢ������������д�뵽�����ļ�
	void Flush();

	off_t writtenBytes() const { return writtenBytes_; }

private:
	//�ļ�������
	FILE* fp_;
	//��fp_ʹ�õĻ�����
	char buffer_[1024 * 64];
	//�������ļ�����д����ֽڴ�С
	off_t writtenBytes_;

};