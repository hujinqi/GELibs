/*base_64.cpp文件*/
#include <iostream>
#include <string>
#include <cstring>
#include "Base64.h"
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
using namespace boost::archive::iterators;

bool Base64::Encode(const string & input, string * output)
{
	typedef base64_from_binary<transform_width<string::const_iterator, 6, 8>> Base64EncodeIterator;
	stringstream result;
	try {
		copy( Base64EncodeIterator( input.begin() ), Base64EncodeIterator( input.end() ), ostream_iterator<char>( result ) );
	} catch ( ... ) {
		return false;
	}
	size_t equal_count = (3 - input.length() % 3) % 3;
	for ( size_t i = 0; i < equal_count; i++ )
	{
		result.put( '=' );
	}
	*output = result.str();
	return output->empty() == false;
}

bool Base64::Decode(const string & input, string * output)
{
	typedef transform_width<binary_from_base64<string::const_iterator>, 8, 6> Base64DecodeIterator;
	stringstream result;
	try {
		copy( Base64DecodeIterator( input.begin() ), Base64DecodeIterator( input.end() ), ostream_iterator<char>( result ) );
	} catch ( ... ) {
		return false;
	}

	*output = result.str();

	//尾部可能有0
	int lostCount = 0;
	for(int i=output->size() - 1; i >= 0; --i)
	{
//		cout<<"char:"<<(int)output->c_str()[i]<<endl;
		if((int)output->c_str()[i] != 0)
		{
			break;
		}
		lostCount++;
	}
//	cout<<"lostCount:"<<lostCount<<endl;
	if(lostCount > 0)
	{
		*output = output->substr(0, output->size() - lostCount);
	}

	return output->empty() == false;
}

//std::string Base64::Encode(const unsigned char * str,int bytes) {
//    int num = 0,bin = 0,i;
//    std::string _encode_result;
//    const unsigned char * current;
//    current = str;
//    while(bytes > 2) {
//        _encode_result += _base64_table[current[0] >> 2];
//        _encode_result += _base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
//        _encode_result += _base64_table[((current[1] & 0x0f) << 2) + (current[2] >> 6)];
//        _encode_result += _base64_table[current[2] & 0x3f];
//
//        current += 3;
//        bytes -= 3;
//    }
//    if(bytes > 0)
//    {
//        _encode_result += _base64_table[current[0] >> 2];
//        if(bytes%3 == 1) {
//            _encode_result += _base64_table[(current[0] & 0x03) << 4];
//            _encode_result += "==";
//        } else if(bytes%3 == 2) {
//            _encode_result += _base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
//            _encode_result += _base64_table[(current[1] & 0x0f) << 2];
//            _encode_result += "=";
//        }
//    }
//    return _encode_result;
//}
//std::string Base64::Decode(const char *str,int length) {
//       //解码表
//    const char DecodeTable[] =
//    {
//        -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -1, -2, -2, -1, -2, -2,
//        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
//        -1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 62, -2, -2, -2, 63,
//        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -2, -2, -2, -2, -2, -2,
//        -2,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
//        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -2, -2, -2, -2, -2,
//        -2, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
//        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -2, -2, -2, -2, -2,
//        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
//        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
//        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
//        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
//        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
//        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
//        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
//        -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2
//    };
//    int bin = 0,i=0,pos=0;
//    std::string _decode_result;
//    const char *current = str;
//    char ch;
//    while( (ch = *current++) != '\0' && length-- > 0 )
//    {
//        if (ch == base64_pad) { // 当前一个字符是“=”号
//            /*
//            先说明一个概念：在解码时，4个字符为一组进行一轮字符匹配。
//            两个条件：
//                1、如果某一轮匹配的第二个是“=”且第三个字符不是“=”，说明这个带解析字符串不合法，直接返回空
//                2、如果当前“=”不是第二个字符，且后面的字符只包含空白符，则说明这个这个条件合法，可以继续。
//            */
//            if (*current != '=' && (i % 4) == 1) {
//                return NULL;
//            }
//            continue;
//        }
//        ch = DecodeTable[ch];
//        //这个很重要，用来过滤所有不合法的字符
//        if (ch < 0 ) { /* a space or some other separator character, we simply skip over */
//            continue;
//        }
//        switch(i % 4)
//        {
//            case 0:
//                bin = ch << 2;
//                break;
//            case 1:
//                bin |= ch >> 4;
//                _decode_result += bin;
//                bin = ( ch & 0x0f ) << 4;
//                break;
//            case 2:
//                bin |= ch >> 2;
//                _decode_result += bin;
//                bin = ( ch & 0x03 ) << 6;
//                break;
//            case 3:
//                bin |= ch;
//                _decode_result += bin;
//                break;
//        }
//        i++;
//    }
//    return _decode_result;
//}
