#pragma once

#include <functional>
#include <memory>

class Buffer;
class Connection;

using ConnectionPtr= std::shared_ptr<Connection>;

using CloseCallback = std::function<void(const ConnectionPtr&)> ;
using WriteCompleteCallback = std::function<void(const ConnectionPtr&)> ;
using ConnectionCallback = std::function<void(const ConnectionPtr&)>;

using TimerCallback = std::function<void()>;

// the data has been read to (buf, len)
using MessageCallback = std::function<void(const ConnectionPtr&,
	Buffer*)> ;

