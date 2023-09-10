#pragma once

#include "websocketRequest.h"
#include"../Buffer.h"
#include"websocketPacket.h"

class WebsocketContext {
public:
	enum class WebsocketSTATUS { kUnconnect, kHandsharked };

	WebsocketContext();
	~WebsocketContext();

	void handleShared(Buffer* buf, const std::string& server_key);

	void parseData(Buffer* buf, Buffer* output);
	void reset() { requestPacket_.reset(); }

	void setwebsocketHandshared() { websocketStatus_ = WebsocketSTATUS::kHandsharked; }
	WebsocketSTATUS getWebsocketSTATUS()const { return websocketStatus_; }


	uint8_t getRequestOpcode()const { return requestPacket_.opcode(); }

private:
	WebsocketSTATUS websocketStatus_;

	WebsocketPacket requestPacket_;
};
