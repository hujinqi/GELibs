/*
 * Multiplatform Async Network Library
 * Copyright (c) 2007 Burlex
 *
 * Network.h - Include this file in your .cpp files to gain access
 *			 to features offered by this library.
 *
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include "SocketDefines.h"
#include "SocketOps.h"
#include "Socket.h"

#ifdef WIN32
#include "SocketMgrWin32.h"
#include "ListenSocketWin32.h"

#else
#define CONFIG_USE_EPOLL
#include "SocketMgrLinux.h"
#include "ListenSocketLinux.h"

#endif
#include "SocketDefines.h"
#include "Socket.h"

#endif
