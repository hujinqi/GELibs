/*
 * Datacache.h
 *
 *  Created on: 2018年3月19日
 *      Author: carlos Hu
 */

#ifndef SRC_MODULE_GEDATACACHE_DATACACHE_H_
#define SRC_MODULE_GEDATACACHE_DATACACHE_H_

#include "common/defines.h"
#include "GEThread/CThreads.h"
#include "common/Queue.h"
#include <string>
#include <map>


struct DatacacheConnection
{
	FastMutex Busy;
};

class SERVER_DECL Datacache : public CThread
{
public:
	bool run();
	virtual bool Initialize(std::string host, int port, std::string passwd, uint32 ConnectionCount) = 0;

	virtual bool Del(std::string key) = 0;	//删除

	/**
	 * hash map
	 */
	virtual bool HGetAll(std::string key, std::map<std::string, std::string>& dataMap) = 0;
	virtual std::string HGet(std::string key, std::string field) = 0;
	virtual bool _SendQuery(DatacacheConnection *con, const char* query, bool Self) = 0;
	virtual bool HSet(std::string key, std::string field, int8 value) = 0;
	virtual bool HSet(std::string key, std::string field, uint8 value) = 0;
	virtual bool HSet(std::string key, std::string field, int16 value) = 0;
	virtual bool HSet(std::string key, std::string field, uint16 value) = 0;
	virtual bool HSet(std::string key, std::string field, int32 value) = 0;
	virtual bool HSet(std::string key, std::string field, uint32 value) = 0;
	virtual bool HSet(std::string key, std::string field, int64 value) = 0;
	virtual bool HSet(std::string key, std::string field, uint64 value) = 0;
	virtual bool HSet(std::string key, std::string field, float value) = 0;
	virtual bool HSet(std::string key, std::string field, double value) = 0;
	virtual bool HSet(std::string key, std::string field, std::string value) = 0;

	/**
	 * list
	 */
	virtual bool LLPush(std::string key, std::string value) = 0;
	virtual bool LRPush(std::string key, std::string value) = 0;
	virtual std::string LLPop(std::string key) = 0;
	virtual std::string LRPop(std::string key) = 0;

	DatacacheConnection * GetFreeConnection();
	Datacache();
	virtual ~Datacache();
protected:
	void _Initialize();
protected:
	FQueue<char*> taskQueue;
	DatacacheConnection ** Connections;
	int32 mConnectionCount;
};



#endif /* SRC_MODULE_GEDATACACHE_DATACACHE_H_ */
