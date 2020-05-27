/*
 * config.h
 *
 *  Created on: 2018年3月6日
 *      Author: carlos Hu
 */

#ifndef SRC_MODULE_COMMON_DEFINES_H_
#define SRC_MODULE_COMMON_DEFINES_H_


#define USING_LIBGO_3_0
//#define USING_C_STRING
//#define USING_SSL
#define TEST_MEMERY


#ifdef WIN32
#pragma warning(disable:4251)		// dll-interface bullshit
#pragma warning(disable:4996)
#endif

#ifdef WIN32
#define GE_INLINE __forceinline
#else
#define GE_INLINE inline
#endif

#ifdef WIN32
typedef signed __int64 int64;
typedef signed __int32 int32;
typedef signed __int16 int16;
typedef signed __int8 int8;

typedef unsigned __int64 uint64;
typedef unsigned __int32 uint32;
typedef unsigned __int16 uint16;
typedef unsigned __int8 uint8;
#else
#include <stdint.h>
typedef int64_t int64;
typedef int32_t int32;
typedef int16_t int16;
typedef int8_t int8;
typedef uint64_t uint64;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;
typedef uint32_t DWORD;
#endif

#ifdef WIN32
	#ifndef SCRIPTLIB
		#define SERVER_DECL __declspec(dllexport)
		#define SCRIPT_DECL __declspec(dllimport)
	#else
		#define SERVER_DECL __declspec(dllimport)
		#define SCRIPT_DECL __declspec(dllexport)
	#endif
#else
	#define SERVER_DECL
	#define SCRIPT_DECL
#endif

#ifndef WIN32
#ifndef X64
#  if defined (__GNUC__)
#	if GCC_VERSION >= 30400
#         ifdef HAVE_DARWIN
#	      define __fastcall
#         else
#    	      define __fastcall __attribute__((__fastcall__))
#         endif
#	else
#	  define __fastcall __attribute__((__regparm__(3)))
#	endif
#  else
#	define __fastcall __attribute__((__fastcall__))
#  endif
#else
#define __fastcall
#endif
#endif

#ifdef WIN32
//windows 两个头文件引用顺序不能错，由于windows.h使用较为频繁，防止出错，提前按顺序引用
#include <winsock2.h>
#include <windows.h>
#else
#include <string.h>
#include <unistd.h>
#endif

#include <glog/logging.h>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
using std::tr1::unordered_map;
using std::tr1::unordered_set;
using namespace std;

#ifndef WIN32
//#define Sleep(ms) myusleep(1000*ms)

//c++11标准库新方法
#include <chrono>
#include <system_error>
#include <thread>
#define Sleep(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms))
#endif

//随机数
#define GE_RAND(min, max) _ge_rand(min, max)
//便利stl容器
#define foreach(stl, it_name) for(auto it_name=(stl).begin(); it_name != (stl).end(); ++it_name)
#define foreach_const(stl, it_name) for(auto it_name=(stl).cbegin(); it_name != (stl).cend(); ++it_name)
//清空stl::queue
#define QUEUE_CLEAR(queue) while(!queue.empty()) queue.pop()


/**
 * 系统的usleep可能会受到信号的影响，失效
 */
GE_INLINE int myusleep(uint32 nusecs)
{
	timeval tval;
	tval.tv_sec = nusecs / 1000000;
	tval.tv_usec = nusecs % 1000000;
	return select(0, NULL, NULL, NULL, &tval);
}

/* these can be optimized into assembly */
//#define USING_BIG_ENDIAN
#ifdef USING_BIG_ENDIAN

