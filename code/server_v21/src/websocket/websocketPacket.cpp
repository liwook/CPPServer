#include"websocketPacket.h"
#include"../Buffer.h"
#include <arpa/inet.h>

void WebsocketPacket::decodeFrame(Buffer* frameBuf,Buffer* output)
{
	const char* msg = frameBuf->peek();

	int pos = 0;
	//获取fin_
	fin_=((unsigned char)msg[pos] >> 7);
	//获取opcode_
	opcode_ = msg[pos] & 0x0f;
	pos++;
	//获取mask_
	mask_ = (unsigned char)msg[pos] >> 7;
	//获取payload_length_
	payload_length_ = msg[pos] & 0x7f;
	pos++;
	if (payload_length_ == 126) {
		uint16_t length = 0;
		memcpy(&length, msg + pos, 2);
		pos += 2;
		payload_length_ = ntohs(length);
	}
	else if (payload_length_ == 127) {
		uint64_t length = 0;
		memcpy(&length, msg + pos, 8);
		pos += 8;
		payload_length_ = ntohl(length);
	}
	//获取masking_key_
	if (mask_ == 1) {
		for (int i = 0; i < 4; i++)
			masking_key_[i] = msg[pos + i];
		pos += 4;
	}


	if (mask_ != 1) {
		output->append(msg + pos, payload_length_);
	}
	else {
		for (uint64 i = 0; i < payload_length_; i++) {
			output->append(msg[pos + i] ^ masking_key_[i % 4], payload_length_);
		}
	}

}

void WebsocketPacket::encodeFrame(Buffer* output,Buffer* data)const
{
	uint8_t onebyte = 0;
	onebyte |= (fin_ << 7);
	onebyte |= (rsv1_ << 6);
	onebyte |= (rsv2_ << 5);
	onebyte |= (rsv3_ << 4);
	onebyte |= (opcode_ & 0x0F);
	output->append((char*)&onebyte, 1);

	onebyte = 0;
	//set mask flag
	onebyte = onebyte | (mask_ << 7);

	int length = data->readableBytes();

	if (length < 126){
		onebyte |= length;
		output->append((char*)&onebyte, 1);
	}
	else if (length == 126){
		onebyte |= length;
		output->append((char*)&onebyte, 1);

		auto len = htons(length);
		output->append((char*)&len, 2);

	}
	else if (length == 127){
		onebyte |= length;
		output->append((char*)&onebyte, 1);

		// also can use htonll if you have it
		onebyte = (payload_length_ >> 56) & 0xFF;
		output->append((char*)&onebyte, 1);
		onebyte = (payload_length_ >> 48) & 0xFF;
		output->append((char*)&onebyte, 1);
		onebyte = (payload_length_ >> 40) & 0xFF;
		output->append((char*)&onebyte, 1);
		onebyte = (payload_length_ >> 32) & 0xFF;
		output->append((char*)&onebyte, 1);
		onebyte = (payload_length_ >> 24) & 0xFF;
		output->append((char*)&onebyte, 1);
		onebyte = (payload_length_ >> 16) & 0xFF;
		output->append((char*)&onebyte, 1);
		onebyte = (payload_length_ >> 8) & 0xFF;
		output->append((char*)&onebyte, 1);
		onebyte = payload_length_ & 0XFF;
		output->append((char*)&onebyte, 1);
	}

	if (mask_ == 1)	//服务器发送给客户端的，是不带mask_key的，所以这个是没有用到的
	{
		output->append((char*)masking_key_, 4);	// save masking key

		char value = 0;
		for (uint64_t i = 0; i < payload_length_; ++i) {
			value = *(char*)(data->peek());
			data->retrieve(1);
			value = value ^ masking_key_[i % 4];
			output->append(&value, 1);
		}
	}
	else {
		output->append(data->peek(), data->readableBytes());
	}
}



void WebsocketPacket::addFrameHeader(Buffer* output)
{
	payload_length_ = output->readableBytes() - 14;

	uint8_t onebyte = 0;
	onebyte |= (fin_ << 7);
	onebyte |= (rsv1_ << 6);
	onebyte |= (rsv2_ << 5);
	onebyte |= (rsv3_ << 4);
	onebyte |= (opcode_ & 0x0F);

	output->append((char*)&onebyte, 1);

	onebyte = 0;
	//set mask flag
	onebyte = onebyte | (mask_ << 7);

	int length = payload_length_;

	if (length < 126)
	{
		onebyte |= length;
		output->append((char*)&onebyte, 1);
	}
	else if (length == 126)
	{
		onebyte |= length;
		output->append((char*)&onebyte, 1);

		auto len = htons(length);
		output->append((char*)&len, 2);

	}
	else if (length == 127)
	{
		onebyte |= length;
		output->append((char*)&onebyte, 1);

		// also can use htonll if you have it
		onebyte = (payload_length_ >> 56) & 0xFF;
		output->append((char*)&onebyte, 1);
		onebyte = (payload_length_ >> 48) & 0xFF;
		output->append((char*)&onebyte, 1);
		onebyte = (payload_length_ >> 40) & 0xFF;
		output->append((char*)&onebyte, 1);
		onebyte = (payload_length_ >> 32) & 0xFF;
		output->append((char*)&onebyte, 1);
		onebyte = (payload_length_ >> 24) & 0xFF;
		output->append((char*)&onebyte, 1);
		onebyte = (payload_length_ >> 16) & 0xFF;
		output->append((char*)&onebyte, 1);
		onebyte = (payload_length_ >> 8) & 0xFF;
		output->append((char*)&onebyte, 1);
		onebyte = payload_length_ & 0XFF;
		output->append((char*)&onebyte, 1);
	}
}
