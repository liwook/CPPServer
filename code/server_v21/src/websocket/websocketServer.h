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
	//�ڻص������У�Ŀǰ��û�뵽WebsocketPacket����ô�
	//using WebsocketCallback = std::function<void(const Buffer*, Buffer*, WebsocketPacket& respondPacket)>;
	using WebsocketCallback = std::function<void(const Buffer*, Buffer*)>;


	websocketServer(EventLoop* loop, const InetAddr& listenAddr);
	~websocketServer();


	EventLoop* getLoop()const { return server_.getLoop(); }

	void setHttpCallback(const WebsocketCallback& cb) { websocketCallback_ = cb; }

	void start(int numThreads);

private:
	void onConnetion(const ConnectionPtr& conn);	//���ӵ����Ļص�����
	void onMessage(const ConnectionPtr& conn, Buffer* buf);	//��Ϣ�����Ļص�����
	void handleData(const ConnectionPtr& conn, WebsocketContext* websocket, Buffer* buf);

	Server server_;
	WebsocketCallback websocketCallback_;
};