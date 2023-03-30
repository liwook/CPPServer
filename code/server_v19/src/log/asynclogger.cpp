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
		//当前缓冲区有足够的空间，就直接往缓冲区中添加
		currentBuffer_->Append(logline, len);
	}
	else {
		// 把已满的缓冲区放入预备给后端log线程的容器中
		buffers_.emplace_back(std::move(currentBuffer_));
		currentBuffer_.reset();		// 将cur_buffer_的指针置为空

		if (nextBuffer_) {			 // 若下一个可用缓冲区不为空，则move给cur
			currentBuffer_ = std::move(nextBuffer_);
		}
		else {	//否则就新建一个
			currentBuffer_.reset(new Buffer);
		}

		currentBuffer_->Append(logline, len);
		cond_.notify_one();			// 这时提醒后端log线程可以读取缓冲区了
	}
}

//后端日志线程函数
void AsyncLogger::ThreadFunc()
{
	printf("AsyncLogger::ThreadFunc()\n");
	//打开磁盘日志文件,即是创建LogFile对象
	LogFile output(basename_, rollsize_);
	
	//准备好后端备用的缓冲区1、2
	auto newBuffer1 = std::make_unique<Buffer>();
	auto newBuffer2 = std::make_unique<Buffer>();
	//auto newBuffer1(new Buffer);
	//auto newBuffer2(new Buffer);
	newBuffer1->Menset();
	newBuffer2->Menset();

	//备好读取的缓冲区vector
	BufferVector buffersToWrite;
	buffersToWrite.reserve(16);	//预留16个元素空间

	while (is_running_) {
		{
			std::unique_lock<std::mutex> lock(mutex_);
			if (buffers_.empty()) {		//注意这里是用 if ,因为够这么多秒时间后会唤醒的
				cond_.wait_for(lock, std::chrono::seconds(flushInterval_));
			}

			//把currentBuffer_放入到容器中
			buffers_.push_back(std::move(currentBuffer_));
			currentBuffer_.reset();	//已放入到容器中，所以需要置空
			// 将后端备好的log线程的缓冲区1交给cur
			currentBuffer_ = std::move(newBuffer1);

			buffersToWrite.swap(buffers_); // 把buffers跟后端log线程空的容器交换

			 // 如果下一个可用缓冲区nextBuffer_为空，就把后端log线程的缓冲区2交给next
			if (!nextBuffer_) {
				nextBuffer_ = std::move(newBuffer2);
			}
		}	//这里出了锁的作用域

		 // 如果缓冲区过多，说明前端产生log的速度远大于后端消费的速度，这里只是简单的将它们丢弃
		if (buffersToWrite.size() > 25){
			buffersToWrite.erase(buffersToWrite.begin() + 2, buffersToWrite.end());
		}

		// 将缓冲区内容写入文件
		for (size_t i = 0; i < buffersToWrite.size(); ++i){
			output.Append(buffersToWrite[i]->Data(), buffersToWrite[i]->Length());
		}

		// 将过多的缓冲区丢弃
		if (buffersToWrite.size() > 2){
			buffersToWrite.resize(2);
		}

		// 恢复后端备用缓冲区
		if (!newBuffer1){
			newBuffer1 = std::move(buffersToWrite.back());
			buffersToWrite.pop_back();
			// 将缓冲区的数据指针归零
			newBuffer1->Reset();
		}
		if (!newBuffer2){
			newBuffer2 = std::move(buffersToWrite.back());
			buffersToWrite.pop_back();
			newBuffer2->Reset();
		}

		// 丢弃无用的缓冲区
		buffersToWrite.clear();
		output.Flush();
	}
	output.Flush();
}