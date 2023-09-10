#include"HttpContext.h"
#include"../Buffer.h"

bool HttpContext::parseRequest(Buffer* buf, Timestamp receiveTime)
{
	bool ok = true;
	bool hasMore = true;

	while (hasMore) {
		if (state_ == HttpRequestPaseState::kExpectRequestLine) {	//解析请求行
			//查找出buf中第一次出现"\r\n"位置
			const char* crlf = buf->findCRLF();
			if (crlf) {
				//若是找到"\r\n",说明至少有一行数据，可以进行解析
				//buf->peek()为数据开始部分
				ok = processRequestLine(buf->peek(), crlf);
				if (ok) {//解析成功
					request_.setRecieveTime(receiveTime);
					buf->retrieveUntil(crlf + 2);//buf->peek()向后移动2字节，到下一行
					state_ = HttpRequestPaseState::kExpectHeaders;
				}
				else {
					hasMore = false;
				}
			}
			else {
				hasMore = false;
			}
		}
		else if (state_ == HttpRequestPaseState::kExpectHeaders) {
			const char* crlf = buf->findCRLF();	//找到"\r\n"位置
			if (crlf) {
				const char* colon = std::find(buf->peek(), crlf, ':');//定位分隔符
				if (colon != crlf) {
					request_.addHeader(buf->peek(), colon, crlf);	//添加键值对
				}
				else {
					/*state_ = HttpRequestPaseState::kGotAll;
					hasMore = false;*/

					state_ = HttpRequestPaseState::kExpectBody;//这样就可以解析body
				}
				buf->retrieveUntil(crlf + 2);	//后移动2字节
			}
			else {
				hasMore = false;
			}
		}
		else if (state_ == HttpRequestPaseState::kExpectBody) {//解析请求体
			if (buf->readableBytes()) {//表明还有数据，那就是请求体
				request_.setQuery(buf->peek(), buf->beginWirte());
			}
			state_ = HttpRequestPaseState::kGotAll;
			hasMore = false;
		}
	}

	return ok;
}


bool HttpContext::parseRequest(Buffer* buf)
{
	bool ok = true;
	bool hasMore = true;
	//printf("HttpContext::parseReques:buf:\n%s\n",buf->peek());
	while (hasMore) {
		if (state_ == HttpRequestPaseState::kExpectRequestLine) {	//解析请求行
			//查找出buf中第一次出现"\r\n"位置
			const char* crlf = buf->findCRLF();
			if (crlf) {
				//若是找到"\r\n",说明至少有一行数据，可以进行解析
				//buf->peek()为数据开始部分
				ok = processRequestLine(buf->peek(), crlf);
				if (ok) {//解析成功
					buf->retrieveUntil(crlf + 2);//buf->peek()向后移动2字节，到下一行
					state_ = HttpRequestPaseState::kExpectHeaders;
				}
				else {
					hasMore = false;
				}
			}
			else {
				hasMore = false;
			}
		}
		else if (state_ == HttpRequestPaseState::kExpectHeaders) {
			const char* crlf = buf->findCRLF();	//找到"\r\n"位置
			if (crlf) {
				const char* colon = std::find(buf->peek(), crlf, ':');//定位分隔符
				if (colon != crlf) {
					request_.addHeader(buf->peek(), colon, crlf);	//添加键值对
				}
				else {
					/*state_ = HttpRequestPaseState::kGotAll;
					hasMore = false;*/

					state_ = HttpRequestPaseState::kExpectBody;//这样就可以解析body
				}
				buf->retrieveUntil(crlf + 2);	//后移动2字节
			}
			else {
				hasMore = false;
			}
		}
		else if (state_ == HttpRequestPaseState::kExpectBody) {//解析请求体
			if (buf->readableBytes()) {//表明还有数据，那就是请求体
				request_.setQuery(buf->peek(), buf->beginWirte());
			}
			state_ = HttpRequestPaseState::kGotAll;
			hasMore = false;
		}
	}

	return ok;
}


bool HttpContext::processRequestLine(const char* begin, const char* end)
{
	//请求行有固定格式Method URL Version CRLF，URL中可能带有请求参数。
	//根据空格符进行分割成三段字符。URL可能带有请求参数，使用"?”分割解析
	//bool succeed = false;
	bool succeed = true;

	const char* start = begin;
	const char* space = std::find(start, end, ' ');
	//第一个空格前的字符串是请求方法 例如：post
	if (space != end && request_.setMethod(start, space)) {
		start = space + 1;
		space = std::find(start, end, ' ');//寻找第二个空格 url
		if (space != end) {
			const char* question = std::find(start, space, '?');
			if (question != space) {// 有"?"，分割成path和请求参数
				request_.setPath(start, question);
				request_.setQuery(question, space);
			}
			else {
				request_.setPath(start, space);//只有path
			}
	
			start = space + 1;
			//最后一部分，解析http协议版本
			//succeed = end - start == 8 && std::equal(start, end - 1, "HTTP/1.");
			//if(succeed) {
			//	if (*(end - 1) == '1') //表明是1.1版本
			//		request_.setVersion(HttpRequest::Version::kHttp11);
			//	else if (*(end - 1) == '0')
			//		request_.setVersion(HttpRequest::Version::kHttp10);
			//	else
			//		succeed = false;
			//}

			string version(start, end);
			//printf("version=%s\n", version.data());
			if (version == "HTTP/1.0")
				request_.setVersion(HttpRequest::Version::kHttp10);
			else if (version == "HTTP/1.1")
				request_.setVersion(HttpRequest::Version::kHttp11);
			else
				succeed = false;
		}
	}
	return succeed;
}
