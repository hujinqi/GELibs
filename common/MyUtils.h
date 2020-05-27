/*
 * MyUtils.h
 *
 *  Created on: 2018年12月13日
 *      Author: fuyunlv
 */

#ifndef SRC_MODULE_COMMON_MYUTILS_H_
#define SRC_MODULE_COMMON_MYUTILS_H_

#include <string>
#include <vector>

#include <memory>
#include <iostream>
#include <string>
#include <cstdio>

//#include <string.h>
using namespace std;
class MyUtils {
public:
	MyUtils();
	virtual ~MyUtils();


//	vector<string> Split(const string& str, const string& delim);
	static vector<string> Split( const string& s, const string& delim, const bool keep_empty = true );
	static string GetSplitNewLine( const string& s, const bool keep_empty );
	static vector<string> SplitIfNull( const string& s, const string& delim, const bool keep_empty = true );
	static std::string Trim( const std::string& str, const std::string& whitespace );

	static int GetSplitData( const string& s, int index );//格式1,2
	static string GetSplitDataStr( const string& s, int index );//格式1,2

	static string NumToString( const int num );
	static string NumToString( const float num );
	static string NumToString( const long num );
	static string NumToString( const double num );
	static string NumToString( const size_t num );

	static int MyAtoi( const char * str );
	static long MyAtol( const char * str );
	static float MyAtof( const char * str );
	static int MyStrtoi( string str );
	static int MyAtoiStr( const string& str );
	template<typename ... Args>
	static string String_format( const std::string& format, Args ... args )
	{
	    size_t size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
	    unique_ptr<char[]> buf( new char[ size ] );
	    snprintf( buf.get(), size, format.c_str(), args ... );
	    return string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
	}


	static int MyRandom( int min, int max );//获得随机数。包左包右
	static int MyRandomStr( string str );		//获得随机数。格式1,3

	static string RunCmd( string cmd );
};

#endif /* SRC_MODULE_COMMON_MYUTILS_H_ */
