#include"websocketServer.h"
#include"../HTTP/HttpRequest.h"
#include"WebsocketContext.h"
#include"../HTTP/HttpContext.h"
#include"../Buffer.h"


//Ĭ�ϵĻص�����
void defaultWebsocketCallback(const Buffer* buf, Buffer* sendBuf)
{
	//����echo��ԭ���ݷ���
	sendBuf->append(buf->peek(), buf->readableBytes());
}

websocketServer::websocketServer(EventLoop* loop, const InetAddr& listenAddr)
	:server_(listenAddr,loop)
	, websocketCallback_(defaultWebsocketCallback)
{
	server_.setConnectionCallback([this](const ConnectionPtr& conn) {onConnetion(conn); });
	server_.setMessageCallback([this](const ConnectionPtr& conn, Buffer* buf) {onMessage(conn, buf); });
}

websocketServer::~websocketServer()
{
}

void websocketServer::start(int numThreads)
{
	server_.start(numThreads);
}


void websocketServer::onConnetion(const ConnectionPtr& conn)
{
	if (conn->connected()) {
		//conn->setContext(std::make_shared<HttpContext>());
		//conn->setContext(HttpContext());
		conn->setContext(WebsocketContext());

		//����ʹ�ã��������԰󶨲����ϵ�����
		//int a = 10;
		//conn->setContext(a);
	}
}
void websocketServer::onMessage(const ConnectionPtr& conn, Buffer* buf)
{
	auto context = std::any_cast<WebsocketContext>(conn->getMutableContext());	//c++117
	if (!context) {
		printf("context kong...\n");
		LOG_ERROR << "context is bad\n";
		return;
	}

	if (context->getWebsocketSTATUS() == WebsocketContext::WebsocketSTATUS::kUnconnect) {
		HttpContext http;
		if (!http.parseRequest(buf)) {
			conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
			conn->shutdown();
		}

		if (http.gotAll()) {
			auto httpRequese = http.request();
			if (httpRequese.getHeader("Upgrade") != "websocket" ||
				httpRequese.getHeader("Connection") != "Upgrade" ||
				httpRequese.getHeader("Sec-WebSocket-Version") != "13" ||
				httpRequese.getHeader("Sec-WebSocket-Key") == "") {

				conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
				conn->shutdown();
				return;		//��������websocket����
			}

			Buffer handsharedbuf;
			context->handleShared(&handsharedbuf, http.request().getHeader("Sec-WebSocket-Key"));
			conn->send(&handsharedbuf);
			context->setwebsocketHandshared();//���ý�������
		}
	}
	else {
		handleData(conn, context, buf);
	}
}

void websocketServer::handleData(const ConnectionPtr& conn, WebsocketContext* websocket, Buffer* buf)
{
	Buffer DataBuf;
	websocket->parseData(buf, &DataBuf);

	WebsocketPacket respondPacket;
	
	int opcode = websocket->getRequestOpcode();
	switch (opcode)
	{
	case WSOpcodeType::WSOpcode_Continue:
		// add your process code here
		respondPacket.set_opcode(WSOpcodeType::WSOpcode_Continue);
		printf("WebSocketEndpoint - recv a Continue opcode.\n");
		break;
	case WSOpcodeType::WSOpcode_Text:
		// add your process code here
		respondPacket.set_opcode(WSOpcodeType::WSOpcode_Text);
		printf("WebSocketEndpoint - recv a Text opcode.\n");
		break;
	case WSOpcodeType::WSOpcode_Binary:
		// add your process code here

		respondPacket.set_opcode(WSOpcodeType::WSOpcode_Binary);
		printf("WebSocketEndpoint - recv a Binary opcode.\n");
		break;
	case WSOpcodeType::WSOpcode_Close:
		// add your process code here
		respondPacket.set_opcode(WSOpcodeType::WSOpcode_Close);
		printf("WebSocketEndpoint - recv a Close opcode.\n");
		break;
	case WSOpcodeType::WSOpcode_Ping:
		// add your process code here
		respondPacket.set_opcode(WSOpcodeType::WSOpcode_Pong);	//����������Ӧ
		printf("WebSocketEndpoint - recv a Ping opcode.\n");
		break;
	case WSOpcodeType::WSOpcode_Pong:		//��ʾ����һ��������Ӧ(pong)���ǾͲ��ûظ���
		// add your process code here
		printf("WebSocketEndpoint - recv a Pong opcode.\n");
		return;
	default:
		LOG_INFO << "WebSocketEndpoint - recv an unknown opcode.\n";
		return;
	}


	Buffer sendbuf;
	if(opcode != WSOpcodeType::WSOpcode_Close && opcode != WSOpcode_Ping && opcode != WSOpcode_Pong)
		websocketCallback_(&DataBuf, &sendbuf);

	Buffer frameBuf;
	respondPacket.encodeFrame(&frameBuf, &sendbuf);
	conn->send(&frameBuf);

	websocket->reset();












	
	//if (static_cast<uint8_t>(WSOpcodeType::WSOpcode_Close) == websocket->getRequestOpcode()) {
	//	//conn->handleClose();
	//	//return;

	//	WebsocketPacket respondPacket;
	//	respondPacket.set_opcode(8);
	//	Buffer frameBuf;
	//	Buffer sendbuf;
	//	respondPacket.appendToBuffer(&frameBuf, &sendbuf);
	//	conn->send(&frameBuf);
	//	return;
	//}
	////ִ���û����õ�ҵ����
	//WebsocketPacket respondPacket;
	//Buffer sendbuf;
	//websocketCallback_(&DataBuf, &sendbuf, respondPacket);

	//Buffer frameBuf;
	//respondPacket.appendToBuffer(&frameBuf, &sendbuf);
	//conn->send(&frameBuf);

	//websocket->reset();
}
