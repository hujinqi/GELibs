#pragma once 
#include "common/defines.h"
#include "common/Errors.h"
#include "common/Singleton.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

enum ValueType
{
	CVT_INT32		=	1,		//int32
	CVT_UINT32		=	2,		//uint32
	CVT_INT64		=	3,		//int64
	CVT_UINT64		=	4,		//uint64
	CVT_FLOAT		=	5,		//float
	CVT_DOUBLE		=	6,		//double
	CVT_STRING		=	7,		//string
};

//sql条件
enum SqlContionEnum
{
	SQL_CONTION_ENUM_AND,
	SQL_CONTION_ENUM_OR
};

struct SqlColumn
{
	string mColumnName;			//列名
	ValueType mType;			//数值类型

	//数值
	union {
		int32	mInt32Value;
		uint32	mUint32Value;
		int64	mInt64Value;
		uint64	mUint64Value;
		float	mFloatValue;
		double	mDoubleValue;
	};
	string	mStringValue;
	SqlContionEnum mCondition;	//条件
	string mCompare;			//比较(大于，等于，或者小于)

	template <typename T> SqlColumn(const char* columnName, ValueType type, T value, SqlContionEnum condition = SQL_CONTION_ENUM_AND, const char* compare = "=");
	SqlColumn(const char* columnName, const char* value, SqlContionEnum condition = SQL_CONTION_ENUM_AND, const char* compare = "=")
	{
		mColumnName = columnName;
		mType = CVT_STRING;
		mUint64Value = 0;
		mCondition = condition;
		mCompare = compare;
		mStringValue = value;
	}
	SqlColumn(const char* columnName, const string& value, SqlContionEnum condition = SQL_CONTION_ENUM_AND, const char* compare = "=")
	{
		mColumnName = columnName;
		mType = CVT_STRING;
		mUint64Value = 0;
		mCondition = condition;
		mCompare = compare;
		mStringValue = value;
	}

	string GetValueStr();
	//获得sql条件的字符串形式
	string GetSqlConditionStr();

	//字符串转义
	static string EscapeString(const string &str);
};

template <typename T>
SqlColumn::SqlColumn(const char* columnName, ValueType type, T value, SqlContionEnum condition, const char* compare):
mColumnName(columnName),
mType(type),
mUint64Value(0),
mCondition(condition),
mCompare(compare)
{
	if( type == CVT_STRING )
	{
		ASSERT(0);
	}
	switch(type)
	{
	case CVT_INT32:		mInt32Value = (int32)value;break;
	case CVT_UINT32:	mUint32Value = (uint32)value;break;
	case CVT_INT64:		mInt64Value = (int64)value;break;
	case CVT_UINT64:	mUint64Value = (uint64)value;break;
	case CVT_FLOAT:		mFloatValue = (float)value;break;
	case CVT_DOUBLE:	mDoubleValue = (double)value;break;
	default:;
	}
}

//SqlColumn::SqlColumn(string columnName, string value, SqlContionEnum condition, string compare)
//{
//	mColumnName = columnName;
//	mType = CVT_STRING;
//	mUint64Value = 0;
//	mCondition = condition;
//	mCompare = compare;
//	mStringValue = value;
//}

class SERVER_DECL SqlStatement
{
public:
	void ResetSql();
	void SetTable( const char* tableName );

	void AddValue( const char* columnName );
	void AddValue( const char* columnName, int32 value );
	void AddValue( const char* columnName, uint32 value );
	void AddValue( const char* columnName, int64 value );
	void AddValue( const char* columnName, uint64 value );
	void AddValue( const char* columnName, float value );
	void AddValue( const char* columnName, double value );
	void AddValue( const char* columnName, const char* value );
	void AddValue( const char* columnName, const string& value );
	void AddValue( const string& columnName );
	void AddValue( const string& columnName, int32 value );
	void AddValue( const string& columnName, uint32 value );
	void AddValue( const string& columnName, int64 value );
	void AddValue( const string& columnName, uint64 value );
	void AddValue( const string& columnName, float value );
	void AddValue( const string& columnName, double value );
	void AddValue( const string& columnName, const char* value );
	void AddValue( const string& columnName, const string& value );
	
