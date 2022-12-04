#pragma once

#include<vector>
#include<string>
#include <assert.h>


/// @code
/// +-------------------+------------------+------------------+
/// | prependable bytes |  readable bytes  |  writable bytes  |
/// |                   |     (CONTENT)    |                  |
/// +-------------------+------------------+------------------+
/// |                   |                  |                  |
/// 0      <=      readerIndex   <=   writerIndex    <=     size
/// @endcode
class Buffer
{
public:
	static const size_t KCheapPrepend = 8;	//数据包长度8字节
	static const size_t KInitailSize = 1024;//缓冲区初始的大小
	explicit Buffer(size_t initialSize=KInitailSize)
		:buffer_(KCheapPrepend +initialSize)
		,readerIndex_(KCheapPrepend)
		,writerIndex_(KCheapPrepend)
	{}

	//待读取的数据大小
	const size_t readableBytes()const {
		return writerIndex_ - readerIndex_;
	}

	//可写的空闲空间大小
	const size_t writeableBytes()const {
		return buffer_.size() - writerIndex_;
	}

	const size_t prependableBytes()const {
		return readerIndex_;
	}

	//缓冲区中可读取的起始位置
	const char* peek()const {
		return begin() + readerIndex_;
	}

	//读取了数据后，更新readerIndex_
	void retrieve(size_t len) {
		if (len < readableBytes()) {
			readerIndex_ += len;
		}
		else {
			retrieveAll();
		}
	}

	void retrieveAll()
	{
		readerIndex_ = KCheapPrepend;
		writerIndex_ = KCheapPrepend;
	}

	std::string retrieveAsString(size_t len) {
		assert(len <= readableBytes());
		std::string result(peek(), len);
		retrieve(len);
		return result;
	}

	std::string retrieveAllAsString() {
		return retrieveAsString(readableBytes());
	}

	void append(const char* data, size_t len) {
		if (writeableBytes() < len) {
			makeSpace(len);	//扩容
		}
		std::copy(data, data + len, beginWirte());
		writerIndex_ += len;
	}

	char* beginWirte() {
		return begin() + writerIndex_;
	}
	const char* beginWirte()const {
		return begin() + writerIndex_;
	}

	ssize_t readFd(int fd, int* saveErrno);
	ssize_t writeFd(int fd, int* saveErrno);

private:
	//返回Buffer底层的数据首地址
	char* begin() {
		return &*buffer_.begin();
	}
	//常对象只能调用常方法，不能调用普通的，而这里是一定要使用const char*,用char*会出现 "错误：从“const char*”到“char*”的无效转换[-fpermissive]"
	const char* begin()const {
		return &*buffer_.begin();
	}

	void makeSpace(size_t len) {
		if (writeableBytes() + prependableBytes() < len + KCheapPrepend) {
			buffer_.resize(writerIndex_ + len);
		}
		else {
			auto readable = readableBytes();

			//挪动
			std::copy(begin() + readerIndex_, begin() + writerIndex_, begin() + KCheapPrepend);
			//更新readerIndex_， writerIndex_
			readerIndex_ = KCheapPrepend;
			writerIndex_ = readerIndex_ + readable;
		}
	}
private:
	std::vector<char> buffer_;
	size_t readerIndex_;
	size_t writerIndex_;

};