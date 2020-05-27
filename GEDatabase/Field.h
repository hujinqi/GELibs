/*
 * Field.h
 *
 *  Created on: 2018年8月3日
 *      Author: carlos
 */

#ifndef SRC_MODULE_GEDATABASE_FIELD_H_
#define SRC_MODULE_GEDATABASE_FIELD_H_

#include "common/defines.h"
#include <string>
#include <inttypes.h>

#ifdef WIN32
#else
#include <stdlib.h>
#endif

class Field
{
public:
	Field()
	{
		mValue = 0;
	}
	GE_INLINE void SetValue(char* value)
	{
		if( !value )
		{
			mValueStr = "";
		}
		else
		{
			mValueStr = value;
		}
		mValue = mValueStr.c_str();
	}

	GE_INLINE const char *GetString() { return mValue; }
	GE_INLINE float GetFloat() { return mValue ? static_cast<float>(atof(mValue)) : 0; }
	GE_INLINE double GetDouble() { return mValue ? static_cast<double>(atof(mValue)) : 0; }
	GE_INLINE bool GetBool() { return mValue ? atoi(mValue) > 0 : false; }
	GE_INLINE uint8 GetUInt8() { return mValue ? static_cast<uint8>(atol(mValue)) : 0; }
	GE_INLINE int8 GetInt8() { return mValue ? static_cast<int8>(atol(mValue)) : 0; }
	GE_INLINE uint16 GetUInt16() { return mValue ? static_cast<uint16>(atol(mValue)) : 0; }
	GE_INLINE uint32 GetUInt32() { return mValue ? static_cast<uint32>(atol(mValue)) : 0; }
	GE_INLINE uint32 GetInt32() { return mValue ? static_cast<int32>(atol(mValue)) : 0; }
	uint64 GetInt64()
	{
		if(mValue)
		{
			uint64 value;
			#ifndef WIN32	// Make GCC happy.
			sscanf(mValue, "%" PRId64 "", &value);
			#else
			sscanf_s(mValue, "%" PRId64 "", &value);
			#endif
			return value;
		}
		else
			return 0;
	}
	uint64 GetUInt64()
	{
		if(mValue)
		{
			uint64 value;
			#ifndef WIN32	// Make GCC happy.
			sscanf(mValue, "%" PRIu64 "", &value);
			#else
			sscanf_s(mValue, "%" PRIu64 "", &value);
			#endif
			return value;
		}
		else
			return 0;
	}

	std::string ToString() { return mValue; }

private:
		const char *mValue;
		string mValueStr;
};



#endif /* SRC_MODULE_GEDATABASE_FIELD_H_ */
