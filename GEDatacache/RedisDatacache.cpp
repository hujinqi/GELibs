/*
 * RedisDatacache.cpp
 *
 *  Created on: 2018年3月19日
 *      Author: carlos Hu
 */

#include "RedisDatacache.h"
#include "common/StringFormat.h"
#include <type_traits>
//extern "C"
//{
//#include <string.h>
//}

using std::string;

RedisDatacache::RedisDatacache()
{

}

RedisDatacache::~RedisDatacache()
{
	for (int i = 0; i < mConnectionCount; ++i)
	{
		redisFree(((RedisConnection*) Connections[i])->redis);
		delete Connections[i];
	}
	delete[] Connections;
}

bool RedisDatacache::Initialize(std::string host, int port, std::string passwd,
		uint32 ConnectionCount)
{
	RedisConnection ** conns;
	redisContext* redis;

	mConnectionCount = ConnectionCount;
	conns = new RedisConnection*[mConnectionCount];
	Connections = ((DatacacheConnection**) conns);
	for (int i = 0; i < mConnectionCount; ++i)
	{
		redis = redisConnect(host.c_str(), port);
		if (redis != NULL && redis->err)
		{
			LOG(ERROR) << "cononection redis failed: " << redis->err
					<< endl;
			return false;
		}

		char cmd[1024];
		sprintf(cmd, "auth %s", passwd.c_str());
		redisReply* reply = (redisReply*) redisCommand(redis, cmd);
		if (strcmp(reply->str, "OK"))
		{
			LOG(ERROR) << "redis auth failed" << endl;
			freeReplyObject(reply);
			return false;
		}
		freeReplyObject(reply);
		conns[i] = new RedisConnection;
		conns[i]->redis = redis;
	}

	Datacache::_Initialize();
	return true;
}

bool RedisDatacache::_SendQuery(DatacacheConnection *con, const char* query,
		bool Self)
{
//	LOG(DEBUG) << "get redis query" << endl;
	if(((RedisConnection*) con)->reply)
	{
		freeReplyObject( ((RedisConnection*) con)->reply );
		((RedisConnection*) con)->reply = NULL;
	}

	redisReply* reply = (redisReply*) redisCommand(
			((RedisConnection*) con)->redis, query);
	bool succ = (reply != NULL ? true : false);
	if (!succ)
	{
		freeReplyObject(reply);
		if (Self == false)
		{
			succ = _SendQuery(con, query, true);
		}
		else
		{
			LOG(ERROR) << "Execute query failed. quety = " << query << endl;
		}
	}
	else
	{
		((RedisConnection*) con)->reply = reply;
	}
	return succ;
}

bool RedisDatacache::_HashMapSet(std::string key, std::string field,
		std::string value)
{
	string cmd = "HSET ";
	cmd.append(key);
	cmd.append(" ");
	cmd.append(field);
	cmd.append(" ");
	cmd.append(value);

	_queryCmd(cmd.c_str());
	return true;
}

void RedisDatacache::_queryCmd(const char* cmd)
{
	size_t len = strlen(cmd);
	char * pCmd = new char[len + 1];
	memcpy(pCmd, cmd, len + 1);

	taskQueue.push(pCmd);
}

bool RedisDatacache::HSet(std::string key, std::string field, int8 value)
{
	string vstr = StringFormat::NumberToString(value);
//	field.append("_int8");
	return _HashMapSet(key, field, vstr);
}

bool RedisDatacache::HSet(std::string key, std::string field, uint8 value)
{
	string vstr = StringFormat::NumberToString(value);
//	field.append("_uint8");
	return _HashMapSet(key, field, vstr);
}

bool RedisDatacache::HSet(std::string key, std::string field, int16 value)
{
	string vstr = StringFormat::NumberToString(value);
//	field.append("_int16");
	return _HashMapSet(key, field, vstr);
}

bool RedisDatacache::HSet(std::string key, std::string field, uint16 value)
{
	string vstr = StringFormat::NumberToString(value);
//	field.append("_uint16");
	return _HashMapSet(key, field, vstr);
}

bool RedisDatacache::HSet(std::string key, std::string field, int32 value)
{
	string vstr = StringFormat::NumberToString(value);
//	field.append("_int32");
	return _HashMapSet(key, field, vstr);
}

bool RedisDatacache::HSet(std::string key, std::string field, uint32 value)
{
	string vstr = StringFormat::NumberToString(value);
//	field.append("_uint32");
	return _HashMapSet(key, field, vstr);
}

bool RedisDatacache::HSet(std::string key, std::string field, int64 value)
{
	string vstr = StringFormat::NumberToString(value);
//	field.append("_int64");
	return _HashMapSet(key, field, vstr);
}

bool RedisDatacache::HSet(std::string key, std::string field, uint64 value)
{
	string vstr = StringFormat::NumberToString(value);
//	field.append("_uint64");
	return _HashMapSet(key, field, vstr);
}

bool RedisDatacache::HSet(std::string key, std::string field, float value)
{
	string vstr = StringFormat::NumberToString(value);
//	field.append("_float");
	return _HashMapSet(key, field, vstr);
}

