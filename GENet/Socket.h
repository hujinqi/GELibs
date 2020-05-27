/*
 * Multiplatform Async Network Library
 * Copyright (c) 2007 Burlex
 *
 * Socket implementable class.
 *
 */

#ifndef SOCKET_H
#define SOCKET_H

#include "common/Singleton.h"
#include "common/Mutex.h"
#include "common/CircularBuffer.h"
#include "GETime/TimeMgr.h"
#include "SocketDefines.h"
#include "WorldPacket.h"
#include <libgo/coroutine.h>

#ifdef USING_SSL
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif

class SERVER_DECL Socket
{
public:
	// Constructor. If fd = 0, it will be assigned 
	Socket(SOCKET fd, uint32 sendbuffersize, uint32 recvbuffersize);
	
	// Destructor.
	virtual ~Socket();

	// Open a connection to another machine.
	bool Connect(const char * Address, uint32 Port);

	//立即生效 Disconnect the socket.
	void Disconnect();

	//延时生效，发送完要发送的消息后，生效。Disconnect the socket.
	void KickOff();

	// Accept from the already-set fd.
	void Accept(sockaddr_in * address);

/* Implementable methods */

	// Called when data is received.
	virtual void OnRead() {}

	// Called when a connection is first successfully established.
	virtual void OnConnect() {}

	// Called when the socket is disconnected from the client (either forcibly or by the connection dropping)
	virtual void OnDisconnect() {}

/* Sending Operations */

	// Locks sending mutex, adds bytes, unlocks mutex.
	bool Send(const uint8 * Bytes, uint32 Size);

	// Burst system - Locks the sending mutex.
	GE_INLINE void BurstBegin() { m_writeMutex.Acquire(); }

	// Burst system - Adds bytes to output buffer.
	bool BurstSend(const uint8 * Bytes, uint32 Size);

	// Burst system - Pushes event to queue - do at the end of write events.
	void BurstPush();

	// Burst system - Unlocks the sending mutex.
	GE_INLINE void BurstEnd() { m_writeMutex.Release(); }

/* Client Operations */

	// Get the client's ip in numerical form.
	string GetRemoteIP();
	GE_INLINE uint32 GetRemotePort() { return ntohs(m_client.sin_port); }
	GE_INLINE SOCKET GetFd() { return m_fd; }
	
/* Platform-specific methods */

	void SetupReadEvent();
	void ReadCallback(uint32 len);
	void WriteCallback();
	bool SendPacket(const WorldPacket& pack);

	GE_INLINE bool IsDeleted() { return m_deleted; }
	GE_INLINE bool IsConnected() { return m_connected; }
	GE_INLINE sockaddr_in & GetRemoteStruct() { return m_client; }
	GE_INLINE CircularBuffer& GetReadBuffer() { return readBuffer; }
	GE_INLINE CircularBuffer& GetWriteBuffer() { return writeBuffer; }

/* Deletion */
	void Delete();

	GE_INLINE in_addr GetRemoteAddress() { return m_client.sin_addr; }

protected:

	// Called when connection is opened.
	void _OnConnect();
  
	SOCKET m_fd;
	bool m_useSSL = false;
#ifdef USING_SSL
	SSL *ssl;
	bool ssl_handshake;
	void _ReadCallback_SSL(uint32 len);
	bool DoSSLHandshake();
#endif
	void _ReadCallback(uint32 len);
	uint32 m_errCnt;

	CircularBuffer readBuffer;
	CircularBuffer writeBuffer;

	Mutex m_writeMutex;
	Mutex m_readMutex;
//	co_mutex co_writeMutex;
//	co_mutex co_readMutext;

	//是否需要关闭
	bool m_kick_off = false;

	// we are connected? stop from posting events.
	bool m_connected;

	// We are deleted? Stop us from posting events.
	bool m_deleted;

	sockaddr_in m_client;

/* Win32 - IOCP Specific Calls */
#ifdef CONFIG_USE_IOCP

public:

	// Set completion port that this socket will be assigned to.
	GE_INLINE void SetCompletionPort(HANDLE cp) { m_completionPort = cp; }
	
	// Atomic wrapper functions for increasing read/write locks
	GE_INLINE void IncSendLock() { InterlockedIncrement(&m_writeLock); }
	GE_INLINE void DecSendLock() { InterlockedDecrement(&m_writeLock); }
	GE_INLINE bool AcquireSendLock()
	{
		if(m_writeLock)
			return false;
		else
		{
			IncSendLock();
			return true;
		}
	}
	OverlappedStruct m_readEvent;
	OverlappedStruct m_writeEvent;

private:
	// Completion port socket is assigned to
	HANDLE m_completionPort;
	
