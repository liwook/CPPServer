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


	//�����־��Ϣ��file_�Ļ�����
	void Append(const char* str, int len);

	void Flush();	//��ˢfile_�Ļ�����

	bool rollFile();	//������־�ĺ���
private:
	std::string getLogFileName(const std::string& basename, time_t* now);

	//��־�ļ���
	//const std::string filename_;
	const std::string basename_;
	const off_t rollsize_;		//������־�Ĵ�С
	const int flush_interval_seonds_;	//��Ҫ��ˢ��ʱ����
	const int flush_everyN_;	//ÿдflushEveryN_����־��Ϣ��ǿ�Ƴ�ˢfile_�Ļ��������ݵ�������
	int count_;	//����д�˶�������־��Ϣ�����ǵ���Append()�Ĵ���)
	std::unique_ptr<AppendFile> file_;

	time_t start_of_period_;//��־�ļ���ʼд��ʱ��
	time_t last_roll_;		//����һ����־������ʱ��
	time_t last_flush_;		//����һ�γ�ˢ��־��ʱ��
	const static int kRollPerSeconds_ = 60 * 60 * 24;	//һ���е�����
};
