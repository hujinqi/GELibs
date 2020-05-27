/*
 * StringFormat.h
 *
 *  Created on: 2018年3月8日
 *      Author: carlos Hu
 */

#ifndef SRC_MODULE_COMMON_STRINGFORMAT_H_
#define SRC_MODULE_COMMON_STRINGFORMAT_H_

#include "defines.h"
#include "Errors.h"
#include <vector>
#include <set>
#include <string>
#include <typeinfo>

class SERVER_DECL StringFormat
{
public:
	template <typename T>
	static T StringToNumber ( const std::string &Text );
	template <typename T>
	static std::string NumberToString ( T Number );

	/**
	 * 字符串分割
	 */
//	template <typename T>
//	static void Split(const std::string& s, const std::string& delim, T& container);	//有错误

	static std::vector<std::string> Split(const std::string& s, const std::string& delim);
	static std::vector<int> Split2Int(const std::string& s, const std::string& delim);
	static std::vector< vector<int> > Split2IntVec(const std::string& s, const std::string& delim1, const std::string& delim2);

	/**
	 * 反分割，默认","分隔符
	 */
	static std::string Join(vector<int>& container, const char* delim=NULL);

	/**
	 * 字符串替换
	 */
	static std::string ReplaceAll(const string& source, const string& find, const string& replace);
	/**
	 * 解析ip
	 */
	static void GetIpFromUrl(const string& url, string& ip, int& port);
	/**
	 * 编码转换
	 */
	static string unicode_to_utf8(const string& instr);
	static string utf8_to_unicode(const string& instr);

private:
	static bool _unicode_to_utf8 (char *inbuf, size_t *inlen, char *outbuf, size_t *outlen);
	static bool _utf8_to_unicode (char *inbuf, size_t *inlen, char *outbuf, size_t *outlen);
	template <typename T>
	static void _pushSplitResult(string& res, T& container);
};

template<typename T>
T StringFormat::StringToNumber(const std::string &Text)
{
	std::istringstream ss(Text);
	T result;
	return ss >> result ? result : 0;
}

template <typename T>
std::string StringFormat::NumberToString ( T Number )
{
	std::ostringstream ss;
	ss << Number;
	return ss.str();
}

//template <typename T>
//void StringFormat::Split(const std::string& s, const std::string& delim, T& container)
//{
//	size_t last = 0;
//	size_t index=s.find_first_of(delim,last);
//	while (index!=std::string::npos)
//	{
//		string res = s.substr(last,index-last);
//		_pushSplitResult(res, container);
//		last=index+1;
//		index=s.find_first_of(delim,last);
//	}
//	if (index-last>0)
//	{
//		string res = s.substr(last,index-last);
//		_pushSplitResult(res, container);
//	}
//}

template <typename T>
void StringFormat::_pushSplitResult(string& res, T& container)
{
	if(std::is_same<T, std::vector<string> >::value)
	{
//		container.push_back(std::vector<string>::value_type(res));
	}
	else if(std::is_same<T, std::vector<int> >::value)
	{
//		container.push_back(StringToNumber<int>(res));
	}
	else if(std::is_same<T, std::set<string> >::value)
	{
//		container.insert(res);
	}
	else if(std::is_same<T, std::set<int> >::value)
	{
//		container.insert(StringToNumber<int>(res));
	}
	else
	{
		LOG(ERROR)<<"不支持的类型"<<typeid(T).name()<<endl;
		ASSERT(0);
	}
}

#endif /* SRC_MODULE_COMMON_STRINGFORMAT_H_ */
