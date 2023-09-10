#include <unistd.h>
#include"WebsocketContext.h"
#include "base64.h"
#include "sha1.h"
#include <arpa/inet.h>
#include<string.h>

#define MAGIC_KEY "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

WebsocketContext::WebsocketContext()
	:websocketStatus_(WebsocketSTATUS::kUnconnect)
{
}

WebsocketContext::~WebsocketContext(){
}

void WebsocketContext::parseData(Buffer* buf,Buffer* output)
{
	requestPacket_.decodeFrame(buf, output);
}

void WebsocketContext::handleShared(Buffer* buf, const std::string& serverKey)
{
	buf->append("HTTP/1.1 101 Switching Protocols\r\n");
	buf->append("Connection: upgrade\r\n");
	buf->append("Sec-WebSocket-Accept: ");

	std::string server_key = serverKey;
	server_key += MAGIC_KEY;

	SHA1 sha;
	unsigned int message_digest[5];
	sha.Reset();
	sha << server_key.c_str();

	sha.Result(message_digest);
	for (int i = 0; i < 5; i++) {
		message_digest[i] = htonl(message_digest[i]);
	}
	server_key = base64_encode(reinterpret_cast<const unsigned char*>(message_digest), 20);
	server_key += "\r\n";

	buf->append(server_key);
	buf->append("Upgrade: websocket\r\n\r\n");
}


