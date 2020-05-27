/*
 * StringFormat.cpp
 *
 *  Created on: 2018年3月8日
 *      Author: carlos Hu
 */

#include "StringFormat.h"
#include <inttypes.h>
#include <iconv.h>

std::vector<std::string> StringFormat::Split(const std::string& s,const std::string& delim)
{
    std::vector<std::string> ret;
    if(s.size() == 0)
    {
    	return ret;
    }

    size_t last = 0;
    size_t index=s.find_first_of(delim,last);
    while (index!=std::string::npos)
    {
        ret.push_back(s.substr(last,index-last));
        last=index+1;
        index=s.find_first_of(delim,last);
    }
    if (index-last>0)
    {
        ret.push_back(s.substr(last,index-last));
    }
    return ret;
}

std::vector<int> StringFormat::Split2Int(const std::string& s, const std::string& delim)
{
	std::vector<int> ret;
	std::vector<std::string> strVec = Split(s, delim);
	for(auto it=strVec.begin(); it != strVec.end(); ++it)
	{
		ret.push_back( StringToNumber<int>(*it) );
	}
	return ret;
}

std::vector< vector<int> > StringFormat::Split2IntVec(const std::string& s, const std::string& delim1, const std::string& delim2)
{
	std::vector< vector<int> > ret;
	std::vector<std::string> strVec = Split(s, delim1);
	for(auto it=strVec.begin(); it != strVec.end(); ++it)
	{
		vector<int> tmpRet;
		std::vector<std::string> tmpVec = Split(*it, delim2);
		for(auto it2=tmpVec.begin(); it2 != tmpVec.end(); ++it2)
		{
			tmpRet.push_back( StringToNumber<int>(*it2) );
		}
		ret.push_back(std::move(tmpRet));
	}
	return ret;
}

std::string StringFormat::ReplaceAll(const string& source, const string& oldValue, const string& newValue)
{
	string ret(source);
	for(string::size_type pos(0); pos != string::npos; pos+=newValue.length())
	{
		if((pos=ret.find(oldValue, pos)) != string::npos)
			ret.replace(pos, oldValue.length(), newValue);
		else
			break;
	}

	return ret;
}

void StringFormat::GetIpFromUrl(const string& url, string& ip, int& port)
{
	ip.clear();
	port = 0;

	std::vector<string> urlVec_1 = Split(url, "/");
//	for(auto it=urlVec_1.begin(); it != urlVec_1.end(); ++it){
//		cout<<*it<<endl;
//	}
	if(urlVec_1.size() == 0){
		return;
	}

	int index = 2;
	if(urlVec_1.size() < 3){
		index = urlVec_1.size() - 1;
	}
	string address = urlVec_1[ index ];
//	cout<<"address:"<<address<<endl;
	std::vector<string> addressVec = Split(address, ":");
	ip = addressVec[0];
	if(addressVec.size() > 1){
		port = StringToNumber<int>(addressVec[1]);
	}
}

bool StringFormat::_unicode_to_utf8(char *inbuf, size_t *inlen, char *outbuf, size_t *outlen)
{
	/* 目的编码, TRANSLIT：遇到无法转换的字符就找相近字符替换
	 *           IGNORE ：遇到无法转换字符跳过*/
	const char *encTo = "UTF-8//IGNORE";
	/* 源编码 */
	const char *encFrom = "UNICODE";

	/* 获得转换句柄
	 *@param encTo 目标编码方式
	 *@param encFrom 源编码方式
	 *
	 * */
	iconv_t cd = iconv_open(encTo, encFrom);
	if (cd == (iconv_t) -1) {
//		perror("iconv_open");
		LOG(ERROR)<<"iconv_open failed! errno:"<<errno<<endl;
	}

	/* 需要转换的字符串 */
//	printf("inbuf=%s\n", inbuf);

	/* 打印需要转换的字符串的长度 */
//	printf("inlen=%d\n", *inlen);

	/* 由于iconv()函数会修改指针，所以要保存源指针 */
	char *tmpin = inbuf;
	char *tmpout = outbuf;
//	size_t insize = *inlen;
//	size_t outsize = *outlen;

	/* 进行转换
	 *@param cd iconv_open()产生的句柄
	 *@param srcstart 需要转换的字符串
	 *@param inlen 存放还有多少字符没有转换
	 *@param tempoutbuf 存放转换后的字符串
	 *@param outlen 存放转换后,tempoutbuf剩余的空间
	 *
	 * */
	size_t ret = iconv(cd, &tmpin, inlen, &tmpout, outlen);
	if (ret != 0) {
//		perror("iconv");
		LOG(ERROR)<<"iconv failed! errno:"<<errno<<endl;
	}

//	/* 存放转换后的字符串 */
//	printf("outbuf=%s\n", outbuf);
//
//	//存放转换后outbuf剩余的空间
//	printf("outlen=%d\n", *outlen);
//
//	int i = 0;
//
//	for (i = 0; i < (outsize - (*outlen)); i++) {
//		//printf("%2c", outbuf[i]);
//		printf("%x\n", outbuf[i]);
//	}

	/* 关闭句柄 */
	iconv_close(cd);

	return 0;
}

