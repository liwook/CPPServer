#pragma once
#include<functional>
#include"../Server.h"
class HttpRequest;
class HttpResponse;

class HttpServer
{
public:
	using HttpCallback = std::function<void(const HttpRequest&, HttpResponse*)>;

	HttpServer(EventLoop* loop, const InetAddr& listenAddr);

	EventLoop* getLoop()const { return server_.getLoop(); }

	void setHttpCallback(const HttpCallback& cb) { httpCallback_ = cb; }


	void start(int numThreads);

private:
	void onConnetion(const ConnectionPtr& conn);	//连接到来的回调函数
	void onMessage(const ConnectionPtr& conn, Buffer* buf);	//消息到来的回调函数
	void onRequest(const ConnectionPtr& conn, const HttpRequest&);


	Server server_;
	HttpCallback httpCallback_;

};