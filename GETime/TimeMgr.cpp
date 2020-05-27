/*
 * TimeMgr.cpp
 *
 *  Created on: 2018年3月6日
 *      Author: carlos Hu
 */

#include "TimeMgr.h"
#include "common/StringFormat.h"
#include <pthread.h>
#include <vector>
#include "../common/MyUtils.h"

createFileSingleton(TimeMgr);

void *update(void *args);
TimeMgr::TimeMgr()
{
	this->now_time = time(NULL);
	this->changeSec = 0;
	this->now_localTime = *localtime(&this->now_time);
	todayZeroTime = 0;

	_shutdown = false;
	if (pthread_create(&this->tid, NULL, update, NULL) != 0)
	{
		//error
		abort();
	}
}

time_t _GetTodayZeroUT(tm now_localTime)
{
	struct tm timeinfo(now_localTime);
	timeinfo.tm_hour = 0;
	timeinfo.tm_min = 0;
	timeinfo.tm_sec = 0;
	return mktime(&timeinfo);
}

TimeMgr::~TimeMgr()
{
	_ShutDown();
	pthread_join(this->tid, NULL);
}

void *update(void *args)
{
	time_t begin = clock();
	time_t end = clock();
	time_t etime = end - begin;

	//main loop of time control ( 5 times / sec)
	while (!sTime._IsShutDown() )
	{
		begin = clock();

		//update global time
		sTime._SetTime( time(NULL) + sTime.GetChangeSec());
		tm m = *localtime(&sTime.Time());
		sTime._SetTM( m );
		sTime._SetTodayZeroTime(_GetTodayZeroUT(m));

		end = clock();
		etime = end - begin;
		if( 200000 > etime )
		{
			usleep(200000 - etime);
		}
	}
	pthread_exit(NULL);
}

time_t TimeMgr::GetOneDayZeroUT(time_t oneUT)
{
	time_t oneUT_t = oneUT;
	tm oneUT_tm = *localtime(&oneUT_t);
	oneUT_tm.tm_hour = 0;
	oneUT_tm.tm_min = 0;
	oneUT_tm.tm_sec = 0;
	return mktime(&oneUT_tm);
}

//根据日期字符串（y/m/d）获得当日0点的时间戳
time_t TimeMgr::GetUTByDayStr(std::string dayStr)
{
	std::vector < std::string > strVec_day = StringFormat::Split(dayStr, "/");
	if (strVec_day.size() != 3)
	{
//		ERROR_LOG("GetUTByDayStr dayStr format error! dayStr=%s",
//				dayStr.c_str());
		return 0;
	}
	int year = StringFormat::StringToNumber<int> (strVec_day.at(0));
	int mon = StringFormat::StringToNumber<int>(strVec_day.at(1));
	int day = StringFormat::StringToNumber<int>(strVec_day.at(2));
	return GetTimeToEpoch(year, mon, day, 0, 0, 0);
}

time_t TimeMgr::GetUTByFullTimeStr(std::string fullTimeStr, std::string strReason)
{
	if (fullTimeStr.empty() || fullTimeStr == "0")
	{
		return 0;
	}

	std::vector < std::string > strVec_1 = StringFormat::Split(fullTimeStr, "-");
	if (strVec_1.size() != 2)
	{
//		ERROR_LOG(
//				"GetUTByFullTimeStr fullTimeStr1 format error! fullTimeStr=%s, strReason = %s",
//				fullTimeStr.c_str(), strReason.c_str());
		return 0;
	}
	std::vector < std::string > strVec_day = StringFormat::Split(strVec_1.at(0), "/");
	if (strVec_day.size() != 3)
	{
//		ERROR_LOG(
//				"GetUTByFullTimeStr fullTimeStr2 format error! fullTimeStr=%s, strReason = %s",
//				fullTimeStr.c_str(), strReason.c_str());
		return 0;
	}
	std::vector < std::string > strVec_time = StringFormat::Split(strVec_1.at(1), ":");
	if (strVec_time.size() != 3)
	{
//		ERROR_LOG(
//				"GetUTByFullTimeStr fullTimeStr3 format error! fullTimeStr=%s, strReason = %s",
//				fullTimeStr.c_str(), strReason.c_str());
		return 0;
	}
	int year = StringFormat::StringToNumber<int>(strVec_day.at(0));
	int month = StringFormat::StringToNumber<int>(strVec_day.at(1));
	int day = StringFormat::StringToNumber<int>(strVec_day.at(2));
	int hour = StringFormat::StringToNumber<int>(strVec_time.at(0));
	int min = StringFormat::StringToNumber<int>(strVec_time.at(1));
	int sec = StringFormat::StringToNumber<int>(strVec_time.at(2));

	return GetTimeToEpoch(year, month, day, hour, min, sec);
}