GE_INLINE static void swap16(uint16* p) { *p = ((*p >> 8) & 0xff) | (*p << 8); }
GE_INLINE static void swap32(uint32* p) { *p = ((*p >> 24 & 0xff)) | ((*p >> 8) & 0xff00) | ((*p << 8) & 0xff0000) | (*p << 24); }
GE_INLINE static void swap64(uint64* p) { *p = ((*p >> 56)) | ((*p >> 40) & 0x000000000000ff00ULL) | ((*p >> 24) & 0x0000000000ff0000ULL) | ((*p >> 8 ) & 0x00000000ff000000ULL) |
								((*p << 8 ) & 0x000000ff00000000ULL) | ((*p << 24) & 0x0000ff0000000000ULL) | ((*p << 40) & 0x00ff000000000000ULL) | ((*p << 56)); }

//GE_INLINE static void swap16(uint16* p) { *p = bswap_16((uint16_t)*p); }
//GE_INLINE static void swap32(uint32* p) { *p = bswap_32((uint32_t)*p); }
//GE_INLINE static void swap64(uint64* p) { *p = bswap_64((uint64_t)*p);; }

GE_INLINE static float swapfloat(float p)
{
	union { float asfloat; uint8 asbytes[4]; } u1, u2;
	u1.asfloat = p;
	/* swap! */
	u2.asbytes[0] = u1.asbytes[3];
	u2.asbytes[1] = u1.asbytes[2];
	u2.asbytes[2] = u1.asbytes[1];
	u2.asbytes[3] = u1.asbytes[0];

	return u2.asfloat;
}

GE_INLINE static double swapdouble(double p)
{
	union { double asfloat; uint8 asbytes[8]; } u1, u2;
	u1.asfloat = p;
	/* swap! */
	u2.asbytes[0] = u1.asbytes[7];
	u2.asbytes[1] = u1.asbytes[6];
	u2.asbytes[2] = u1.asbytes[5];
	u2.asbytes[3] = u1.asbytes[4];
	u2.asbytes[4] = u1.asbytes[3];
	u2.asbytes[5] = u1.asbytes[2];
	u2.asbytes[6] = u1.asbytes[1];
	u2.asbytes[7] = u1.asbytes[0];

	return u2.asfloat;
}

GE_INLINE static void swapfloat(float * p)
{
	union { float asfloat; uint8 asbytes[4]; } u1, u2;
	u1.asfloat = *p;
	/* swap! */
	u2.asbytes[0] = u1.asbytes[3];
	u2.asbytes[1] = u1.asbytes[2];
	u2.asbytes[2] = u1.asbytes[1];
	u2.asbytes[3] = u1.asbytes[0];
	*p = u2.asfloat;
}

GE_INLINE static void swapdouble(double * p)
{
	union { double asfloat; uint8 asbytes[8]; } u1, u2;
	u1.asfloat = *p;
	/* swap! */
	u2.asbytes[0] = u1.asbytes[7];
	u2.asbytes[1] = u1.asbytes[6];
	u2.asbytes[2] = u1.asbytes[5];
	u2.asbytes[3] = u1.asbytes[4];
	u2.asbytes[4] = u1.asbytes[3];
	u2.asbytes[5] = u1.asbytes[2];
	u2.asbytes[6] = u1.asbytes[1];
	u2.asbytes[7] = u1.asbytes[0];
	*p = u2.asfloat;
}

GE_INLINE static uint16 swap16(uint16 p) { return ((p >> 8) & 0xff) | (p << 8); }
GE_INLINE static uint32 swap32(uint32 p) { return ((p >> 24) & 0xff) | ((p >> 8) & 0xff00) | ((p << 8) & 0xff0000) | (p << 24); }
GE_INLINE static uint64 swap64(uint64 p)  { p = (((p >> 56) & 0xff)) | ((p >> 40) & 0x000000000000ff00ULL) | ((p >> 24) & 0x0000000000ff0000ULL) | ((p >> 8 ) & 0x00000000ff000000ULL) |
								((p << 8 ) & 0x000000ff00000000ULL) | ((p << 24) & 0x0000ff0000000000ULL) | ((p << 40) & 0x00ff000000000000ULL) | ((p << 56)); return p;}

