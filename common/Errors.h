#ifndef SERVER_ERRORS_H
#define SERVER_ERRORS_H
#include <stdio.h>
#include <assert.h>
//#include "NGLog.h"
//#include "CrashHandler.h"
// TODO: handle errors better


// An assert isn't necessarily fatal, although if compiled with asserts enabled it will be.
#if defined(WIN32) && defined(_DEBUG)
#define GEAssert( assertion ) { if( !(assertion) ) { fprintf( stderr, "\n%s:%i ASSERTION FAILED:\n  %s\n", __FILE__, __LINE__, #assertion ); /*CStackWalker sw; sw.ShowCallstack();*/ assert(assertion); } }
#else
#define GEAssert( assertion ) { if( !(assertion) ) { fprintf( stderr, "\n%s:%i ASSERTION FAILED:\n  %s\n", __FILE__, __LINE__, #assertion ); assert(assertion); } }
#endif

#define GEError( assertion, errmsg ) if( ! (assertion) ) { sLog.Error( "ERROR","%s:%i INFO:\n  %s\n", __FILE__, __LINE__, (char *)errmsg ); assert( false ); }
#define GEWarning( assertion, errmsg ) if( ! (assertion) ) { sLog.Error( "WARNING","%s:%i INFO:\n  %s\n", __FILE__, __LINE__, (char *)errmsg ); }

// This should always halt everything.  If you ever find yourself wanting to remove the assert( false ), switch to WPWarning or WPError
#define GEFatal( assertion, errmsg ) if( ! (assertion) ) { sLog.Error( "ERROR","%s:%i FATAL INFO:\n  %s\n", __FILE__, __LINE__, (char *)errmsg ); assert( #assertion &&0 ); abort(); }

#define ASSERT GEAssert

struct _ABORT
{
	char _A;
};

#ifdef WIN32
#define ABORT() _ABORT* _A = NULL;_A->_A = 1
#else
#define ABORT() abort()
#endif

#endif