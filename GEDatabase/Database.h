/*
 * Database.h
 *
 *  Created on: 2018年8月2日
 *      Author: carlos
 */

#ifndef SRC_MODULE_GEDATABASE_DATABASE_H_
#define SRC_MODULE_GEDATABASE_DATABASE_H_

#include "Field.h"
#include "GEThread/CThreads.h"
#include "common/Mutex.h"
#include "common/CallBack.h"
#include "common/Queue.h"

#include <map>
#include <string>
#include "SqlStatement.h"

using namespace std;
class QueryResult;
class QueryThread;
class Database;

class DatabaseConnection
{
public:
	DatabaseConnection(int id)
	{
		this->id = id;
	}

	GE_INLINE bool AttemptAcquire()
	{
		if(Busy.AttemptAcquire())
		{
//			DLOG(INFO)<<"=> lock: "<<id;
			return true;
		}
		return false;
	}

	GE_INLINE void Acquire()
	{
		Busy.Acquire();
//		DLOG(INFO)<<"=> lock: "<<id;
	}

	GE_INLINE void Release()
	{
//		DLOG(INFO)<<"=> release: "<<id;
		Busy.Release();
	}

	int GetId() { return id; }

private:
	int id;
	FastMutex Busy;
};

class SERVER_DECL Database
{
	friend class QueryThread;
public:

	Database();
	virtual ~Database();

	/************************************************************************/
	/* Virtual Functions                                                    */
	/************************************************************************/
	virtual bool Initialize(const char* Hostname, unsigned int port,
			const char* Username, const char* Password,
			const char* DatabaseName, uint32 ConnectionCount,
			uint32 BufferSize) = 0;
	virtual void OnShutdown() = 0;

	virtual QueryResult* Query(const char* QueryString, ...);
	virtual QueryResult* QueryNA(const char* QueryString);
	virtual QueryResult* QueryNAString(const string& QueryString) { return QueryNA(QueryString.c_str()); }
	virtual QueryResult * FQuery(const char * QueryString, DatabaseConnection * con);
	virtual bool FExecute(const char* QueryString, DatabaseConnection * con);
	virtual bool Execute(const char* QueryString, ...);
	virtual bool ExecuteNA(const char* QueryString);

	GE_INLINE const string& GetHostName() { return mHostname; }
	GE_INLINE const string& GetDatabaseName() { return mDatabaseName; }

	virtual string EscapeString(string Escape) = 0;
	virtual void EscapeLongString(const char * str, uint32 len, stringstream& out) = 0;
	virtual string EscapeString(const char * esc, DatabaseConnection * con) = 0;

	DatabaseConnection * GetFreeConnection();
	// 获得指定id的连接，如果超出，则自动取模
	DatabaseConnection * GetConnection(int id);

	static Database * CreateDatabaseInterface(uint32 uType);
	static void CleanupLibs();
protected:

	virtual void _BeginTransaction(DatabaseConnection * conn) = 0;
	virtual void _EndTransaction(DatabaseConnection * conn) = 0;

	// actual query function
	virtual bool _SendQuery(DatabaseConnection *con, const char* Sql, bool Self) = 0;
	virtual QueryResult * _StoreQueryResult(DatabaseConnection * con) = 0;

	////////////////////////////////
	
	////////////////////////////////
	DatabaseConnection ** Connections;
	int32 mConnectionCount;
	int32 mBusyCount = 0;

	// For reconnecting a broken connection
	string mHostname;
	string mUsername;
	string mPassword;
	string mDatabaseName;
	uint32 mPort = 0;
};

class SERVER_DECL QueryResult
{
	friend class DBResult;
public:
	QueryResult(uint32 fields, uint32 rows) : mFieldCount(fields), mRowCount(rows), mCurrentRow(NULL) {}
	virtual ~QueryResult() {}

	virtual bool NextRow() = 0;
	void Delete() {	delete this; }

	GE_INLINE Field* Fetch() { return mCurrentRow; }
	GE_INLINE uint32 GetFieldCount() const { return mFieldCount; }
	GE_INLINE uint32 GetRowCount() const { return mRowCount; }

protected:
	uint32 mFieldCount;
	uint32 mRowCount;
	Field *mCurrentRow;
};

class SERVER_DECL DBResult
{
public:
	DBResult();
	DBResult(QueryResult* r);
	~DBResult();
	void Release();

//	bool hasResult() { return (pResult != NULL);};
	DBResult& operator=(QueryResult* r);
	DBResult& operator=(DBResult& r);

//	GE_INLINE Field* Fetch() { return pResult->mCurrentRow; }
	GE_INLINE DBResult& Fetch() { return *this; }	//兼容老的写法
	GE_INLINE uint32 GetFieldCount() const { return (pResult != NULL ? pResult->mFieldCount : 0); }
	GE_INLINE uint32 GetRowCount() const { return (pResult != NULL ? pResult->mRowCount : 0); }
	GE_INLINE bool NextRow()
	{
		return (pResult != NULL ? pResult->NextRow() : false);
	}

	Field& operator[](uint32 i)
	{
		if(i >= GetFieldCount())
		{
			LOG(ERROR)<<"数据库获取结果超出范围 index:"<<i<<" fieldCount:"<<GetFieldCount()<<endl;
			ASSERT(NULL);
		}
		return pResult->mCurrentRow[i];
	}

private:
	QueryResult* pResult;
};

#endif /* SRC_MODULE_GEDATABASE_DATABASE_H_ */