bool RedisDatacache::HSet(std::string key, std::string field, double value)
{
	string vstr = StringFormat::NumberToString(value);
//	field.append("_double");
	return _HashMapSet(key, field, vstr);
}

bool RedisDatacache::HSet(std::string key, std::string field, std::string value)
{
//	field.append("_string");
	string sv = "'";
	sv.append(value);
	sv.append("'");
	return _HashMapSet(key, field, sv);
}

bool RedisDatacache::HGetAll(std::string key, std::map<std::string, std::string>& dataMap)
{
	char cmd[1024];
	sprintf(cmd, "HGETALL %s", key.c_str());
	DatacacheConnection * con = GetFreeConnection();
	if(_SendQuery(con, cmd, false))
	{
		RedisConnection* redisConn = (RedisConnection*)con;
		if(redisConn->reply->type == REDIS_REPLY_ARRAY)
		{
			for(size_t i=0; i < redisConn->reply->elements; i=i+2)
			{
				string key = redisConn->reply->element[i]->str;
				string value = redisConn->reply->element[i+1]->str;
				dataMap[key] = value;
			}
		}
		else if(redisConn->reply->type == REDIS_REPLY_NIL)
		{
			LOG(WARN) << "try get nil value. cmd:" << cmd << endl;
		}
		else
		{
			LOG(ERROR) << "try get hash value, but reply type error. type:" << redisConn->reply->type << " cmd: "<< cmd <<endl;
			con->Busy.Release();
			return false;
		}
	}
	con->Busy.Release();
	return true;
}

std::string RedisDatacache::HGet(std::string key, std::string field)
{
	string result;
	char cmd[1024];
	sprintf(cmd, "HGET %s %s", key.c_str(), field.c_str());
	DatacacheConnection * con = GetFreeConnection();
	if(_SendQuery(con, cmd, false))
	{
		RedisConnection* redisConn = (RedisConnection*)con;
		if(redisConn->reply->type == REDIS_REPLY_STRING)
		{
			result = redisConn->reply->str;
		}
		else if(redisConn->reply->type == REDIS_REPLY_NIL)
		{
			LOG(WARN) << "try get nil value. cmd:" << cmd << endl;
		}
		else
		{
			LOG(ERROR) << "try get hash-field value, but reply type error. type:" << redisConn->reply->type << endl;
		}
	}
	con->Busy.Release();
	return result;
}

bool RedisDatacache::LLPush(std::string key, std::string value)
{
	string cmd = "LPUSH ";
	cmd.append(key);
	cmd.append(" ");
	cmd.append(value);

	_queryCmd(cmd.c_str());
	return true;
}

bool RedisDatacache::LRPush(std::string key, std::string value)
{
	string cmd = "RPUSH ";
	cmd.append(key);
	cmd.append(" ");
	cmd.append(value);

	_queryCmd(cmd.c_str());
	return true;
}

std::string RedisDatacache::LLPop(std::string key)
{
	string result;
	char cmd[1024];
	sprintf(cmd, "LPOP %s", key.c_str());
	DatacacheConnection * con = GetFreeConnection();
	if (_SendQuery(con, cmd, false))
	{
		RedisConnection* redisConn = (RedisConnection*) con;
		if (redisConn->reply->type == REDIS_REPLY_STRING)
		{
			result = redisConn->reply->str;
		}
		else if (redisConn->reply->type == REDIS_REPLY_NIL)
		{
//			LOG(WARN) << "try get nil value. cmd:" << cmd << endl;
			//也许空的，没有数据了，什么也不做
		}
		else
		{
			LOG(ERROR) << "try get list value, but reply type error. type:"
					<< redisConn->reply->type << endl;
		}
	}
	con->Busy.Release();
	return result;
}

std::string RedisDatacache::LRPop(std::string key)
{
	string result;
	char cmd[1024];
	sprintf(cmd, "RPOP %s", key.c_str());
	DatacacheConnection * con = GetFreeConnection();
	if (_SendQuery(con, cmd, false))
	{
		RedisConnection* redisConn = (RedisConnection*) con;
		if (redisConn->reply->type == REDIS_REPLY_STRING)
		{
			result = redisConn->reply->str;
		}
		else if (redisConn->reply->type == REDIS_REPLY_NIL)
		{
//			LOG(WARN) << "try get nil value. cmd:" << cmd << endl;
			//也许空的，没有数据了，什么也不做
		}
		else
		{
			LOG(ERROR) << "try get list value, but reply type error. type:"
					<< redisConn->reply->type << endl;
		}
	}
	con->Busy.Release();
	return result;
}

std::string RedisDatacache::DecodeString(std::string value)
{
//	LOG(DEBUG) << "value: " << value << " size: " << value.size() << endl;
	if(value.size() < 2 || value.at(0) != '\'')
	{
		return value;
	}

	string v = value.substr(1, value.size()-2);
	return  v;
}

bool RedisDatacache::Del(std::string key)
{
	string cmd = "delete ";
	cmd.append(key);

	_queryCmd(cmd.c_str());
	return true;
}
