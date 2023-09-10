#include"HttpServer.h"
#include"HttpRequest.h"
#include"HttpResponse.h"
#include"HttpContext.h"

//Ĭ�ϵĻص�����
void defaultHttpCallback(const HttpRequest& req, HttpResponse* resp)
{
	
	resp->setStatusCode(HttpResponse::HttpStatusCode::k404NotFound);
	resp->setstatusMessage("Not Found");
	resp->setCloseConnection(true);
}

HttpServer::HttpServer(EventLoop* loop, const InetAddr& listenAddr)
	:server_(listenAddr,loop)
	, httpCallback_([](const HttpRequest& req, HttpResponse* resp) {defaultHttpCallback(req, resp); })
{
	server_.setConnectionCallback([this](const ConnectionPtr& conn) {onConnetion(conn); });
	server_.setMessageCallback([this](const ConnectionPtr& conn, Buffer* buf) {onMessage(conn, buf); });
}



void HttpServer::start(int numThreads)
{
	server_.start(numThreads);
}


void HttpServer::onConnetion(const ConnectionPtr& conn)
{
	if (conn->connected()) {
		//conn->setContext(std::make_shared<HttpContext>());
		conn->setContext(HttpContext());
		//����ʹ�ã��������԰󶨲����ϵ�����
		//int a = 10;
		//conn->setContext(a);
	}
}
void HttpServer::onMessage(const ConnectionPtr& conn, Buffer* buf)
{
	//HttpContext* context = reinterpret_cast<HttpContext*>(conn->getConntext().get());	//c++11����
	auto context = std::any_cast<HttpContext>(conn->getMutableContext());	//c++117
	if (!context) {
		printf("context kong...\n");
		LOG_ERROR<<"context is bad\n";
		return;
	}

	
	if (!context->parseRequest(buf)) {
		conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
		conn->shutdown();
		printf("http 400\n");
	}


	if (context->gotAll()) {
		onRequest(conn, context->request());
		context->reset();//һ����������ϣ�����context����ΪHttpContext��Connection���ˣ�������Ҫ����ظ�ʹ��
	}
}

void HttpServer::onRequest(const ConnectionPtr& conn, const HttpRequest& req)
{
	const std::string& connetion = req.getHeader("Connection");
	bool close = connetion == "close" || (req.getVersion() == HttpRequest::Version::kHttp10 && connetion != "Keep-Alive");

	HttpResponse response(close);
	//ִ���û�ע��Ļص�����
	httpCallback_(req, &response);

	Buffer buf;
	response.appendToBuffer(&buf);
	conn->send(&buf);
	if (response.closeConnection()) {
		conn->shutdown();
	}
}