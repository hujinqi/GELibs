/*
 * Multiplatform Async Network Library
 * Copyright (c) 2007 Burlex
 *
 * Socket implementable class.
 *
 */

#include "Network.h"
#include "Socket.h"
#include "SocketOps.h"
#include <glog/logging.h>


initialiseSingleton(SocketGarbageCollector);

Socket::Socket(SOCKET fd, uint32 sendbuffersize, uint32 recvbuffersize) : m_fd(fd), m_connected(false),	m_deleted(false)
{
	// Allocate Buffers
	readBuffer.Allocate(recvbuffersize);
	writeBuffer.Allocate(sendbuffersize);

	// IOCP Member Variables
#ifdef CONFIG_USE_IOCP
	m_writeLock = 0;
	m_completionPort = 0;
#else
	m_writeLock = 0;
#endif

#ifdef USING_SSL
	ssl = NULL;
	ssl_handshake = false;
#endif
	m_errCnt = 0;

	// Check for needed fd allocation.
	if(m_fd == 0)
		m_fd = SocketOps::CreateTCPFileDescriptor();
}

Socket::~Socket()
{
#ifdef USING_SSL
	if(ssl)
		SSL_free(ssl);
#endif
	m_errCnt = 0;
}

bool Socket::Connect(const char * Address, uint32 Port)
{
	struct hostent * ci = gethostbyname(Address);
	if(ci == 0)
		return false;

	m_client.sin_family = ci->h_addrtype;
	m_client.sin_port = ntohs((u_short)Port);
	memcpy(&m_client.sin_addr.s_addr, ci->h_addr_list[0], ci->h_length);

	SocketOps::Blocking(m_fd);
	if(connect(m_fd, (const sockaddr*)&m_client, sizeof(m_client)) == -1)
		return false;

	// at this point the connection was established
#ifdef CONFIG_USE_IOCP
	m_completionPort = sSocketMgr.GetCompletionPort();
#endif
	LOG(INFO)<<"client connect server. fd:"<<m_fd<<" ip:"<<GetRemoteIP()<<endl;
	_OnConnect();
	return true;
}

#ifdef USING_SSL
bool Socket::DoSSLHandshake()
{
	if(NULL == ssl)
	{
		/* 基于 ctx 产生一个新的 SSL */
	        ssl = SSL_new(sSocketMgr.GetSSL_CTX());
        	/* 将连接用户的 socket 加入到 SSL */
        	SSL_set_fd(ssl, m_fd);
        	// 设置成服务器模式
        	SSL_set_accept_state(ssl);
	}

	DLOG(INFO)<<"开始握手 fd:"<<m_fd<<endl;
	int r = SSL_do_handshake(ssl);
	//int r = SSL_accept(ssl);
	if(1 == r)
	{
		//SSL握手已完成
		int socketErr = SocketOps::GetError(m_fd);
		DLOG(INFO)<<"握手成功 fd:"<<m_fd<<endl;
		ssl_handshake = true;
		return true;
	}

	int err = SSL_get_error(ssl, r);
	if (err == SSL_ERROR_WANT_WRITE)
	{
		//SSL需要在非阻塞socket可写时写入数据
		DLOG(INFO)<<"SSL握手。需要写数据"<<endl;
		PostEvent(EPOLLOUT);
		return false;
	}
	else if(err == SSL_ERROR_WANT_READ)
	{
		//SSL需要在非阻塞socket可读时读入数据
		DLOG(INFO)<<"SSL握手。需要继续读取数据"<<endl;
		PostEvent(EPOLLIN);
		return false;
	}

	//错误
	LOG(WARNING)<<"SSL握手错误, fd:"<<m_fd<<" err:"<<err<<" errno:"<<errno<<endl;
	//记录所有加密算法的信息
    LOG(WARNING)<<"SSL connection using: "<<SSL_get_cipher(ssl)<<endl;
    //得到服务端的证书并打印些信息
    X509* client_cert = SSL_get_peer_certificate(ssl);
    if (client_cert != NULL)
    {
        char* str = X509_NAME_oneline(X509_get_subject_name(client_cert), 0, 0);
        LOG(WARNING)<<"Client certificate:"<<endl;
        LOG(WARNING)<<"subject:"<<str<<endl;
        free(str);
        str = X509_NAME_oneline(X509_get_issuer_name(client_cert), 0, 0);
        LOG(WARNING)<<"issuer:"<<str<<endl;
        free(str);
        X509_free(client_cert);
    }
	Disconnect();
	return false;
}
#endif

void Socket::Accept(sockaddr_in * address)
{
//	DLOG(INFO)<<"创建连接 fd:"<<m_fd<<endl;
	memcpy(&m_client, address, sizeof(*address));
	_OnConnect();
}

