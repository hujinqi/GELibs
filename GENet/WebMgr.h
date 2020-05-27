/*
 * Web.h
 *
 *  Created on: 2018年3月13日
 *      Author: carlos Hu
 */

#ifndef SRC_MODULE_GENET_WEBMGR_H_
#define SRC_MODULE_GENET_WEBMGR_H_

#include "GECrow.h"
#include "GEThread/ThreadStarter.h"
#include "common/Singleton.h"

using namespace crow;

class WebThread : public ThreadBase
{
	friend class WebMgr;
public:
	GE_INLINE SimpleApp& GetWebInstance() { return web; }
	void SetPort(int port) { this->port = port;}
	void SetIp(const string& ip) { this->ip = ip; }
	bool run();
private:
	SimpleApp web;
	int port;
	string ip;
};

class WebMgr : public Singleton<WebMgr>
{
public:
	WebMgr();
	GE_INLINE SimpleApp& GetWebInstance() { ASSERT(worker); return worker->GetWebInstance(); }
	void RunbyPort(int port);	//在全ip的port端口启动
	void Run(const string& ip, int port);
	void Shutdown();

private:
	WebThread* worker;
};

#define sWeb WebMgr::getSingleton()

#endif /* SRC_MODULE_GENET_WEBMGR_H_ */
