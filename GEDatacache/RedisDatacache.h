/*
 * RedisDatacache.h
 *
 *  Created on: 2018年3月19日
 *      Author: carlos Hu
 */

#ifndef SRC_MODULE_GEDATACACHE_REDISDATACACHE_H_
#define SRC_MODULE_GEDATACACHE_REDISDATACACHE_H_

#include "Datacache.h"
extern "C"
{
#include <hiredis/hiredis.h>
#include <hiredis/async.h>
}
#include <map>

struct RedisConnection : public DatacacheConnection
{
	RedisConnection():redis(NULL),reply(NULL) {}

	redisContext* redis;
	redisReply* reply;
};

class RedisDatacache : public Datacache
{
public:
	RedisDatacache();
	~RedisDatacache();
	bool Initialize(std::string host, int port, std::string passwd, uint32 ConnectionCount);
	bool _SendQuery(DatacacheConnection *con, const char* query, bool Self = false);
	std::string DecodeString(std::string value);

	bool Del(std::string key);

	bool HGetAll(std::string key, std::map<std::string, std::string>& dataMap);
	std::string HGet(std::string key, std::string field);
	bool HSet(std::string key, std::string field, int8 value);
	bool HSet(std::string key, std::string field, uint8 value);
	bool HSet(std::string key, std::string field, int16 value);
	bool HSet(std::string key, std::string field, uint16 value);
	bool HSet(std::string key, std::string field, int32 value);
	bool HSet(std::string key, std::string field, uint32 value);
	bool HSet(std::string key, std::string field, int64 value);
	bool HSet(std::string key, std::string field, uint64 value);
	bool HSet(std::string key, std::string field, float value);
	bool HSet(std::string key, std::string field, double value);
	bool HSet(std::string key, std::string field, std::string value);

	bool LLPush(std::string key, std::string value);
	bool LRPush(std::string key, std::string value);
	std::string LLPop(std::string key);
	std::string LRPop(std::string key);

private:
	GE_INLINE void _queryCmd(const char* cmd);
	GE_INLINE bool _HashMapSet(std::string key, std::string field, std::string value);
private:

};

#endif /* SRC_MODULE_GEDATACACHE_REDISDATACACHE_H_ */
