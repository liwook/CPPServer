#pragma once
#include<memory>
#include<string>
#include"fileutil.h"
//class AppendFile;

class LogFile
{
public:
	LogFile(const std::string& file_name, int flushEveryN=1024);
	//�����־��Ϣ��file_�Ļ�����
	void Append(const char* str, int len);

	void Flush();	//��ˢfile_�Ļ�����
private:
	//��־�ļ���
	const std::string filename_;
	const int flushEveryN_;	//ÿдflushEveryN_����־��Ϣ��ǿ�Ƴ�ˢfile_�Ļ��������ݵ�������
	int count_;	//����д�˶�������־��Ϣ�����ǵ���Append()�Ĵ���)
	std::unique_ptr<AppendFile> file_;
};
