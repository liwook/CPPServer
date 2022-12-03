#include"Buffer.h"
#include<sys/uio.h>
#include<unistd.h>
#include<errno.h>

ssize_t Buffer::readFd(int fd, int* saveErrno)
{
	char extrabuffer[65535];
	struct iovec vec[2];

	auto writable = writeableBytes();

	vec[0].iov_base = begin() + writerIndex_;	//第一块缓冲区
	vec[0].iov_len = writable;	//iov_base缓冲区可写的空间大小

	vec[1].iov_base = extrabuffer;	//第二快缓冲区
	vec[1].iov_len = sizeof(extrabuffer);

	//若Buffer有65535字节的空间空间，就不适用栈上的缓冲区
	auto iovcnt = (writable < sizeof(extrabuffer)) ? 2 : 1;
	auto n = ::readv(fd, vec, iovcnt);
	if (n < 0) {
		*saveErrno = errno;
	}
	else if (static_cast<size_t>(n) <= writable) {
		writerIndex_ += n;
	}
	else {
		//Buffer底层的可写空间不够存放n字节数据，
		writerIndex_ = buffer_.size();	//更新writerIndex_为末尾，再使用append
		append(extrabuffer, n - writable);
	}

	return n;
}
ssize_t Buffer::writeFd(int fd, int* saveErrno)
{
	auto n = ::write(fd, peek(), readableBytes());
	if (n < 0) {
		*saveErrno = errno;
	}
	return n;
}


