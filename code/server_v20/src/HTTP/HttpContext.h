#pragma once
#include"HttpRequest.h"

class Buffer;

class HttpContext
{
public:
	enum class HttpRequestPaseState
	{
		kExpectRequestLine,	//������
		kExpectHeaders,
		kExpectBody,
		kGotAll,
	};

	HttpContext()
		:state_(HttpRequestPaseState::kExpectRequestLine)//Ĭ�ϴ������п�ʼ����
	{
	}

	bool parseRequest(Buffer* buf, Timestamp reciveTime);// ��������Buffer
	bool parseRequest(Buffer* buf);// ��������Buffer


	bool gotAll()const { return state_ == HttpRequestPaseState::kGotAll; }

	void reset()// Ϊ�˸���HttpContext
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


	HttpRequestPaseState state_;	//״̬��
	HttpRequest request_;
};