/*
 * Multiplatform Async Network Library
 * Copyright (c) 2007 Burlex
 *
 * SocketMgr - epoll manager for Linux.
 *
 */

#include "Network.h"
#ifdef CONFIG_USE_EPOLL

#include "GEThread/ThreadPool.h"
#include <errno.h>
#include <glog/logging.h>

//#define ENABLE_ANTI_DOS

initialiseSingleton(SocketMgr);

void SocketMgr::AddSocket(Socket * s)
{
#ifdef ENABLE_ANTI_DOS
	uint32 saddr;
	int i, count;

	// Check how many connections we already have from that ip
	saddr = s->GetRemoteAddress().s_addr;
	for (i=0, count=0; i<=max_fd; i++)
	{
		if (fds[i])
		{
			if (fds[i]->GetRemoteAddress().s_addr == saddr) count++;
		}
	}

	// More than 16 connections from the same ip? enough! xD
	if (count > 16)
	{
		s->Disconnect(false);
		return;
	}
#endif

	if(fds[s->GetFd()] != NULL)
	{
		//fds[s->GetFd()]->Delete();
		//fds[s->GetFd()] = NULL;
		s->Delete();
		return;
	}

	if (max_fd < s->GetFd()) max_fd = s->GetFd();
    fds[s->GetFd()] = s;

    // Add epoll event based on socket activity.
    struct epoll_event ev;
    memset(&ev, 0, sizeof(epoll_event));
    ev.events = (s->GetWriteBuffer().GetSize()) ? EPOLLOUT : EPOLLIN;
    ev.events |= EPOLLET;			/* use edge-triggered instead of level-triggered because we're using nonblocking sockets */
    ev.data.fd = s->GetFd();
    
    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, ev.data.fd, &ev))
    	LOG(WARNING) << "Could not add event to epoll set on fd " << ev.data.fd << endl;
}

void SocketMgr::AddListenSocket(ListenSocketBase * s)
{
	assert(listenfds[s->GetFd()] == 0);
	listenfds[s->GetFd()] = s;

	// Add epoll event based on socket activity.
	struct epoll_event ev;
	memset(&ev, 0, sizeof(epoll_event));
	ev.events = EPOLLIN;
	ev.events |= EPOLLET;			/* use edge-triggered instead of level-triggered because we're using nonblocking sockets */
	ev.data.fd = s->GetFd();

	if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, ev.data.fd, &ev))
		LOG(WARNING) << "Could not add event to epoll set on fd " << ev.data.fd << endl;
//		sLog.Warning("epoll", "Could not add event to epoll set on fd %u", ev.data.fd);
}

void SocketMgr::RemoveSocket(Socket * s)
{
    if(fds[s->GetFd()] != s)
	{
    	LOG(WARNING) << "Could not remove fd " << s->GetFd() << " from the set due to it not existing?" << endl;
//		sLog.Warning("epoll", "Could not remove fd %u from the set due to it not existing?", s->GetFd());
    	assert(0);
    	return;
	}

	fds[s->GetFd()] = 0;

    // Remove from epoll list.
    struct epoll_event ev;
	memset(&ev, 0, sizeof(epoll_event));
    ev.data.fd = s->GetFd();
    ev.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP | EPOLLONESHOT;

    if(epoll_ctl(epoll_fd, EPOLL_CTL_DEL, ev.data.fd, &ev))
    	LOG(WARNING) << "Could not remove fd " << s->GetFd() << " from epoll set, errno " << errno << endl;
//		sLog.Warning("epoll", "Could not remove fd %u from epoll set, errno %u", s->GetFd(), errno);
}

void SocketMgr::Shutdown()
{
    for(uint32 i = 0; i < SOCKET_HOLDER_SIZE; ++i)
        if(fds[i] != NULL)
            fds[i]->Delete();

    //stop
	for(uint32 i = 0; i < workerCount; ++i){
		workers[i]->Shutdown();
		DLOG(INFO)<<"shutdown epoll worker thread. Id:"<<workers[i]->GetThreadId()<<endl;
	}
	close(epoll_fd);

    sSocketGarbageCollector.Update();

//    delete sSocketGarbageCollector.getSingletonPtr();
}

void SocketMgr::SpawnWorkerThreads()
{
	workerCount = 1;
    workers = new SocketWorkerThread*[workerCount];
    for(uint32 i = 0; i < workerCount; ++i){
    	workers[i] = new SocketWorkerThread();
        ThreadPool.ExecuteTask(workers[i]);
    }
}

void SocketMgr::ShowStatus()
{
	int i, count;

	for (i=0, count=0; i<=max_fd; i++)
	{
		if (fds[i]) count++;
	}

	DLOG(INFO) << "Sockets: " << count <<endl;
//	sLog.Debug("Sockets","Sockets: %d", count);
}