bool StringFormat::_utf8_to_unicode (char *inbuf, size_t *inlen, char *outbuf, size_t *outlen)
{

	/* 目的编码, TRANSLIT：遇到无法转换的字符就找相近字符替换
	 *           IGNORE ：遇到无法转换字符跳过*/
	const char *encTo = "UNICODE//IGNORE";
	/* 源编码 */
	const char *encFrom = "UTF-8";

	/* 获得转换句柄
	 *@param encTo 目标编码方式
	 *@param encFrom 源编码方式
	 *
	 * */
	iconv_t cd = iconv_open(encTo, encFrom);
	if (cd == (iconv_t) -1) {
//		perror("iconv_open");
		LOG(ERROR)<<"iconv_open failed! errno:"<<errno<<endl;
	}

//	/* 需要转换的字符串 */
//	printf("inbuf=%s\n", inbuf);
//
//	/* 打印需要转换的字符串的长度 */
//	printf("inlen=%d\n", *inlen);

	/* 由于iconv()函数会修改指针，所以要保存源指针 */
	char *tmpin = inbuf;
	char *tmpout = outbuf;
//	size_t insize = *inlen;
//	size_t outsize = *outlen;

	/* 进行转换
	 *@param cd iconv_open()产生的句柄
	 *@param srcstart 需要转换的字符串
	 *@param inlen 存放还有多少字符没有转换
	 *@param tempoutbuf 存放转换后的字符串
	 *@param outlen 存放转换后,tempoutbuf剩余的空间
	 *
	 * */
	size_t ret = iconv(cd, &tmpin, inlen, &tmpout, outlen);
	if (ret != 0) {
//		perror("iconv");
		LOG(ERROR)<<"iconv failed! errno:"<<errno<<endl;
	}

//	/* 存放转换后的字符串 */
//	printf("outbuf=%s\n", outbuf);
//
//	//存放转换后outbuf剩余的空间
//	printf("outlen=%d\n", *outlen);
//
//	int i = 0;
//
//	for (i = 0; i < (outsize - (*outlen)); i++) {
//		//printf("%2c", outbuf[i]);
//		printf("%x\n", outbuf[i]);
//	}

	/* 关闭句柄 */
	iconv_close(cd);

	return 0;
}

string StringFormat::unicode_to_utf8(const string& str)
{
	size_t inlen = str.length();
	if(inlen > 65535)
	{
		return "";
	}

	char inbuf[65535] = {};
	memcpy(inbuf, str.c_str(), inlen);
	char outbuf[65535] = {};
	size_t outlen = 65535;
	_unicode_to_utf8(inbuf, &inlen, outbuf, &outlen);
	return string(outbuf);
}

string StringFormat::utf8_to_unicode(const string& str)
{
	size_t inlen = str.length();
	if(inlen > 65535)
	{
		return "";
	}

	char inbuf[65535] = {};
	memcpy(inbuf, str.c_str(), inlen);
	char outbuf[65535] = {};
	size_t outlen = 65535;
	_utf8_to_unicode(inbuf, &inlen, outbuf, &outlen);
	return string(outbuf);
}

std::string StringFormat::Join(vector<int>& container, const char* delim)
{
	string ret;
	for(auto it=container.begin(); it != container.end(); ++it)
	{
		if(ret != "")
		{
			if(delim)
				ret.append(delim);
			else
				ret.append(",");
		}
		ret.append( NumberToString(*it) );
	}

	return ret;
}