//根据完整时间字符串（2019-02-15 14:33:58）获得时间戳
time_t TimeMgr::GetUTByMysqlTimeStr(std::string fullTimeStr, std::string strReason)
{
	if (fullTimeStr.empty() || fullTimeStr == "0")
	{
		return 0;
	}

	std::vector < std::string > strVec_1 = StringFormat::Split(fullTimeStr, " ");
	if (strVec_1.size() != 2)
	{
//		ERROR_LOG(
//				"GetUTByFullTimeStr fullTimeStr1 format error! fullTimeStr=%s, strReason = %s",
//				fullTimeStr.c_str(), strReason.c_str());
		return 0;
	}
	std::vector < std::string > strVec_day = StringFormat::Split(strVec_1.at(0), "-");
	if (strVec_day.size() != 3)
	{
//		ERROR_LOG(
//				"GetUTByFullTimeStr fullTimeStr2 format error! fullTimeStr=%s, strReason = %s",
//				fullTimeStr.c_str(), strReason.c_str());
		return 0;
	}
	std::vector < std::string > strVec_time = StringFormat::Split(strVec_1.at(1), ":");
	if (strVec_time.size() != 3)
	{
//		ERROR_LOG(
//				"GetUTByFullTimeStr fullTimeStr3 format error! fullTimeStr=%s, strReason = %s",
//				fullTimeStr.c_str(), strReason.c_str());
		return 0;
	}
	int year = StringFormat::StringToNumber<int>(strVec_day.at(0));
	int month = StringFormat::StringToNumber<int>(strVec_day.at(1));
	int day = StringFormat::StringToNumber<int>(strVec_day.at(2));
	int hour = StringFormat::StringToNumber<int>(strVec_time.at(0));
	int min = StringFormat::StringToNumber<int>(strVec_time.at(1));
	int sec = StringFormat::StringToNumber<int>(strVec_time.at(2));

	return GetTimeToEpoch(year, month, day, hour, min, sec);
}


//根据时间的日期，获取时间戳
time_t TimeMgr::GetTimeToEpoch(int year, int month, int day, int hour, int min,
		int sec)
{
	time_t t;
	time(&t);
	t += changeSec;		//偏移
	struct tm * timeinfo;
	timeinfo = localtime(&t);
	timeinfo->tm_year = year - 1900;
	timeinfo->tm_mon = month - 1;
	timeinfo->tm_mday = day;
	timeinfo->tm_hour = hour;
	timeinfo->tm_min = min;
	timeinfo->tm_sec = sec;
	return mktime(timeinfo);
}

time_t TimeMgr::GetUTByTodayTimeStr(std::string timeStr)
{
	std::vector < std::string > strVec_time = StringFormat::Split(timeStr, ":");
	if (strVec_time.size() != 3)
	{
//		ERROR_LOG("GetUTByTodayTimeStr timeStr format error! timeStr=%s",
//				timeStr.c_str());
		return 0;
	}
	int hour = StringFormat::StringToNumber<int>(strVec_time.at(0));
	int min = StringFormat::StringToNumber<int>(strVec_time.at(1));
	int sec = StringFormat::StringToNumber<int>(strVec_time.at(2));
	return TODAYTIME(hour, min, sec);
}

