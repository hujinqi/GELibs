/*
 * Multiplatform Async Network Library
 * Copyright (c) 2007 Burlex
 *
 * SocketLinux.cpp - Linux-specific functions of Socket class are located here.
 *
 */

#include "Network.h"
#ifdef CONFIG_USE_EPOLL
#include "Socket.h"
#include <glog/logging.h>

void Socket::PostEvent(uint32 events)
{
	if(m_kick_off)
	{
		return;
	}

    int epoll_fd = sSocketMgr.GetEpollFd();

    struct epoll_event ev;
	memset(&ev, 0, sizeof(epoll_event));
    ev.data.fd = m_fd;
    ev.events = events | EPOLLET;			/* use edge-triggered instead of level-triggered because we're using nonblocking sockets */

    // post actual event
    if(epoll_ctl(epoll_fd, EPOLL_CTL_MOD, ev.data.fd, &ev))
    	LOG(WARNING) << "Could not post event on fd " << m_fd << " errno:" << errno << endl;
//		sLog.Warning("epoll", "Could not post event on fd %u", m_fd);
}

void Socket::ReadCallback(uint32 len)
{
#ifdef USING_SSL
	if(m_useSSL)
	{
		_ReadCallback_SSL(len);
	}
	else
	{
		_ReadCallback(len);
	}
#else
	_ReadCallback(len);
#endif
}

void Socket::WriteCallback()
{
	// We should already be locked at this point, so try to push everything out.
	uint32 pos = 0;
	uint32 waitSendLen = writeBuffer.GetContiguiousBytes();
	uint32 sendTimes = 0;
//	DLOG(INFO)<<"# test total send len:"<<waitSendLen<<endl;
	do{

#ifdef USING_SSL
		int bytes_written = 0;
		if(m_useSSL)
		{
			bytes_written = SSL_write(ssl, (uint8*)writeBuffer.GetBufferStart() + pos, writeBuffer.GetContiguiousBytes() - pos);
		}
		else
		{
			bytes_written = send(m_fd, (uint8*)writeBuffer.GetBufferStart() + pos, writeBuffer.GetContiguiousBytes() - pos, 0);
		}
#else
//		DLOG(INFO)<<"~~~~ send m_fd:"<<m_fd<<" msg:"<<(void*)((uint8*)writeBuffer.GetBufferStart() + pos)<<" cnt:"<<(writeBuffer.GetContiguiousBytes() - pos)<<" pos:"<<pos;
		int bytes_written = send(m_fd, (uint8*)writeBuffer.GetBufferStart() + pos, writeBuffer.GetContiguiousBytes() - pos, 0);
#endif
		//发生错误
		if(-1 == bytes_written)
		{
			if(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
			{
				bytes_written = 0;
				continue;
			}
			//异常断开
			else
			{
				Disconnect();
				break;
			}
		}
		//接收到主动关闭请求
		if(0 == bytes_written)
		{
			Disconnect();
			break;
		}

//		DLOG(INFO)<<"# test send len:"<<bytes_written<<endl;
		pos += bytes_written;
		sendTimes++;
	}while(pos < waitSendLen);

//	writeBuffer.Remove(pos);
	writeBuffer.Clean();

//	if(sendTimes > 1)
//	{
//		DLOG(INFO)<<"分多次发送"<<endl;
//	}

	//最后的数据发送完毕后踢掉
	if(m_kick_off)
	{
		DLOG(INFO)<<"检测到需要断开"<<endl;
		Disconnect();
	}
}

void Socket::BurstPush()
{
    if(AcquireSendLock())
        PostEvent(EPOLLOUT);
}

void Socket::_ReadCallback(uint32 len)
{
	// We have to lock here.
	m_readMutex.Acquire();	//LOCK

	while(true)
	{
		size_t space = readBuffer.GetSpace();
		int bytes = recv(m_fd, readBuffer.GetBuffer(), space, 0);

//		DLOG(INFO)<<"# test recv ret bytes:"<<bytes<<" errno:"<<errno<<" fd:"<<m_fd<<" m_errCnt:"<<m_errCnt<<" space:"<<space<<endl;
		if(bytes == 0)
		{
			if(space == 0)
			{
				LOG(ERROR)<<"socket读缓存满了，断开连接。 size: "<<readBuffer.GetSize()<<" errno:"<<errno;
			}
			//客户端断开了连接
			m_readMutex.Release();
			Disconnect();
			return;
		}
		else if(bytes < 0)
		{
			//没有数据了，稍后尝试
			if(errno == EAGAIN || errno == EWOULDBLOCK)
			{
				break;
			}
			//操作被中断，重新尝试
			if(errno == EINTR)
			{
				DLOG(ERROR)<<"# test recv EINTR! fd:"<<m_fd<<endl;
				if(++m_errCnt < 8)
				{
					continue;	//继续尝试几次
				}
			}
			m_readMutex.Release();
			Disconnect();
			return;
		}

		//连续错误计数清0
		m_errCnt = 0;

		//m_readByteCount += bytes;
		readBuffer.IncrementWritten(bytes);
		// call virtual onread()
		OnRead();

		if(!this->m_connected)
		{
			break;	//OnRead中断开了连接
		}
	}

	m_readMutex.Release();	//UNLOCK
}

#endif
