/*
 * Web.cpp
 *
 *  Created on: 2018年3月13日
 *      Author: carlos Hu
 */



#include "WebMgr.h"
#include "GEThread/ThreadPool.h"
#include <glog/logging.h>

bool WebThread::run()
{
	if(ip != ""){
		web.bindaddr(ip).port(port).multithreaded().run();
	}else{
		web.port(port).multithreaded().run();
	}
	return true;
}

initialiseSingleton(WebMgr);

WebMgr::WebMgr()
{
	worker = new WebThread();
}

void WebMgr::Shutdown()
{
	worker->GetWebInstance().stop();
	worker = NULL;
	delete this;
}

void WebMgr::RunbyPort(int port)
{
	worker->SetPort(port);
	ThreadPool.ExecuteTask( worker );
}

void WebMgr::Run(const string& ip, int port)
{
	worker->SetIp(ip);
	worker->SetPort(port);
	ThreadPool.ExecuteTask( worker );
}
