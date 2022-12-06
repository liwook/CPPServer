#include"fileutil.h"
#include<stdio.h>
#include<string.h>
AppendFile::AppendFile(const std::string& file_name)
	:fp_(fopen(file_name.c_str(),"ae"))
	,writtenBytes_(0)
{
	if (fp_ == nullptr) {
		printf("log file open failed: errno = %d reason = %s \n", errno, strerror(errno));
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

//������������ݵ������ļ�������fwrite()
void AppendFile::Append(const char* logline, size_t len)
{
	//��������û��һ����ȫд�룬������������
	size_t written = 0;
	while (written != len) {
		auto remain = len - written;
		size_t n = fwrite_unlocked(logline+written, 1, remain, fp_);	//����fwrite()�������汾
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

//��ˢ������������д�뵽�����ļ�
void AppendFile::Flush()
{
	//�����ǳ�ϴ���е���Ϣ���ú���ͨ�����ڴ�������ļ���fflush()��ǿ�Ƚ��������ڵ�����д�ز���stream ָ�����ļ��С�
	fflush(fp_);
}