#include"currentthread.h"
#include<stdio.h>

namespace CurrentThread
{
	// internal
	thread_local int t_cachedTid = 0;
	thread_local char t_tidString[32];
	thread_local int t_tidStringLength = 6;

	pid_t gettid()
	{
		return static_cast<pid_t>(::syscall(SYS_gettid));
	}

	void cacheTid()
	{
		if (t_cachedTid == 0)
		{
			t_cachedTid = gettid();
			t_tidStringLength = snprintf(t_tidString, sizeof(t_tidString), "%5d ", t_cachedTid);
		}
	}

	//inline bool isMainThread()
	//{
	//	return tid() == ::getpid();
	//}


	//inline const char* tidString() // for logging
	//{
	//	return t_tidString;
	//}

	//inline int tidStringLength() // for logging
	//{
	//	return t_tidStringLength;
	//}
}  // namespace CurrentThread

