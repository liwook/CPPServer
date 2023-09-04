#include"HttpResponse.h"
#include"../Buffer.h"

void HttpResponse::appendToBuffer(Buffer* output) const
{
	//响应行
	string buf = "HTTP/1.1 " + std::to_string(static_cast<int>(statusCode_));
	output->append(buf);
	output->append(statusMessage_);
	output->append("\r\n");

	//响应头部
	if (closeConnection_) {
		output->append("Connection: close\r\n");
	}
	else {
		output->append("Connection: Keep-Alive\r\n");
		buf = "Content-Length:" + std::to_string(body_.size()) + "\r\n";
		output->append(buf);
	}

	for (const auto& header : headers_) {
		buf = header.first + ": " + header.second + "\r\n";
		output->append(buf);
	}

	output->append("\r\n");	//空行
	output->append(body_);	//响应体
}