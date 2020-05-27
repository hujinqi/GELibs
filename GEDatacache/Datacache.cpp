/*
 * Datacache.cpp
 *
 *  Created on: 2018年3月19日
 *      Author: carlos Hu
 */

#include "Datacache.h"
#include "GEThread/ThreadPool.h"

Datacache::Datacache(): CThread()
{
	Connections = NULL;
	mConnectionCount = 0;
}

Datacache::~Datacache()
{

}

void Datacache::_Initialize()
{
	//run
	ThreadPool.ExecuteTask(this);
}

DatacacheConnection * Datacache::GetFreeConnection()
{
	uint32 i = 0;
	for(;;)
	{
		DatacacheConnection * con = Connections[ ((i++) % mConnectionCount) ];
		if(con->Busy.AttemptAcquire())
			return con;
	}

	// shouldn't be reached
	return NULL;
}

bool Datacache::run()
{
	//SetThreadName("Database Execute Thread");
	SetThreadState(THREADSTATE_BUSY);
	char * query = taskQueue.pop();
	DatacacheConnection * con = GetFreeConnection();
	while (query)
	{
		_SendQuery(con, query, false);
		if (ThreadState == THREADSTATE_TERMINATE)
			break;

		query = taskQueue.pop();
	}

	con->Busy.Release();

	if (taskQueue.get_size() > 0)
	{
		// execute all the remaining queries
		query = taskQueue.pop_nowait();
		while (query)
		{
			DatacacheConnection * con = GetFreeConnection();
			_SendQuery(con, query, false);
			con->Busy.Release();
			query = taskQueue.pop_nowait();
		}
	}

	return false;
}
