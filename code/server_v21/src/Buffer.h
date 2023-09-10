#pragma once

#include<string>
#include <assert.h>
#include<algorithm>	//Ϊ��ʹ��std::search()
#include<vector>
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
	static const size_t KCheapPrepend = 8;	//���ݰ�����8�ֽ�
	static const size_t KInitailSize = 1024;//��������ʼ�Ĵ�С
	explicit Buffer(size_t initialSize=KInitailSize)
		:buffer_(KCheapPrepend +initialSize)
		,readerIndex_(KCheapPrepend)
		,writerIndex_(KCheapPrepend)
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
			retrieveAll();
		}
	}


	//Ϊ�˽���http��������һ�к󣬷�����Ž�����һ��
	void retrieveUntil(const char* end) {
		assert(peek() <= end);
		assert(end <= beginWirte());
		retrieve(end - peek());
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
	void append(const std::string& data) {
		append(data.data(), data.size());
	}

	void append(const char* data, size_t len) {
		if (writeableBytes() < len) {
			makeSpace(len);	//����
		}
		std::copy(data, data + len, beginWirte());
		writerIndex_ += len;
	}

	//Ϊ����websocket������ʹ�ã�һ���ֽ�һ���ֽ��������
	void append(const char data, size_t allLen) {
		if (writeableBytes() < allLen) {
			makeSpace(allLen);	//����
		}
		buffer_[writerIndex_++] = data;
	}

	//void appendInReaderIndex(const char* data, size_t len)
	//{
	//	if (readerIndex_ > KCheapPrepend + 14)
	//		return;
	//	std::copy(data, data + len, begin() + readerIndex_);
	//	readerIndex_ += len;
	//}
	//void setWriterIndex(int n) { writerIndex_ = n + KCheapPrepend; }
	//void setReaderIndex(int n) { readerIndex_ = n + KCheapPrepend; }



	char* beginWirte() {
		return begin() + writerIndex_;
	}
	const char* beginWirte()const {
		return begin() + writerIndex_;
	}

	ssize_t readFd(int fd, int* saveErrno);
	ssize_t writeFd(int fd, int* saveErrno);

	//Ϊ�˷������http "\r\n"λ��
	const char* findCRLF()const {
		const char* crlf = std::search(peek(), beginWirte(), kCRLF, kCRLF + 2);
		return crlf == beginWirte() ? nullptr : crlf;
	}

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
		if (writeableBytes() + prependableBytes() < len + KCheapPrepend) {
			buffer_.resize(writerIndex_ + len);
		}
		else {
			auto readable = readableBytes();

			//Ų��
			std::copy(begin() + readerIndex_, begin() + writerIndex_, begin() + KCheapPrepend);
			//����readerIndex_�� writerIndex_
			readerIndex_ = KCheapPrepend;
			writerIndex_ = readerIndex_ + readable;
		}
	}
private:
	std::vector<char> buffer_;
	size_t readerIndex_;
	size_t writerIndex_;

	static const char kCRLF[];


};