/*
 * TimeMgr.h
 *	时间相关接口，由于获得系统时间开销较大，单独一个线程维护时间，加快访问速度
 *
 *  Created on: 2018年3月6日
 *      Author: carlos Hu
 */

#ifndef SRC_MODULE_TIMEMGR_TIMEMGR_H_
#define SRC_MODULE_TIMEMGR_TIMEMGR_H_

#include "common/Singleton.h"
#include <sys/time.h>

enum WeekEnum
{
	WEEK_ENUM_MON = 1,		//星期一
	WEEK_ENUM_TUES = 2,		//星期二
	WEEK_ENUM_WED = 3,		//星期三
	WEEK_ENUM_THUR = 4,		//星期四
	WEEK_ENUM_FRI = 5,		//星期五
	WEEK_ENUM_SAT = 6,		//星期六
	WEEK_ENUM_SUN = 7,		//星期日
};

class SERVER_DECL TimeMgr: public Singleton<TimeMgr>
{
public:
	/*
	 * 获得当前时间（秒）
	 */
	GE_INLINE const time_t& Time()
	{
		return now_time;
	}

	/*
	 * 获得当前时间（毫秒）
	 * 		效率不如秒
	 */
	GE_INLINE uint64 MSTime()
	{
		struct timeval tv;
		gettimeofday(&tv, NULL);
		return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
	}

	/*
	 * 获得当前结构化的时间
	 */
	GE_INLINE const tm& GetLocalTime()
	{
		return this->now_localTime;
	}

	/**
	 * 获得今天0点的时间戳
	 */
	GE_INLINE time_t GetTodayZeroUT()
	{
		return todayZeroTime;
	}

	/**
	 * 获得指定时间当天0点的时间戳
	 */
	time_t GetOneDayZeroUT(time_t time);
	/**
	 * 根据日期字符串（y/m/d）获得当日0点的时间戳
	 */
	time_t GetUTByDayStr(std::string dayStr);
	/**
	 * 根据完整时间字符串（y/m/d-h:m:s）获得时间戳
	 */
	time_t GetUTByFullTimeStr(std::string fullTimeStr, std::string strReason);
	/**
	 * 根据完整时间字符串（2019-02-15 14:33:58）获得时间戳
	 */
	time_t GetUTByMysqlTimeStr(std::string fullTimeStr, std::string strReason = "0");
	/**
	 * 根据时间的日期，获取时间戳
	 */
	time_t GetTimeToEpoch(int year, int month, int day, int hour, int min, int sec);
	/**
	 * 根据时间字符串（h:m:s）获得今日的时间戳
	 */
	time_t GetUTByTodayTimeStr(std::string timeStr);
	/**
	 * 获得今天规定时间的时间戳
	 */
	time_t TODAYTIME(uint32 hour, uint32 min, uint32 sec);
	/**
	 * 获得时间完整形式（y/m/d-h:m:s）
	 */
	std::string UTToDateStr(time_t time);
	/**
	 * 获得时间完整形式（y/m/d）
	 */
	std::string UTToDayStr(time_t time);
	/**
	 * 获得今天星期几
	 */
	WeekEnum GetTodayWeek();
	/**
	 * 获得时间戳的时间点星期几
	 */
	WeekEnum GetOneDayWeek(time_t time);
	/**
	 * 获得时间日期形式不加下划线
	 */
	string TimeToDateStr2( time_t time );
	/**
	 * 获得时间的日期形式
	 */
	long TimeToDateStrLong( time_t time );
	long TimeToDateStrLong();
	/**
	 * 获得日期 20180418
	 */
	int GetTimeDate( time_t time );
	/**
	 * 获得今日日期 20180418
	 */
	int GetNowData();

	TimeMgr();
	~TimeMgr();

	GE_INLINE time_t GetChangeSec()
	{
		return changeSec;
	}
	GE_INLINE void SetChangeSec(time_t time)
	{
		changeSec = time;
	}
	GE_INLINE void _SetTime(time_t time)
	{
		now_time = time;
	}
	GE_INLINE void _SetTM(tm t)
	{
		now_localTime = t;
	}
	GE_INLINE bool _IsShutDown()
	{
		return _shutdown;
	}
	GE_INLINE void _ShutDown()
	{
		_shutdown = true;
	}
	GE_INLINE void _SetTodayZeroTime(time_t time)
	{
		todayZeroTime = time;
	}
private:
	bool _shutdown;
	pthread_t tid;
	time_t now_time;	//当前系统时间
	time_t changeSec;	//修改的时间
	tm now_localTime;	//结构化时间
	time_t todayZeroTime;	//今日的0点时间戳
};

#define sTime TimeMgr::getSingleton()

#endif /* SRC_MODULE_TIMEMGR_TIMEMGR_H_ */
