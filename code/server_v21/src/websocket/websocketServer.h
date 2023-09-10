#pragma once
#include<functional>
#include"../Server.h"
#include"websocketPacket.h"
class WebsocketContext;
class HttpRequest;
class HttpResponse;

class websocketServer
{
public:
	//在回调函数中，目前还没想到WebsocketPacket类的用处
	//using WebsocketCallback = std::function<void(const Buffer*, Buffer*, WebsocketPacket& respondPacket)>;
	using WebsocketCallback = std::function<void(const Buffer*, Buffer*)>;


	websocketServer(EventLoop* loop, const InetAddr& listenAddr);
	~websocketServer();


	EventLoop* getLoop()const { return server_.getLoop(); }

	void setHttpCallback(const WebsocketCallback& cb) { websocketCallback_ = cb; }

	void start(int numThreads);

private:
	void onConnetion(const ConnectionPtr& conn);	//连接到来的回调函数
	void onMessage(const ConnectionPtr& conn, Buffer* buf);	//消息到来的回调函数
	void handleData(const ConnectionPtr& conn, WebsocketContext* websocket, Buffer* buf);

	Server server_;
	WebsocketCallback websocketCallback_;
};