	// Write lock, stops multiple write events from being posted.
	volatile long m_writeLock;
	
	// Assigns the socket to his completion port.
	void AssignToCompletionPort();

#endif

/* Linux - EPOLL Specific Calls */
#ifdef CONFIG_USE_EPOLL
public:
	// Posts a epoll event with the specifed arguments.
	void PostEvent(uint32 events);

	// Atomic wrapper functions for increasing read/write locks
	GE_INLINE void IncSendLock() { m_writeLockMutex.Acquire(); m_writeLock++; m_writeLockMutex.Release(); }
	GE_INLINE void DecSendLock() { m_writeLockMutex.Acquire(); m_writeLock--; m_writeLockMutex.Release(); }
	GE_INLINE bool HasSendLock() { bool res; m_writeLockMutex.Acquire(); res = (m_writeLock != 0); m_writeLockMutex.Release(); return res; }
	bool AcquireSendLock()
	{
	  bool rv;
	  m_writeLockMutex.Acquire();
	  if(m_writeLock != 0)
		rv = false;
	  else
	  {
		rv = true;
		m_writeLock++;
	  }
	  m_writeLockMutex.Release();
	  return rv;
	}

private:
	unsigned int m_writeLock;
	Mutex m_writeLockMutex;
#endif

/* FreeBSD - kqueue specific calls */
#ifdef CONFIG_USE_KQUEUE
public:
	// Posts a epoll event with the specifed arguments.
	void PostEvent(int events, bool oneshot);
	// Atomic wrapper functions for increasing read/write locks
	GE_INLINE void IncSendLock() { m_writeLockMutex.Acquire(); m_writeLock++; m_writeLockMutex.Release(); }
	GE_INLINE void DecSendLock() { m_writeLockMutex.Acquire(); m_writeLock--; m_writeLockMutex.Release(); }
	GE_INLINE bool HasSendLock() { bool res; m_writeLockMutex.Acquire(); res = (m_writeLock != 0); m_writeLockMutex.Release(); return res; }
	bool AcquireSendLock()
	{
		bool rv;
		m_writeLockMutex.Acquire();
		if(m_writeLock != 0)
			rv = false;
		else
		{
			rv = true;
			m_writeLock++;
		}
		m_writeLockMutex.Release();
		return rv;
	}

private:
	unsigned int m_writeLock;
	Mutex m_writeLockMutex;
#endif
};

/** Connect to a server.
* @param hostname Hostname or IP address to connect to
* @param port Port to connect to
* @return templated type if successful, otherwise null
*/
template<class T>
T* ConnectTCPSocket(const char * hostname, u_short port)
{
	sockaddr_in conn;
	hostent * host;

	/* resolve the peer */
	host = gethostbyname(hostname);

	if(!host)
		return NULL;

	/* copy into our address struct */
	memcpy(&conn.sin_addr, host->h_addr_list[0], sizeof(in_addr));
	conn.sin_family = AF_INET;
	conn.sin_port = ntohs(port);

	T * s = new T(0);
	if(!s->Connect(hostname, port))
	{
		s->Delete();
		return 0;
	}
	return s;	
}

/* Socket Garbage Collector */
#define SOCKET_GC_TIMEOUT 15

class SocketGarbageCollector : public Singleton<SocketGarbageCollector>
{
	map<Socket*, time_t> deletionQueue;
	Mutex lock;
public:
	~SocketGarbageCollector()
	{
		map<Socket*, time_t>::iterator i;
		for(i=deletionQueue.begin();i!=deletionQueue.end();++i)
			delete i->first;
	}

	void Update()
	{
		map<Socket*, time_t>::iterator i, i2;
		time_t t = sTime.Time();
		lock.Acquire();
		for(i = deletionQueue.begin(); i != deletionQueue.end();)
		{
			i2 = i++;
			if(i2->second <= t)
			{
				delete i2->first;
				deletionQueue.erase(i2);
			}
		}
		lock.Release();
	}

	void QueueSocket(Socket * s)
	{
		lock.Acquire();
		deletionQueue.insert( map<Socket*, time_t>::value_type( s, sTime.Time() + SOCKET_GC_TIMEOUT ) );
		lock.Release();
	}
};

#define sSocketGarbageCollector SocketGarbageCollector::getSingleton()

#endif