GE_INLINE static void swap16(int16* p) { *p = ((*p >> 8) & 0xff) | (*p << 8); }
GE_INLINE static void swap32(int32* p) { *p = ((*p >> 24) & 0xff) | ((*p >> 8) & 0xff00) | ((*p << 8) & 0xff0000) | (*p << 24); }
GE_INLINE static void swap64(int64* p) { *p = ((*p >> 56) & 0xff) | ((*p >> 40) & 0x000000000000ff00ULL) | ((*p >> 24) & 0x0000000000ff0000ULL) | ((*p >> 8 ) & 0x00000000ff000000ULL) |
								((*p << 8 ) & 0x000000ff00000000ULL) | ((*p << 24) & 0x0000ff0000000000ULL) | ((*p << 40) & 0x00ff000000000000ULL) | ((*p << 56)); }

GE_INLINE static int16 swap16(int16 p) { return ((p >> 8) & 0xff) | (p << 8); }
GE_INLINE static int32 swap32(int32 p) { return ((p >> 24) & 0xff) | ((p >> 8) & 0xff00) | ((p << 8) & 0xff0000) | (p << 24); }
GE_INLINE static int64 swap64(int64 p)  { return ((((p >> 56) & 0xff)) | ((p >> 40) & 0x000000000000ff00ULL) | ((p >> 24) & 0x0000000000ff0000ULL) | ((p >> 8 ) & 0x00000000ff000000ULL) |
								((p << 8 ) & 0x000000ff00000000ULL) | ((p << 24) & 0x0000ff0000000000ULL) | ((p << 40) & 0x00ff000000000000ULL) | ((p << 56))); }

//GE_INLINE static uint16 swap16(uint16 p) { return bswap_16((uint16_t)p); }
//GE_INLINE static uint32 swap32(uint32 p) { return bswap_32((uint32_t)p); }
//GE_INLINE static uint64 swap64(uint64 p)  { return bswap_64((uint64_t)p); }
//
//GE_INLINE static void swap16(int16* p) { *p = bswap_16((uint16_t)*p); }
//GE_INLINE static void swap32(int32* p) { *p = bswap_32((uint32_t)*p); }
//GE_INLINE static void swap64(int64* p) { *p = bswap_64((uint64_t)*p); }
//
//GE_INLINE static int16 swap16(int16 p) { return bswap_16((uint16_t)p); }
//GE_INLINE static int32 swap32(int32 p) { return bswap_32((uint32_t)p); }
//GE_INLINE static int64 swap64(int64 p)  { return bswap_64((uint64_t)p); }

#endif

GE_INLINE static void HexDump(const uint8 *buf,size_t len,int addr) {
    int i,j,k;
    char binstr[80];

    for (i=0;i<(int)len;i++) {
        if (0==(i%16)) {
            sprintf(binstr,"%08x -",i+addr);
            sprintf(binstr,"%s %02x",binstr,(unsigned char)buf[i]);
        } else if (15==(i%16)) {
            sprintf(binstr,"%s %02x",binstr,(unsigned char)buf[i]);
            sprintf(binstr,"%s  ",binstr);
            for (j=i-15;j<=i;j++) {
                sprintf(binstr,"%s%c",binstr,('!'<buf[j]&&buf[j]<='~')?buf[j]:'.');
            }
            printf("%s\n",binstr);
        } else {
            sprintf(binstr,"%s %02x",binstr,(unsigned char)buf[i]);
        }
    }
    if (0!=(i%16)) {
        k=16-(i%16);
        for (j=0;j<k;j++) {
            sprintf(binstr,"%s   ",binstr);
        }
        sprintf(binstr,"%s  ",binstr);
        k=16-k;
        for (j=i-k;j<i;j++) {
            sprintf(binstr,"%s%c",binstr,('!'<buf[j]&&buf[j]<='~')?buf[j]:'.');
        }
        printf("%s\n",binstr);
    }
}

GE_INLINE int _ge_rand(int min, int max)
{
	if(min == max)
		return min;
	return rand()%(max - min) + min;
}



#endif /* SRC_MODULE_COMMON_DEFINES_H_ */
