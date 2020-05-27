/*
 * MyUtils.cpp
 *
 *  Created on: 2018年12月13日
 *      Author: fuyunlv
 */

#include "MyUtils.h"
#include <string.h>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <sstream>
#include<cstdlib>
#include<string>
#include<cstdio>
#include<cstring>
#include<iostream>
#include<algorithm>
MyUtils::MyUtils() {
	// TODO Auto-generated constructor stub

}

MyUtils::~MyUtils() {
	// TODO Auto-generated destructor stub
}


vector<string> MyUtils::Split( const string& s, const string& delim, const bool keep_empty )
{
    vector<string> result;
    if ( delim.empty() )
    {
//        result.push_back( s );
        return result;
    }
    string::const_iterator substart = s.begin();
    string::const_iterator subend;
    while (true)
    {
        subend = search( substart, s.end(), delim.begin(), delim.end() );
        string temp( substart, subend );
        if( keep_empty || !temp.empty() )
        {
            result.push_back(temp);
        }
        if( subend == s.end() )
        {
            break;
        }
        substart = subend + delim.size();
    }
    return result;
}

//风格之后直接返回换行后的字符串
string MyUtils::GetSplitNewLine( const string& s, const bool keep_empty )
{
         vector<string> result = MyUtils::Split( s, "|", keep_empty );
         int index = 0;
         string str;
         for( vector<string>::iterator it = result.begin(); it != result.end(); ++it )
         {
                 if( 0 != index )
                 {
                         str += "\n";
                 }
                 str += *it;
                 ++index;
         }
         return str;
}
vector<string> MyUtils::SplitIfNull( const string& s, const string& delim, const bool keep_empty )
{
        if( s == "" )
        {
                return  vector<string>();
        }
        return MyUtils::Split( s, delim, keep_empty );
}

std::string MyUtils::Trim( const std::string& str, const std::string& whitespace )
{

        const size_t strBegin = str.find_first_not_of(whitespace);
        if (strBegin == std::string::npos)
                return ""; // no content

        const size_t strEnd = str.find_last_not_of(whitespace);
        const size_t strRange = strEnd - strBegin + 1;

        return str.substr(strBegin, strRange);
}

int MyUtils::GetSplitData( const string& s, int index )
{
	vector<string> vec = MyUtils::SplitIfNull( s, "," );
	if( index >= (int)vec.size() )
	{
		return 0;
	}
	return MyAtoiStr( vec.at( index ) );
}

//格式1,2
string MyUtils::GetSplitDataStr( const string& s, int index )
{
	vector<string> vec = MyUtils::SplitIfNull( s, "," );
	if( index >= (int)vec.size() )
	{
		return "";
	}
	return vec.at( index );
}


string MyUtils::NumToString( const int num )
{
     std::stringstream ss;
     ss << num;
     return ss.str();
}

string MyUtils::NumToString( const float num )
{
	std::stringstream ss;
	ss << num;
	return ss.str();
}

string MyUtils::NumToString( const long num )
{
	std::stringstream ss;
	ss << num;
	return ss.str();
}

string MyUtils::NumToString( const double num )
{
	std::stringstream ss;
	ss << num;
	return ss.str();
}

string MyUtils::NumToString( const size_t num )
{
	std::stringstream ss;
	ss << num;
	return ss.str();
}


int MyUtils::MyAtoi( const char * str )
{
	if( str )
	{
		return atoi(str);
	}
	else
	{
		return 0;
	}
}

long MyUtils::MyAtol( const char * str )
{
	if( str )
	{
	 return atol(str);
	}
	else
	{
	 return 0;
	}
}

float MyUtils::MyAtof( const char * str )
{
	if( str )
	{
	 return atof(str);
	}
	else
	{
	 return 0;
	}
}

int MyUtils::MyStrtoi( string str )
{
	if( str.c_str() )
	{
	 return atoi(str.c_str());
	}
	else
	{
	 return 0;
	}
}

int MyUtils::MyAtoiStr( const string& str )
{
	if( str.c_str() )
	{
	 return atoi(str.c_str());
	}
	else
	{
	 return 0;
	}
}

//获得随机数。包左包右
int MyUtils::MyRandom( int min, int max )
{
	if( min < 0 )
	{
		min = -min;
	}
	if( max < 0 )
	{
		max = -max;
	}
	if( min > max )
	{
		min = max;
	}


	int gap = max - min + 1;
	int data = rand() % gap;

	data += min;
	return data;
}

//获得随机数。格式1,3
int MyUtils::MyRandomStr( string str )
{
	int min = MyUtils::GetSplitData( str, 0 );
	int max = MyUtils::GetSplitData( str, 1 );
	return MyUtils::MyRandom( min, max );
}

string MyUtils::RunCmd( string cmd )
{
	char line[1024*512];


	FILE *fp;

	const char *sysCommand = cmd.data();
	if ((fp = popen(sysCommand, "r")) == NULL) {
		return "err";
	}
	string s = "";
	while (fgets(line, sizeof(line)-1, fp) != NULL){
		string lineStr = line;
		s += lineStr;
	}
	pclose(fp);

	return s;
}

