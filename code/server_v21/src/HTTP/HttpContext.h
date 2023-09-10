#pragma once
#include"HttpRequest.h"

class Buffer;

class HttpContext
{
public:
	enum class HttpRequestPaseState
	{
		kExpectRequestLine,	//请求行
		kExpectHeaders,
		kExpectBody,
		kGotAll,
	};

	HttpContext()
		:state_(HttpRequestPaseState::kExpectRequestLine)//默认从请求行开始解析
	{
	}

	bool parseRequest(Buffer* buf, Timestamp reciveTime);// 解析请求Buffer
	bool parseRequest(Buffer* buf);// 解析请求Buffer


	bool gotAll()const { return state_ == HttpRequestPaseState::kGotAll; }

	void reset()// 为了复用HttpContext
	{
		state_ = HttpRequestPaseState::kExpectRequestLine;
		HttpRequest dumy;
		request_.swap(dumy);
	}

	const HttpRequest& request() const
	{
		return request_;
	}

	HttpRequest& request()
	{
		return request_;
	}

private:
	bool processRequestLine(const char* begin, const char* end);


	HttpRequestPaseState state_;	//状态机
	HttpRequest request_;
};