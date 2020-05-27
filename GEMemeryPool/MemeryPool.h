/*
 * MemeryPool.h
 *
 *  Created on: 2018年1月10日
 *      Author: carlos Hu
 */

#ifndef LOGSERVER_MEMERYPOOL_H_
#define LOGSERVER_MEMERYPOOL_H_

#include "common/Errors.h"
#include "common/Mutex.h"
#include <glog/logging.h>
#include <list>
#include <typeinfo>

/**
 * 频繁的 new delete 将会导致内存碎片过多，降低性能
 * 	需要使用内存池
 */
template<class T>
class MemeryPool
{
	typedef T* (*CreateFun)();
public:
	MemeryPool(uint32 page, uint32 pageOneNum, CreateFun fun=NULL);
	//获得一个可用对象
	T* GetNew();
	//回收一个对象
	void Recy(T* tp);
	int32 GetTotalCount() { return nowPage * pageOneNum;}

private:
	void Expansion();
private:
	uint32 maxPage;
	uint32 nowPage;
	uint32 pageOneNum;
	uint32 totalCount;	//累计对象数量
	std::list<T*> avialPool;	//可用对象池
	Mutex lock;
	CreateFun createFun;

#ifdef TEST_MEMERY
public:
	int OutCount;
#endif
};

template<class T>
MemeryPool<T>::MemeryPool(uint32 tpage, uint32 tpageOneNum, CreateFun fun):
maxPage(tpage),
nowPage(0),
pageOneNum(tpageOneNum),
totalCount(0)
{
#ifdef TEST_MEMERY
	OutCount = 0;
#endif
	createFun = fun;
	this->Expansion();
}

template<class T>
void MemeryPool<T>::Expansion()
{
	if(nowPage >= maxPage)
	{
		char info[1024];
		sprintf(info, "page out! nowPage=%u, maxPage=%u, totalCount=%u, type=%s", nowPage, maxPage, totalCount, typeid(T).name());
		LOG(WARNING) << info << endl;
//		sLog.Warning("MemeryPool", "page out! nowPage=%u, maxPage=%u, totalCount=%u, type=%s", nowPage, maxPage, totalCount, typeid(T).name());
	}

//	LOG(DEBUG) << "扩展" << endl;
	nowPage++;
	for (uint32 i = 0; i < pageOneNum; ++i)
	{
		T* tp = NULL;
		if(createFun != NULL){
			tp = createFun();
		}else{
			tp = new T();
		}
		ASSERT(tp);
		totalCount++;
		avialPool.push_back(tp);
	}
}

template<class T>
T* MemeryPool<T>::GetNew()
{
	lock.Acquire();	//LOCK
	if(avialPool.size() <= 0)
	{
		this->Expansion();
	}
//	ASSERT(avialPool.size() > 0);

#ifdef TEST_MEMERY
	OutCount++;
#endif

	T* tp = avialPool.back();
	avialPool.pop_back();
	lock.Release();	//UNLOCK

	ASSERT(tp);
//	DLOG(INFO)<<"[MemeryPool] GetNew: "<<tp<<endl;
	return tp;
}

template<class T>
void MemeryPool<T>::Recy(T* tp)
{
#ifdef TEST_MEMERY
	OutCount--;
#endif

//	DLOG(INFO)<<"[MemeryPool] Recy: "<<tp<<endl;
	ASSERT(tp);
	lock.Acquire();	//LOCK
	avialPool.push_back(tp);
	lock.Release();	//UNLOCK
}

template<class T>
class FMemeryPool
{
	typedef T* (*CreateFun)();
public:
	FMemeryPool(uint32 page, uint32 pageOneNum, CreateFun fun=NULL):mPool(page,pageOneNum, fun) {}

	//获得一个可用对象
	T* GetNew()
	{
		T* t = mPool.GetNew();
		t->Reset();
		return t;
	}

	//回收一个对象
	void Recy(T* tp)
	{
		mPool.Recy(tp);
	}

	int32 GetTotalCount() { return mPool.GetTotalCount();}
	int GetOutCount() { return mPool.OutCount; }

private:
	MemeryPool<T> mPool;
};

#endif /* LOGSERVER_MEMERYPOOL_H_ */
