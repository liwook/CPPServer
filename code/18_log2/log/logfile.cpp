#include"logfile.h"
//#include"fileutil.h"

LogFile::LogFile(const std::string& filename, int flushEveryN)
	:filename_(filename)
	,flushEveryN_(flushEveryN)
	,count_(0)
	,file_(std::make_unique<AppendFile>(filename_))
{
}
//�����־��Ϣ��file_�Ļ�����
void LogFile::Append(const char* logline, int len)
{
	file_->Append(logline,len);
	if (++count_ >= flushEveryN_) {
		count_ = 0;
		file_->Flush();
	}
}

void LogFile::Flush()
{
	file_->Flush();
}