	void AddDup( const char* columnName, int32 value );
	void AddDup( const char* columnName, uint32 value );
	void AddDup( const char* columnName, int64 value );
	void AddDup( const char* columnName, uint64 value );
	void AddDup( const char* columnName, float value );
	void AddDup( const char* columnName, double value );
	void AddDup( const char* columnName, const char* value );
	void AddDup( const char* columnName, const string& value );
	void AddDup( const string& columnName, int32 value );
	void AddDup( const string& columnName, uint32 value );
	void AddDup( const string& columnName, int64 value );
	void AddDup( const string& columnName, uint64 value );
	void AddDup( const string& columnName, float value );
	void AddDup( const string& columnName, double value );
	void AddDup( const string& columnName, const char* value );
	void AddDup( const string& columnName, const string& value );

	void AddWhere( const char* columnName, int32 value, const char* compare = "=" );
	void AddWhere( const char* columnName, uint32 value, const char* compare = "=" );
	void AddWhere( const char* columnName, int64 value, const char* compare = "="  );
	void AddWhere( const char* columnName, uint64 value,  const char* compare = "="  );
	void AddWhere( const char* columnName, float value,  const char* compare = "="  );
	void AddWhere( const char* columnName, double value,  const char* compare = "="  );
	void AddWhere( const char* columnName, const char* value,  const char* compare = "="  );
	void AddWhere( const char* columnName, const string& value,  const char* compare = "="  );
	void AddWhere( const string& columnName, int32 value, const char* compare = "=" );
	void AddWhere( const string& columnName, uint32 value, const char* compare = "=" );
	void AddWhere( const string& columnName, int64 value, const char* compare = "="  );
	void AddWhere( const string& columnName, uint64 value,  const char* compare = "="  );
	void AddWhere( const string& columnName, float value,  const char* compare = "="  );
	void AddWhere( const string& columnName, double value,  const char* compare = "="  );
	void AddWhere( const string& columnName, const char* value,  const char* compare = "="  );
	void AddWhere( const string& columnName, const string& value,  const char* compare = "="  );

	void AddWhere( const char* columnName, int32 value, const SqlContionEnum condition, const char* compare );
	void AddWhere( const char* columnName, uint32 value, const SqlContionEnum condition, const char* compare );
	void AddWhere( const char* columnName, int64 value, const SqlContionEnum condition, const char* compare );
	void AddWhere( const char* columnName, uint64 value, const SqlContionEnum condition, const char* compare );
	void AddWhere( const char* columnName, float value, const SqlContionEnum condition, const char* compare );
	void AddWhere( const char* columnName, const double value, const SqlContionEnum condition, const char* compare );
	void AddWhere( const char* columnName, const char* value, const SqlContionEnum condition, const char* compare );
	void AddWhere( const char* columnName, const string& value, const SqlContionEnum condition, const char* compare );
	void AddWhere( const string& columnName, int32 value, const SqlContionEnum condition, const char* compare );
	void AddWhere( const string& columnName, uint32 value, const SqlContionEnum condition, const char* compare );
	void AddWhere( const string& columnName, int64 value, const SqlContionEnum condition, const char* compare );
	void AddWhere( const string& columnName, uint64 value, const SqlContionEnum condition, const char* compare );
	void AddWhere( const string& columnName, float value, const SqlContionEnum condition, const char* compare );
	void AddWhere( const string& columnName, double value, const SqlContionEnum condition, const char* compare );
	void AddWhere( const string& columnName, const char* value, const SqlContionEnum condition, const char* compare );
	void AddWhere( const string& columnName, const string& value, const SqlContionEnum condition, const char* compare );

	//获得简单查询的sql
	string GetSelSqlStr();

	//获得简单插入的sql
	string GetInsertSqlStr();

	//获得简单修改的sql
	string GetUpdateSqlStr();

	//获得简单删除的sql
	string GetDelSqlStr();

	//获得插入简单插入on dupicate update的sql
	string GetInsertDupSqlStr();

private:
	//拼装where语句
	void PackWhereSql();

private:
	string mSqlStr;

	string mTableName;			//表格名字

	vector<SqlColumn> mColumnVec;		//存放所有需要修改的value的列值
	vector<SqlColumn> mWhereVec;		//存放所有的条件判断语句的sql
	vector<SqlColumn> mDupVec;			//存放所有重复主键的vec
};

#define sSqlState SqlStatement::getSingleton()
