#ifndef _THREADING_STARTER_H
#define _THREADING_STARTER_H

#ifdef WIN32
#include <winsock2.h>
#include <Windows.h>
#else

#endif

#include "common/defines.h"

class SERVER_DECL ThreadBase
{
public:
	ThreadBase() {}
	virtual ~ThreadBase() {}
	virtual bool run() = 0;
	virtual void OnShutdown() {}

	uint32 GetThreadId() { return threadId; }
	void SetThreadId(uint32 threadId) { this->threadId = threadId; }
#ifdef WIN32
	HANDLE THREAD_HANDLE;
#else
	pthread_t THREAD_HANDLE;
#endif
	uint32 threadId;
};

#endif

