#pragma once
#include<stdlib.h>

#include <sys/syscall.h>
#include <unistd.h>
namespace CurrentThread
{
	// internal
	extern thread_local int t_cachedTid;
	extern thread_local char t_tidString[32];
	extern thread_local int t_tidStringLength;

	inline pid_t gettid();

	void cacheTid();

	inline int tid()
	{
		if (__builtin_expect(t_cachedTid == 0, 0)){
			cacheTid();
		}
		return t_cachedTid;
	}

	//bool isMainThread()
	//{
	//	return tid() == ::getpid();
	//}

	inline const char* tidString() // for logging
	{
		return t_tidString;
	}

	inline int tidStringLength() // for logging
	{
		return t_tidStringLength;
	}
}  // namespace CurrentThread

