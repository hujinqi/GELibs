﻿/*
 * Multiplatform Async Network Library
 * Copyright (c) 2007 Burlex
 *
 * ListenSocket<T>: Creates a socket listener on specified address and port,
 *                  requires Update() to be called every loop.
 *
*/

#ifdef CONFIG_USE_EPOLL
#ifndef _LISTENSOCKET_H
#define _LISTENSOCKET_H

#include "Network.h"
#include <glog/logging.h>

class ListenSocketBase
{
public:
	virtual ~ListenSocketBase() {}
	virtual void OnAccept() = 0;
	virtual int GetFd() = 0;
};

template<class T>
class ListenSocket : public ListenSocketBase
{
public:
	ListenSocket(const char * ListenAddress, uint32 Port) : ListenSocketBase()
    {
        m_socket = socket(AF_INET, SOCK_STREAM, 0);
        SocketOps::ReuseAddr(m_socket);
        SocketOps::Nonblocking(m_socket);
        SocketOps::SetTimeout(m_socket, 60);

        m_address.sin_family = AF_INET;
        m_address.sin_port = ntohs((u_short)Port);
        m_address.sin_addr.s_addr = htonl(INADDR_ANY);
        m_opened = false;

        if(strcmp(ListenAddress, "0.0.0.0"))
        {
            struct hostent * hostname = gethostbyname(ListenAddress);
            if(hostname != 0)
                memcpy(&m_address.sin_addr.s_addr, hostname->h_addr_list[0], hostname->h_length);
        }

        // bind.. well attempt to.
        int ret = bind(m_socket, (const sockaddr*)&m_address, sizeof(m_address));
        if(ret != 0)
        {
            printf("Bind unsuccessful on port %u.", (unsigned int)Port);
            return;
        }

        ret = listen(m_socket, 128);
        if(ret != 0) 
        {
            printf("Unable to listen on port %u.", (unsigned int)Port);
            return;
        }
        len = sizeof(sockaddr_in);
        m_opened = true;
		sSocketMgr.AddListenSocket(this);
    }

    ~ListenSocket()
    {
        if(m_opened)
            SocketOps::CloseSocket(m_socket);
    }

    void Close()
    {
        if(m_opened)
            SocketOps::CloseSocket(m_socket);
        m_opened = false;
    }

    /*void Update()
    {
        aSocket = accept(m_socket, (sockaddr*)&m_tempAddress, (socklen_t*)&len);
        if(aSocket == -1)
            return;

        dsocket = new T(aSocket);
        dsocket->Accept(&m_tempAddress);
    }*/

	void OnAccept()
	{
		while(true)
		{
			aSocket = accept(m_socket, (sockaddr*)&m_tempAddress, (socklen_t*)&len);
			if(aSocket == -1)
				break;

			dsocket = new T(aSocket);
			dsocket->Accept(&m_tempAddress);
		}
	}

    inline bool IsOpen() { return m_opened; }
	int GetFd() { return m_socket; }	

private:
    SOCKET m_socket;
    SOCKET aSocket;
    struct sockaddr_in m_address;
    struct sockaddr_in m_tempAddress;
    bool m_opened;
    uint32 len;
    T * dsocket;
};

#endif
#endif