bool SocketWorkerThread::run()
{
    int fd_count;
    Socket * ptr;
    int i;
    running = true;
    SocketMgr * mgr = SocketMgr::getSingletonPtr();

    while(running)
    {
        fd_count = epoll_wait(mgr->epoll_fd, events, THREAD_EVENT_SIZE, 1000);
        for(i = 0; i < fd_count; ++i)
        {
            if(events[i].data.fd >= SOCKET_HOLDER_SIZE)
            {
            	LOG(WARNING) << "Requested FD that is too high (" << events[i].data.fd << ")" <<endl;
                continue;
            }

//            DLOG(INFO)<<"epoll event. fd:"<<events[i].data.fd<<" event:"<<events[i].events<<" errno:"<<errno<<endl;
            ptr = mgr->fds[events[i].data.fd];

            if(ptr == NULL)
            {
				if( (ptr = ((Socket*)mgr->listenfds[events[i].data.fd])) != NULL )
					((ListenSocketBase*)ptr)->OnAccept();
				else
					LOG(WARNING) << "Returned invalid fd (no pointer) of FD " << events[i].data.fd << endl;

                continue;
            }

            if((events[i].events & EPOLLHUP) || (events[i].events & EPOLLERR))
            {
//				LOG(WARNING)<<"EPOLL 异常，断开连接. fd:"<<events[i].data.fd<<" events:"<<events[i].events<<" errno:"<<errno<<endl;
            	ptr->Disconnect();
                continue;
            }
			else if(events[i].events & EPOLLIN)
            {
                ptr->ReadCallback(0);               // Len is unknown at this point.

				/* changing to written state? */
				if(ptr->GetWriteBuffer().GetSize() && !ptr->HasSendLock() && ptr->IsConnected())
					ptr->PostEvent(EPOLLOUT);
            }
			else if(events[i].events & EPOLLOUT)
            {
                ptr->BurstBegin();          // Lock receive mutex
                ptr->WriteCallback();       // Perform actual send()
                if(ptr->GetWriteBuffer().GetSize() > 0)
                {
                    /* we don't have to do anything here. no more oneshots :) */
                }
                else
                {
					/* change back to a read event */
                    ptr->DecSendLock();
                    ptr->PostEvent(EPOLLIN);
                }
                ptr->BurstEnd();            // Unlock
            }
        }       
    }
    return true;
}

#ifdef USING_SSL

bool SocketMgr::InitSSL()
{
	LOG( INFO ) << "InitSSL start";
	/* SSL 库初始化 */
	SSL_library_init();
	/* 载入所有 SSL 算法 */
	OpenSSL_add_all_algorithms();
	/* 载入所有 SSL 错误消息 */
	SSL_load_error_strings();
	/* 以 SSL V2 和 V3 标准兼容方式产生一个 SSL_CTX ，即 SSL Content Text */
	SSL_CTX *ctx = SSL_CTX_new(SSLv23_server_method());
	/* 也可以用 SSLv2_server_method() 或 SSLv3_server_method() 单独表示 V2 或 V3标准 */
	if (ctx == NULL)
	{
			ERR_print_errors_fp(stdout);
			LOG(ERROR)<< "create SSL_CTX failed."<< endl;
			exit(1);
	}
	/*加载公钥证书*/
	char pwd[256];
	if(getcwd(pwd, 256) == NULL)
			LOG(ERROR)<<"getcwd failed."<<endl;
	if (strlen(pwd) == 1)
			pwd[0] = '\0';
	char* temp = strcat(pwd, "/ssl/ssl.crt");
	if (SSL_CTX_use_certificate_file(ctx, temp,     SSL_FILETYPE_PEM) <= 0)
	{
			ERR_print_errors_fp(stdout);
			LOG(ERROR)<< "load ssl.crt failed."<< endl;
			exit(1);
	}
	/* 载入证书链 */
	if(getcwd(pwd, 256) == NULL)
		LOG(ERROR)<<"getcwd failed."<<endl;
	if (strlen(pwd) == 1)
		pwd[0] = '\0';
	temp = strcat(pwd, "/ssl/ssl.chain");
	if (SSL_CTX_use_certificate_chain_file(ctx, temp) <= 0)
	{
		ERR_print_errors_fp(stdout);
		LOG(ERROR)<< "load ssl.chain failed."<< endl;
		exit(1);
	}
	/* 载入用户私钥 */
	if(getcwd(pwd, 256) == NULL)
			LOG(ERROR)<<"getcwd failed."<<endl;
	if (strlen(pwd) == 1)
			pwd[0] = '\0';
	temp = strcat(pwd, "/ssl/ssl.key");
	if (SSL_CTX_use_PrivateKey_file(ctx, temp, SSL_FILETYPE_PEM) <= 0)
	{
			ERR_print_errors_fp(stdout);
			LOG(ERROR)<< "load ssl.key failed."<< endl;
			exit(1);
	}
	/* 检查用户私钥是否正确 */
	int ret = SSL_CTX_check_private_key(ctx);
	LOG(INFO) << "ssl: check private [" << ret << "]" << endl;
	if (!ret)
	{
			ERR_print_errors_fp(stdout);
			exit(1);
	}

	ssl_ctx = ctx;
	return true;
}
#endif

#endif
