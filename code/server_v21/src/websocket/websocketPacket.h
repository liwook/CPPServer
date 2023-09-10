#pragma once
#include <stdint.h>
#include<string.h>

class Buffer;


enum WSOpcodeType : uint8_t
{
	WSOpcode_Continue = 0x0,
	WSOpcode_Text = 0x1,
	WSOpcode_Binary = 0x2,
	WSOpcode_Close = 0x8,
	WSOpcode_Ping = 0x9,
	WSOpcode_Pong = 0xA,
};

class WebsocketPacket
{
public:
	WebsocketPacket()
		:fin_(1)	//1表示是消息的最后一个分片,表示不分包
		, rsv1_(0)
		, rsv2_(0)
		, rsv3_(0)
		, opcode_(1)	//默认是发送文本帧
		, mask_(0)
		, payload_length_(0)
	{
		memset(masking_key_, 0, sizeof(masking_key_));
	}
	~WebsocketPacket()
	{
	}

	void reset()
	{
		fin_ = 1;	//默认是1
		rsv1_ = 0;
		rsv2_ = 0;
		rsv3_ = 0;
		opcode_ = 1;//默认是发送文本帧
		mask_ = 0;
		memset(masking_key_, 0, sizeof(masking_key_));

		payload_length_ = 0;
	}
	void decodeFrame(Buffer* frameBuf, Buffer* output);

	void encodeFrame(Buffer* output, Buffer* data)const;

	void addFrameHeader(Buffer* output);

public:
	uint8_t fin() const { return fin_; }
	uint8_t rsv1() const { return rsv1_; }
	uint8_t rsv2()const { return rsv2_; }
	uint8_t rsv3() const { return rsv3_; }
	uint8_t opcode()const { return opcode_; }
	uint8_t mask()const { return mask_; }
	uint64_t payload_length() const { return payload_length_; }

	void set_fin(uint8_t fin) { fin_ = fin; }
	void set_rsv1(uint8_t rsv1) { rsv1_ = rsv1; }
	void set_rsv2(uint8_t rsv2) { rsv2_ = rsv2; }
	void set_rsv3(uint8_t rsv3) { rsv3_ = rsv3; }
	void set_opcode(uint8_t opcode) { opcode_ = opcode; }
	void set_mask(uint8_t mask) { mask_ = mask; }
	void set_payload_length(uint64_t length) { payload_length_ = length; }

private:
    uint8_t fin_;
    uint8_t rsv1_;
    uint8_t rsv2_;
    uint8_t rsv3_;
    uint8_t opcode_;
    uint8_t mask_;
    uint8_t masking_key_[4];
    uint64_t payload_length_;
};