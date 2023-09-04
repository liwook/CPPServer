#pragma once
#include<string>
#include"../timestamp.h"
#include<unordered_map>
using std::string;

class HttpRequest
{
public:
	enum class Method
	{
		kInvalid, kGet, kPost, kHead, kPut, kDelete
	};
	enum class Version
	{
		kUnknown, kHttp10, kHttp11
	};

	HttpRequest()
		:method_(Method::kInvalid),
		version_(Version::kUnknown)
	{

	}

	void setVersion(Version v) { version_ = v; }
	Version getVersion()const { return version_; }

	bool setMethod(const char* start, const char* end)
	{
		string m(start, end);
		if (m == "GET") {
			method_ = Method::kGet;
		}
		else if (m == "POST") {
			method_ = Method::kPost;
		}
		else if (m == "HEAD") {
			method_ = Method::kHead;
		}
		else if (m == "PUT") {
			method_ = Method::kPut;
		}
		else if (m == "DELETE") {
			method_ = Method::kDelete;
		}
		else {
			method_ = Method::kInvalid;
		}
		return method_ != Method::kInvalid;
	}
	Method getMothod()const { return method_; }

	const char* methodString()const {
		const char* result = "UNKNOWN";
		switch (method_) {
		case Method::kGet:
			result = "GET";
			break;
		case Method::kPost:
			result = "POST";
			break;
		case Method::kHead:
			result = "PUT";
			break;
		case Method::kDelete:
			result = "DELETE";
			break;
		default:
			break;
		}
		return result;
	}

	void setPath(const char* start, const char* end) {
		path_.assign(start, end);
	}
	const string& path()const { return path_; }

	void setQuery(const char* start, const char* end) {
		query_.assign(start, end);
	}
	const string& query()const { return query_; }

	void setRecieveTime(Timestamp t) { receiveTime_ = t; }
	Timestamp receiveTime()const { return receiveTime_; }

	void addHeader(const char* start, const char* colon, const char* end)
	{
		//isspace(int c)函数判断字符c是否为空白符
		//说明：当c为空白符时，返回非零值，否则返回零。（空白符指空格、水平制表、垂直制表、换页、回车和换行符。
		
		// 要求冒号前无空格
		string field(start, colon);
		++colon;
		while (colon < end && isspace(*colon))// 过滤冒号后的空格
			++colon;

		string value(colon, end);
		while (!value.empty() && isspace(value[value.size() - 1]))//过滤value中的空格
			value.resize(value.size() - 1);

		headers_[field] = value;
	}

	string getHeader(const string& field)const
	{
		string result;
		auto it = headers_.find(field);
		if (it != headers_.end()) {
			return it->second;
		}
		return result;
	}

	void swap(HttpRequest& that)
	{
		std::swap(method_, that.method_);
		std::swap(version_, that.version_);
		path_.swap(that.path_);
		query_.swap(that.query_);
		//receiveTime_.swap(that.receiveTime_);
		headers_.swap(that.headers_);
	}

	const std::unordered_map<string, string>& headers()const { return headers_; }


private:
	Method method_;
	Version version_;
	string path_;	//请求路径
	string query_;	//请求体
	
	Timestamp receiveTime_;
	std::unordered_map<string, string> headers_;
};