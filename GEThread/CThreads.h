
// Class CThread - Base class for all threads in the
// server, and allows for easy management by ThreadMgr.

#ifndef _MAPUPDATER_H
#define _MAPUPDATER_H

#include "ThreadStarter.h"

enum CThreadState
{
	THREADSTATE_TERMINATE = 0,		//结束
	THREADSTATE_PAUSED	= 1,		//暂停
	THREADSTATE_SLEEPING  = 2,		//
	THREADSTATE_BUSY	  = 3,
	THREADSTATE_AWAITING  = 4,
};


struct NameTableEntry;

class SERVER_DECL CThread : public ThreadBase
{
public:
	CThread();
	~CThread();

	GE_INLINE void SetThreadState(CThreadState thread_state) { ThreadState = thread_state; }
	GE_INLINE CThreadState GetThreadState() { return ThreadState; }
//	int GetThreadId() { return ThreadId; }
	time_t GetStartTime() { return start_time; }
	virtual bool run();
	void OnShutdown();

protected:
	CThreadState ThreadState;
	time_t start_time;
//	int ThreadId;
};

#endif
