/*
 * GarbageCollector.h
 *
 *  Created on: 2018年3月28日
 *      Author: carlos Hu
 */

#ifndef SRC_MODULE_COMMON_GARBAGECOLLECTOR_H_
#define SRC_MODULE_COMMON_GARBAGECOLLECTOR_H_

#include "Mutex.h"
#include <map>
#include <time.h>

template<class T>
class GarbageCollector
{
	time_t later_sec;
	std::map<T*, time_t> deletionQueue;
	Mutex lock;
public:
	GarbageCollector(int lateSec)
	{
		later_sec = lateSec;
	}
	~GarbageCollector()
	{
		typename std::map<T*, time_t>::iterator i;
		for(i=deletionQueue.begin();i!=deletionQueue.end();++i)
			delete i->first;
	}

	void Update(time_t now)
	{
		typename std::map<T*, time_t>::iterator i, i2;
		lock.Acquire();
		for(i = deletionQueue.begin(); i != deletionQueue.end();)
		{
			i2 = i++;
			if(i2->second <= now)
			{
				delete i2->first;
				deletionQueue.erase(i2);
			}
		}
		lock.Release();
	}

	void Queue(T * s, time_t now)
	{
		typename std::map<T*, time_t>::value_type vt(s, now + later_sec);
		lock.Acquire();
		deletionQueue.insert( vt );
		lock.Release();
	}
};



#endif /* SRC_MODULE_COMMON_GARBAGECOLLECTOR_H_ */
