#include"HttpResponse.h"
#include"../Buffer.h"

void HttpResponse::appendToBuffer(Buffer* output) const
{
	//��Ӧ��
	string buf = "HTTP/1.1 " + std::to_string(static_cast<int>(statusCode_));
	output->append(buf);
	output->append(statusMessage_);
	output->append("\r\n");

	//��Ӧͷ��
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

	output->append("\r\n");	//����
	output->append(body_);	//��Ӧ��
}