void Socket::_OnConnect()
{
	//OnBeforeConnect();

	// set common parameters on the file descriptor
	SocketOps::Nonblocking(m_fd);
	SocketOps::DisableBuffering(m_fd);
/*	SocketOps::SetRecvBufferSize(m_fd, m_writeBufferSize);
	SocketOps::SetSendBufferSize(m_fd, m_writeBufferSize);*/
	m_connected = true;

	// IOCP stuff
#ifdef CONFIG_USE_IOCP
	AssignToCompletionPort();
	SetupReadEvent();
#endif
	sSocketMgr.AddSocket(this);

	// Call virtual onconnect
	OnConnect();
}

bool Socket::Send(const uint8 * Bytes, uint32 Size)
{
	bool rv;

	// This is really just a wrapper for all the burst stuff.
	BurstBegin();
	rv = BurstSend(Bytes, Size);
	if(rv)
		BurstPush();
	BurstEnd();

	return rv;
}

bool Socket::BurstSend(const uint8 * Bytes, uint32 Size)
{
//	cout << " socket send. Size: "<< Size << endl;
	return writeBuffer.Write(Bytes, Size);
}

string Socket::GetRemoteIP()
{
	char* ip = (char*)inet_ntoa( m_client.sin_addr );
	if( ip != NULL )
		return string( ip );
	else
		return string( "noip" );
}

void Socket::Disconnect()
{
	if(!m_connected){
		return;
	}

	m_connected = false;

	// remove from mgr
	sSocketMgr.RemoveSocket(this);

#ifdef USING_SSL
	if(ssl){
		SSL_shutdown(ssl);
	}
	
	if(m_useSSL && !ssl_handshake){
		LOG(WARNING)<<"断开没有ssl握手成功的连接。fd:"<<m_fd<<endl;
	}
#endif

	SocketOps::CloseSocket(m_fd);

	// Call virtual ondisconnect
	OnDisconnect();

	if(!m_deleted) Delete();
}

void Socket::KickOff()
{
	m_kick_off = true;
}

void Socket::Delete()
{
	if(m_deleted) return;
	m_deleted = true;
	if(m_connected) Disconnect();
	sSocketGarbageCollector.QueueSocket(this);
}

bool Socket::SendPacket(const WorldPacket& pack)
{
	if( m_connected == false )
		return false;
	
	uint32 len = (uint32)pack.size();
	uint16 cmd = (uint16)pack.GetOpcode();
	int totalLen = PACKET_HEADER_LEN + (int)pack.size();
	uint8* buff = new uint8[totalLen];
	memcpy(buff, &totalLen, 4);
	memcpy(buff+4, &cmd, 2);
	memcpy(buff+PACKET_HEADER_LEN, pack.contents(), pack.size());

//	uint16 len = swap16( (uint16)(pack->size() - 4));	//playerId 不算
//	uint16 cmd = swap16((uint16)pack->GetOpcode());
//	int totalLen = 4 + (int)pack->size();
//	uint8* buff = new uint8[totalLen];
//	memcpy(buff, &len, 2);
//	memcpy(buff+2, &cmd, 2);
//	memcpy(buff+4, pack->contents(), pack->size());

//	LOG(DEBUG) << "totalLen " << totalLen;
//	for(int i=0; i < totalLen; ++i)
//	{
//		LOG(DEBUG) << (int)((uint8)*(buff+i));
//	}

	bool succ = false;
	if( Send(buff,totalLen) )
		succ = true;

	delete [] buff;
	return succ;
}

#ifdef USING_SSL
void Socket::_ReadCallback_SSL(uint32 len)
{
	if(false == ssl_handshake)
	{
		DoSSLHandshake();
		return;	
	}

	// We have to lock here.
	m_readMutex.Acquire();	//LOCK

	while(true)
	{
		size_t space = readBuffer.GetSpace();
		int bytes = SSL_read(ssl, readBuffer.GetBuffer(), space);
		int err = SSL_get_error(ssl, bytes);

		if(bytes == 0)
		{
			//客户端断开了连接
			LOG(INFO)<<"客户端主动断开了连接. fd:"<< m_fd << " ssl_err:"<<err<<" errno:"<<errno<<endl;
			m_readMutex.Release();
			Disconnect();
			return;
		}
		else if(bytes < 0)
		{
			//没有数据了，稍后尝试
			if(err == SSL_ERROR_WANT_READ)
			{
				break;
			}
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
			LOG(INFO)<<"# test recv ret bytes:"<<bytes<<" ssl_err:"<<err<<" fd:"<<m_fd<<" m_errCnt:"<<m_errCnt<<" space:"<<space<<endl;
			LOG(INFO)<<"# test errno:"<<errno<<" errCnt:"<<m_errCnt<<endl;
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
