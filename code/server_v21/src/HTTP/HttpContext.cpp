#include"HttpContext.h"
#include"../Buffer.h"

bool HttpContext::parseRequest(Buffer* buf, Timestamp receiveTime)
{
	bool ok = true;
	bool hasMore = true;

	while (hasMore) {
		if (state_ == HttpRequestPaseState::kExpectRequestLine) {	//����������
			//���ҳ�buf�е�һ�γ���"\r\n"λ��
			const char* crlf = buf->findCRLF();
			if (crlf) {
				//�����ҵ�"\r\n",˵��������һ�����ݣ����Խ��н���
				//buf->peek()Ϊ���ݿ�ʼ����
				ok = processRequestLine(buf->peek(), crlf);
				if (ok) {//�����ɹ�
					request_.setRecieveTime(receiveTime);
					buf->retrieveUntil(crlf + 2);//buf->peek()����ƶ�2�ֽڣ�����һ��
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
			const char* crlf = buf->findCRLF();	//�ҵ�"\r\n"λ��
			if (crlf) {
				const char* colon = std::find(buf->peek(), crlf, ':');//��λ�ָ���
				if (colon != crlf) {
					request_.addHeader(buf->peek(), colon, crlf);	//��Ӽ�ֵ��
				}
				else {
					/*state_ = HttpRequestPaseState::kGotAll;
					hasMore = false;*/

					state_ = HttpRequestPaseState::kExpectBody;//�����Ϳ��Խ���body
				}
				buf->retrieveUntil(crlf + 2);	//���ƶ�2�ֽ�
			}
			else {
				hasMore = false;
			}
		}
		else if (state_ == HttpRequestPaseState::kExpectBody) {//����������
			if (buf->readableBytes()) {//�����������ݣ��Ǿ���������
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
		if (state_ == HttpRequestPaseState::kExpectRequestLine) {	//����������
			//���ҳ�buf�е�һ�γ���"\r\n"λ��
			const char* crlf = buf->findCRLF();
			if (crlf) {
				//�����ҵ�"\r\n",˵��������һ�����ݣ����Խ��н���
				//buf->peek()Ϊ���ݿ�ʼ����
				ok = processRequestLine(buf->peek(), crlf);
				if (ok) {//�����ɹ�
					buf->retrieveUntil(crlf + 2);//buf->peek()����ƶ�2�ֽڣ�����һ��
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
			const char* crlf = buf->findCRLF();	//�ҵ�"\r\n"λ��
			if (crlf) {
				const char* colon = std::find(buf->peek(), crlf, ':');//��λ�ָ���
				if (colon != crlf) {
					request_.addHeader(buf->peek(), colon, crlf);	//��Ӽ�ֵ��
				}
				else {
					/*state_ = HttpRequestPaseState::kGotAll;
					hasMore = false;*/

					state_ = HttpRequestPaseState::kExpectBody;//�����Ϳ��Խ���body
				}
				buf->retrieveUntil(crlf + 2);	//���ƶ�2�ֽ�
			}
			else {
				hasMore = false;
			}
		}
		else if (state_ == HttpRequestPaseState::kExpectBody) {//����������
			if (buf->readableBytes()) {//�����������ݣ��Ǿ���������
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
	//�������й̶���ʽMethod URL Version CRLF��URL�п��ܴ������������
	//���ݿո�����зָ�������ַ���URL���ܴ������������ʹ��"?���ָ����
	//bool succeed = false;
	bool succeed = true;

	const char* start = begin;
	const char* space = std::find(start, end, ' ');
	//��һ���ո�ǰ���ַ��������󷽷� ���磺post
	if (space != end && request_.setMethod(start, space)) {
		start = space + 1;
		space = std::find(start, end, ' ');//Ѱ�ҵڶ����ո� url
		if (space != end) {
			const char* question = std::find(start, space, '?');
			if (question != space) {// ��"?"���ָ��path���������
				request_.setPath(start, question);
				request_.setQuery(question, space);
			}
			else {
				request_.setPath(start, space);//ֻ��path
			}
	
			start = space + 1;
			//���һ���֣�����httpЭ��汾
			//succeed = end - start == 8 && std::equal(start, end - 1, "HTTP/1.");
			//if(succeed) {
			//	if (*(end - 1) == '1') //������1.1�汾
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
