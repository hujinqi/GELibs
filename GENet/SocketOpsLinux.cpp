/*
 * Multiplatform Async Network Library
 * Copyright (c) 2007 Burlex
 *
 * SocketOpsLinux.cpp - Linux implementation of SocketOps.
 *
 */


#include "Network.h"
#ifdef CONFIG_USE_EPOLL
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>

namespace SocketOps
{
    // Create file descriptor for socket i/o operations.
    SOCKET CreateTCPFileDescriptor()
    {
        // create a socket for use with overlapped i/o.
        return socket(AF_INET, SOCK_STREAM, 0);
    }

    // Disable blocking send/recv calls.
    bool Nonblocking(SOCKET fd)
    {
        //uint32 arg = 1;
        //return (ioctl(fd, FIONBIO, &arg) == 0);

		int opts = fcntl( fd, F_GETFL );
		if( opts < 0 )
		{
			LOG(ERROR)<<"fcntl getfl failed at "<<fd<<endl;
			return false;
		}
		opts = opts | O_NONBLOCK;
		if( fcntl( fd, F_SETFL, opts ) < 0 )
		{
			LOG(ERROR)<<"fcntl nonblk failed at "<<fd<<endl;
			return false;
		}
		int opt = 1;
		setsockopt( fd, SOL_SOCKET, SO_REUSEADDR, (const void*)&opt, sizeof(opt) );
		const int nodelay = 1;
		if( setsockopt( fd, IPPROTO_TCP, TCP_NODELAY, (const char*)&nodelay, sizeof(int) ) )
		{
			LOG(ERROR)<<"setsockopt fd:"<<fd<<" TCP_NODELAY failed! errno:"<<errno<<endl;
			return false;
		}
		int flag = 1;
		setsockopt( fd, SOL_SOCKET, SO_LINGER, (const char*)&flag, sizeof(int) );
		return true;
    }

    // Disable blocking send/recv calls.
    bool Blocking(SOCKET fd)
    {
        uint32 arg = 0;
        return (ioctl(fd, FIONBIO, &arg) == 0);
    }

    // Disable nagle buffering algorithm
    bool DisableBuffering(SOCKET fd)
    {
        uint32 arg = 1;
        return (setsockopt(fd, 0x6, 0x1, (const char*)&arg, sizeof(arg)) == 0);
    }

    // Enable nagle buffering algorithm
    bool EnableBuffering(SOCKET fd)
    {
        uint32 arg = 0;
        return (setsockopt(fd, 0x6, 0x1, (const char*)&arg, sizeof(arg)) == 0);
    }

    // Set internal buffer size to socket.
    bool SetSendBufferSize(SOCKET fd, uint32 size)
    {
        return (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (const char*)&size, sizeof(size)) == 0);
    }

    // Set internal buffer size to socket.
    bool SetRecvBufferSize(SOCKET fd, uint32 size)
    {
        return (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (const char*)&size, sizeof(size)) == 0);
    }

	// Set internal timeout.
	bool SetTimeout(SOCKET fd, uint32 timeout)
	{
		struct timeval to;
		to.tv_sec = timeout;
		to.tv_usec = 0;
		if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&to, (socklen_t)sizeof(to)) != 0) return false;
		return (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&to, (socklen_t)sizeof(to)) == 0);
	}

    // Closes a socket fully.
    void CloseSocket(SOCKET fd)
    {
        shutdown(fd, SHUT_RDWR);
        close(fd);
    }

    // Sets reuseaddr
    void ReuseAddr(SOCKET fd)
    {
        uint32 option = 1;
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&option, 4);
    }

    int GetError(SOCKET fd)
    {
    	int optval;
    	unsigned int optlen = sizeof(int);
    	getsockopt(fd, SOL_SOCKET, SO_ERROR, (char*)&optval, &optlen);
    	return optval;
    }
}

#endif
