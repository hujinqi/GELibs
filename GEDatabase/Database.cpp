/*
 * Database.cpp
 *
 *  Created on: 2018年8月2日
 *      Author: carlos
 */


#include "Database.h"
#include "GEThread/ThreadPool.h"
#include <stdarg.h>
#include <glog/logging.h>
#include "GETime/TimeMgr.h"

Database::Database()
{
	Connections = NULL;
	mConnectionCount = -1;   // Not connected.
}

Database::~Database()
{
//	if(mConnectionCount > 0)
//	{
//		for(int i=0; i < mConnectionCount; ++i)
//		{
//			DatabaseConnection * con = Connections[i];
//			con->
//		}
//	}
}

DatabaseConnection * Database::GetFreeConnection()
{
	uint32 i = 0;
	for(;;)
	{
		DatabaseConnection * con = Connections[ ((i++) % mConnectionCount) ];
		if(con->AttemptAcquire())
		{
			mBusyCount++;
			return con;
		}
	}

	// shouldn't be reached
	return NULL;
}

DatabaseConnection * Database::GetConnection(int id)
{
	DatabaseConnection * con = Connections[ (id % mConnectionCount) ];
	con->Acquire();
	mBusyCount++;
	return con;
}

QueryResult * Database::Query(const char* QueryString, ...)
{
	char sql[16384];
	va_list vlist;
	va_start(vlist, QueryString);
	vsnprintf(sql, 16384, QueryString, vlist);
	va_end(vlist);

	// Send the query
	QueryResult * qResult = NULL;
	DatabaseConnection * con = GetFreeConnection();

	if(_SendQuery(con, sql, false))
		qResult = _StoreQueryResult( con );

	mBusyCount--;
	con->Release();

	return qResult;
}

QueryResult * Database::QueryNA(const char* QueryString)
{
	// Send the query
	QueryResult * qResult = NULL;
	DatabaseConnection * con = GetFreeConnection();

	if( _SendQuery( con, QueryString, false ) )
		qResult = _StoreQueryResult( con );

	mBusyCount--;
	con->Release();

	return qResult;
}

QueryResult * Database::FQuery(const char * QueryString, DatabaseConnection * con)
{
	// Send the query
	QueryResult * qResult = NULL;
	if( _SendQuery( con, QueryString, false ) )
		qResult = _StoreQueryResult( con );

	return qResult;
}

bool Database::FExecute(const char* QueryString, DatabaseConnection * con)
{
	return _SendQuery( con, QueryString, false );
}

bool Database::Execute(const char* QueryString, ...)
{
	char sql[16384];
	va_list vlist;
	va_start(vlist, QueryString);
	vsnprintf(sql, 16384, QueryString, vlist);
	va_end(vlist);

	DatabaseConnection * con = GetFreeConnection();
	bool Result = _SendQuery(con, sql, false);

	mBusyCount--;
	con->Release();

	return Result;
}

bool Database::ExecuteNA(const char* QueryString)
{
	DatabaseConnection * con = GetFreeConnection();
	bool Result = _SendQuery(con, QueryString, false);

	mBusyCount--;
	con->Release();

	return Result;
}

DBResult::DBResult()
{
	pResult = NULL;
}

DBResult::DBResult(QueryResult* r)
{
	pResult = r;
}

DBResult::~DBResult()
{
	if( pResult != NULL )
	{
		delete pResult;
		pResult = NULL;
	}
}

void DBResult::Release()
{
	if( pResult != NULL )
	{
		delete pResult;
		pResult = NULL;
	}
}

DBResult& DBResult::operator=(QueryResult* r)
{
	if( pResult != NULL )
	{
//		LOG(DEBUG) << "delete old db result" << endl;
//		sLog.Debug("DBResult","delete old db result");
		delete pResult;
		pResult = NULL;
	}

	pResult = r;
	return *this;
}

DBResult& DBResult::operator=(DBResult& r)
{
	if (pResult != NULL)
	{
		delete pResult;
		pResult = NULL;
	}

	pResult = r.pResult;
	r.pResult = NULL;
	return *this;
}
