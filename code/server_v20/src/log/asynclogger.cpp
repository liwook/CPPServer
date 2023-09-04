#include"asynclogger.h"
#include"logfile.h"
#include<stdio.h>
#include<functional>
#include<chrono>
#include<unistd.h>

//AsyncLogger::AsyncLogger(const std::string fileName, int flushInterval)
AsyncLogger::AsyncLogger(const std::string basename, off_t rollSize, int flushInterval)
	:flushInterval_(flushInterval)
	,is_running_(false)
	,basename_(basename)
	,rollsize_(rollSize)
	,currentBuffer_(std::make_unique<Buffer>())
	,nextBuffer_(std::make_unique<Buffer>())
{
	currentBuffer_->Menset();
	nextBuffer_->Menset();
	buffers_.reserve(16);
}


void AsyncLogger::Append(const char* logline, int len)
{
	std::unique_lock<std::mutex> lock(mutex_);
	
	if (currentBuffer_->Available() > len) {
		//��ǰ���������㹻�Ŀռ䣬��ֱ���������������
		currentBuffer_->Append(logline, len);
	}
	else {
		// �������Ļ���������Ԥ�������log�̵߳�������
		buffers_.emplace_back(std::move(currentBuffer_));
		currentBuffer_.reset();		// ��cur_buffer_��ָ����Ϊ��

		if (nextBuffer_) {			 // ����һ�����û�������Ϊ�գ���move��cur
			currentBuffer_ = std::move(nextBuffer_);
		}
		else {	//������½�һ��
			currentBuffer_.reset(new Buffer);
		}

		currentBuffer_->Append(logline, len);
		cond_.notify_one();			// ��ʱ���Ѻ��log�߳̿��Զ�ȡ��������
	}
}

//�����־�̺߳���
void AsyncLogger::ThreadFunc()
{
	//printf("AsyncLogger::ThreadFunc()\n");
	//�򿪴�����־�ļ�,���Ǵ���LogFile����
	LogFile output(basename_, rollsize_);
	
	//׼���ú�˱��õĻ�����1��2
	auto newBuffer1 = std::make_unique<Buffer>();
	auto newBuffer2 = std::make_unique<Buffer>();
	//auto newBuffer1(new Buffer);
	//auto newBuffer2(new Buffer);
	newBuffer1->Menset();
	newBuffer2->Menset();

	//���ö�ȡ�Ļ�����vector
	BufferVector buffersToWrite;
	buffersToWrite.reserve(16);	//Ԥ��16��Ԫ�ؿռ�

	while (is_running_) {
		{
			std::unique_lock<std::mutex> lock(mutex_);
			if (buffers_.empty()) {		//ע���������� if ,��Ϊ����ô����ʱ���ỽ�ѵ�
				cond_.wait_for(lock, std::chrono::seconds(flushInterval_));
			}

			//��currentBuffer_���뵽������
			buffers_.push_back(std::move(currentBuffer_));
			currentBuffer_.reset();	//�ѷ��뵽�����У�������Ҫ�ÿ�
			// ����˱��õ�log�̵߳Ļ�����1����cur
			currentBuffer_ = std::move(newBuffer1);

			buffersToWrite.swap(buffers_); // ��buffers�����log�߳̿յ���������

			 // �����һ�����û�����nextBuffer_Ϊ�գ��ͰѺ��log�̵߳Ļ�����2����next
			if (!nextBuffer_) {
				nextBuffer_ = std::move(newBuffer2);
			}
		}	//�����������������

		 // ������������࣬˵��ǰ�˲���log���ٶ�Զ���ں�����ѵ��ٶȣ�����ֻ�Ǽ򵥵Ľ����Ƕ���
		if (buffersToWrite.size() > 25){
			buffersToWrite.erase(buffersToWrite.begin() + 2, buffersToWrite.end());
		}

		// ������������д���ļ�
		for (size_t i = 0; i < buffersToWrite.size(); ++i){
			output.Append(buffersToWrite[i]->Data(), buffersToWrite[i]->Length());
		}

		// ������Ļ���������
		if (buffersToWrite.size() > 2){
			buffersToWrite.resize(2);
		}

		// �ָ���˱��û�����
		if (!newBuffer1){
			newBuffer1 = std::move(buffersToWrite.back());
			buffersToWrite.pop_back();
			// ��������������ָ�����
			newBuffer1->Reset();
		}
		if (!newBuffer2){
			newBuffer2 = std::move(buffersToWrite.back());
			buffersToWrite.pop_back();
			newBuffer2->Reset();
		}

		// �������õĻ�����
		buffersToWrite.clear();
		output.Flush();
	}
	output.Flush();
}