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
	static const size_t KCheapPprepend = 8;	//���ݰ�����8�ֽ�
	static const size_t KInitailSize = 1024;//��������ʼ�Ĵ�С
	explicit Buffer(size_t initialSize=KInitailSize)
		:buffer_(KCheapPprepend+initialSize)
		,readerIndex_(KCheapPprepend)
		,writerIndex_(KCheapPprepend)
	{}

	//����ȡ�����ݴ�С
	const size_t readableBytes()const {
		return writerIndex_ - readerIndex_;
	}

	//��д�Ŀ��пռ��С
	const size_t writeableBytes()const {
		return buffer_.size() - writerIndex_;
	}

	const size_t prependableBytes()const {
		return readerIndex_;
	}

	//�������пɶ�ȡ����ʼλ��
	const char* peek()const {
		return begin() + readerIndex_;
	}

	//��ȡ�����ݺ󣬸���readerIndex_
	void retrieve(size_t len) {
		if (len < readableBytes()) {
			readerIndex_ += len;
		}
		else {
			readerIndex_ = KCheapPprepend;
			writerIndex_ = KCheapPprepend;
		}
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
			makeSpace(len);	//����
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
	//����Buffer�ײ�������׵�ַ
	char* begin() {
		return &*buffer_.begin();
	}
	//������ֻ�ܵ��ó����������ܵ�����ͨ�ģ���������һ��Ҫʹ��const char*,��char*����� "���󣺴ӡ�const char*������char*������Чת��[-fpermissive]"
	const char* begin()const {
		return &*buffer_.begin();
	}

	void makeSpace(size_t len) {
		if (writeableBytes() + prependableBytes() < len + KCheapPprepend) {
			buffer_.resize(writerIndex_ + len);
		}
		else {
			auto readable = readableBytes();

			//Ų��
			std::copy(begin() + readerIndex_, begin() + writerIndex_, begin() + KCheapPprepend);
			//����readerIndex_�� writerIndex_
			readerIndex_ = KCheapPprepend;
			writerIndex_ = readerIndex_ + readable;
		}
	}
private:
	std::vector<char> buffer_;
	size_t readerIndex_;
	size_t writerIndex_;

};