//获得今天规定时间的时间戳
time_t TimeMgr::TODAYTIME(uint32 hour, uint32 min, uint32 sec)
{
	if (hour > 23)
	{
		hour = 23;
	}
	if (min > 59)
	{
		min = 59;
	}
	if (sec > 59)
	{
		sec = 59;
	}
	time_t t;
	time(&t);
	t += changeSec;		//偏移
	struct tm * timeinfo;
	timeinfo = localtime(&t);
	timeinfo->tm_hour = hour;
	timeinfo->tm_min = min;
	timeinfo->tm_sec = sec;
	return mktime(timeinfo);
}

std::string TimeMgr::UTToDateStr(time_t time)
{
	time_t t = time;
	std::stringstream ss;
	struct tm * now = localtime(&t);
	int mon = now->tm_mon + 1;
	ss << (now->tm_year + 1900);
	ss << "/";
	if (mon < 10)
	{
		ss << 0;
	}
	ss << mon;
	ss << "/";
	if (now->tm_mday < 10)
	{
		ss << 0;
	}
	ss << now->tm_mday;
	ss << "-";
	if (now->tm_hour < 10)
	{
		ss << 0;
	}
	ss << now->tm_hour;
	ss << ":";
	if (now->tm_min < 10)
	{
		ss << 0;
	}
	ss << now->tm_min;
	ss << ":";
	if (now->tm_sec < 10)
	{
		ss << 0;
	}
	ss << now->tm_sec;
	return ss.str();
}

//获得时间完整形式（y/m/）
std::string TimeMgr::UTToDayStr(time_t time)
{
	time_t t = time;
	std::stringstream ss;
	struct tm * now = localtime(&t);
	int mon = now->tm_mon + 1;
	ss << (now->tm_year + 1900);
	ss << "/";
	if (mon < 10)
	{
		ss << 0;
	}
	ss << mon;
	ss << "/";
	if (now->tm_mday < 10)
	{
		ss << 0;
	}
	ss << now->tm_mday;
	return ss.str();
}

//获得今天星期几
WeekEnum TimeMgr::GetTodayWeek()
{
	int weekNum = sTime.GetLocalTime().tm_wday;
	if( weekNum == 0 )
	return WEEK_ENUM_SUN;
	else
	return (WeekEnum)weekNum;
}

//获得时间戳的时间点星期几
WeekEnum TimeMgr::GetOneDayWeek(time_t oneUT)
{
	time_t oneUT_t = oneUT;
	tm oneUT_tm = *localtime(&oneUT_t);
	int weekNum = oneUT_tm.tm_wday;
	if (weekNum == 0)
		return WEEK_ENUM_SUN;
	else
		return (WeekEnum) weekNum;
}

//获得时间日期形式不加下划线
string TimeMgr::TimeToDateStr2( time_t time )
{
	std::stringstream ss;
	struct tm * now = localtime( &time );
	int mon = now->tm_mon + 1;
	ss << (now->tm_year + 1900);
	if( mon < 10 )
	{
		ss << 0;
	}
	ss << mon;
	if( now->tm_mday < 10 )
	{
		ss << 0;
	}
	ss << now->tm_mday;
	if( now->tm_hour < 10 )
	{
		ss << 0;
	}
	ss << now->tm_hour;
	if( now->tm_min < 10 )
	{
		ss << 0;
	}
	ss << now->tm_min;
	if( now->tm_sec < 10 )
	{
		ss << 0;
	}
	ss << now->tm_sec;
	return ss.str();
}


//获得时间的日期形式
long TimeMgr::TimeToDateStrLong( time_t time )
{
	string timeStr = TimeToDateStr2( time );
	return MyUtils::MyAtol( timeStr.c_str() );
}

long TimeMgr::TimeToDateStrLong()
{
	return TimeToDateStrLong( this->Time() );
}

int TimeMgr::GetTimeDate( time_t time )
{
	std::stringstream ss;
	struct tm * now = localtime( &time );
	return (now->tm_year + 1900) * 10000 + (now->tm_mon + 1) * 100 + now->tm_mday;
}

int TimeMgr::GetNowData()
{
	return GetTimeDate( this->Time() );
}

