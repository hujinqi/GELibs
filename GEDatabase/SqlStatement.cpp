#include "SqlStatement.h"

#include "common/StringFormat.h"

//initialiseSingleton( SqlStatement );
//void _SqlStatement_Link()
//{
//	sSqlState;
//}

//获得sql条件的字符串形式
string SqlColumn::GetSqlConditionStr()
{
	switch( mCondition )
	{
	case SQL_CONTION_ENUM_AND:
		return "and";
	case SQL_CONTION_ENUM_OR:
		return "or";
	}
	return "and";
}

string SqlColumn::GetValueStr()
{
	switch(mType)
	{
	case CVT_INT32:		return StringFormat::NumberToString<int32>(mInt32Value);
	case CVT_UINT32:	return StringFormat::NumberToString<uint32>(mUint32Value);
	case CVT_INT64:		return StringFormat::NumberToString<int64>(mInt64Value);
	case CVT_UINT64:	return StringFormat::NumberToString<uint64>(mUint64Value);
	case CVT_FLOAT:		return StringFormat::NumberToString<float>(mFloatValue);
	case CVT_DOUBLE:	return StringFormat::NumberToString<double>(mDoubleValue);
	case CVT_STRING:
		{
			return "'" + EscapeString(mStringValue) + "'";
		}
	};
	ASSERT(0);
	return "";
}

string SqlColumn::EscapeString(const string &str)
{
	size_t iSrcSize = str.size();
	std::string strDest;
	for (size_t i = 0; i < iSrcSize; i++)
	{
		char ch = str[i];
		switch (ch)
		{
		case '\0':
			strDest.append("\\0");
			break;
		case '\n':
			strDest.append("\\n");
			break;
		case '\r':
			strDest.append("\\r");
			break;
		case '\'':
			strDest.append("\\'");
			break;
		case '"':
			strDest.append("\\\"");
			break;
		case '\\':
			strDest.append("\\\\");
			break;
		case '%':
//			strDest.append("\\%");
			strDest.append("\%");
			break;
//		case '_':
//			strDest.append("\\_");
//			strDest.append("\_");
//			break;
		default:
			strDest.append(1, ch);
			break;
		}
	}

	return strDest;
}


void SqlStatement::ResetSql()
{
	mColumnVec.clear();
	mDupVec.clear();
	mWhereVec.clear();
	mTableName = "";
	mSqlStr = "";
}

void SqlStatement::SetTable( const char* tableName )
{
	ResetSql();
	mTableName = tableName;
}

void SqlStatement::AddValue( const char* columnName )
{
	SqlColumn s(columnName, CVT_INT32, 0 );
	mColumnVec.push_back(std::move(s));
}

void SqlStatement::AddValue( const char* columnName, int32 value )
{
	SqlColumn s(columnName, CVT_INT32, value );
	mColumnVec.push_back(std::move(s));
}

void SqlStatement::AddValue( const char* columnName, uint32 value )
{
	SqlColumn s(columnName, CVT_UINT32, value );
	mColumnVec.push_back(std::move(s));
}

void SqlStatement::AddValue( const char* columnName, int64 value )
{
	SqlColumn s(columnName, CVT_INT64, value );
	mColumnVec.push_back(std::move(s));
}

void SqlStatement::AddValue( const char* columnName, uint64 value )
{
	SqlColumn s(columnName, CVT_UINT64, value );
	mColumnVec.push_back(std::move(s));
}

void SqlStatement::AddValue( const char* columnName, float value )
{
	SqlColumn s(columnName, CVT_FLOAT, value );
	mColumnVec.push_back(std::move(s));
}

void SqlStatement::AddValue( const char* columnName, double value )
{
	SqlColumn s(columnName, CVT_DOUBLE, value );
	mColumnVec.push_back(std::move(s));
}

void SqlStatement::AddValue( const char* columnName, const char* value )
{
	SqlColumn s(columnName, value );
	mColumnVec.push_back(std::move(s));
}

void SqlStatement::AddValue( const char* columnName, const string& value )
{
	SqlColumn s(columnName, value.c_str() );
	mColumnVec.push_back(std::move(s));
}

void SqlStatement::AddValue( const string& columnName )
{
	AddValue(columnName.c_str());
}

void SqlStatement::AddValue( const string& columnName, int32 value )
{
	AddValue(columnName.c_str(), value);
}

void SqlStatement::AddValue( const string& columnName, uint32 value )
{
	AddValue(columnName.c_str(), value);
}

void SqlStatement::AddValue( const string& columnName, int64 value )
{
	AddValue(columnName.c_str(), value);
}

void SqlStatement::AddValue( const string& columnName, uint64 value )
{
	AddValue(columnName.c_str(), value);
}

void SqlStatement::AddValue( const string& columnName, float value )
{
	AddValue(columnName.c_str(), value);
}

void SqlStatement::AddValue( const string& columnName, double value )
{
	AddValue(columnName.c_str(), value);
}

void SqlStatement::AddValue( const string& columnName, const char* value )
{
	AddValue(columnName.c_str(), value);
}

void SqlStatement::AddValue( const string& columnName, const string& value )
{
	AddValue(columnName.c_str(), value);
}

void SqlStatement::AddDup( const char* columnName, int32 value )
{
	SqlColumn s(columnName, CVT_INT32, value );
	mDupVec.push_back(std::move(s));
}

void SqlStatement::AddDup( const char* columnName, uint32 value )
{
	SqlColumn s(columnName, CVT_UINT32, value );
	mDupVec.push_back(std::move(s));
}

void SqlStatement::AddDup( const char* columnName, int64 value )
{
	SqlColumn s(columnName, CVT_INT64, value );
	mDupVec.push_back(std::move(s));
}

void SqlStatement::AddDup( const char* columnName, uint64 value )
{
	SqlColumn s(columnName, CVT_UINT64, value );
	mDupVec.push_back(std::move(s));
}

void SqlStatement::AddDup( const char* columnName, float value )
{
	SqlColumn s(columnName, CVT_FLOAT, value );
	mDupVec.push_back(std::move(s));
}

void SqlStatement::AddDup( const char* columnName, double value )
{
	SqlColumn s(columnName, CVT_DOUBLE, value );
	mDupVec.push_back(std::move(s));
}

void SqlStatement::AddDup( const char* columnName, const char* value )
{
	SqlColumn s(columnName, value );
	mDupVec.push_back(std::move(s));
}

void SqlStatement::AddDup( const char* columnName, const string& value )
{
	SqlColumn s(columnName, value.c_str() );
	mDupVec.push_back(std::move(s));
}

void SqlStatement::AddDup( const string& columnName, int32 value )
{
	AddDup(columnName.c_str(), value);
}

void SqlStatement::AddDup( const string& columnName, uint32 value )
{
	AddDup(columnName.c_str(), value);
}

void SqlStatement::AddDup( const string& columnName, int64 value )
{
	AddDup(columnName.c_str(), value);
}

void SqlStatement::AddDup( const string& columnName, uint64 value )
{
	AddDup(columnName.c_str(), value);
}

void SqlStatement::AddDup( const string& columnName, float value )
{
	AddDup(columnName.c_str(), value);
}

void SqlStatement::AddDup( const string& columnName, double value )
{
	AddDup(columnName.c_str(), value);
}

void SqlStatement::AddDup( const string& columnName, const char* value )
{
	AddDup(columnName.c_str(), value);
}

void SqlStatement::AddDup( const string& columnName, const string& value )
{
	AddDup(columnName.c_str(), value);
}


void SqlStatement::AddWhere( const char* columnName, int32 value, const SqlContionEnum condition, const char* compare )
{
	SqlColumn s(columnName, CVT_INT32, value, condition, compare);
	mWhereVec.push_back(std::move(s));
}

void SqlStatement::AddWhere( const char* columnName, uint32 value, const SqlContionEnum condition, const char* compare )
{
	SqlColumn s(columnName, CVT_UINT32, value, condition, compare);
	mWhereVec.push_back(std::move(s));
}

void SqlStatement::AddWhere( const char* columnName, int64 value, const SqlContionEnum condition, const char* compare )
{
	SqlColumn s(columnName, CVT_INT64, value, condition, compare);
	mWhereVec.push_back(std::move(s));
}

void SqlStatement::AddWhere( const char* columnName, uint64 value, const SqlContionEnum condition, const char* compare )
{
	SqlColumn s(columnName, CVT_UINT64, value, condition, compare);
	mWhereVec.push_back(std::move(s));
}

void SqlStatement::AddWhere( const char* columnName, float value, const SqlContionEnum condition, const char* compare )
{
	SqlColumn s(columnName, CVT_FLOAT, value, condition, compare);
	mWhereVec.push_back(std::move(s));
}

void SqlStatement::AddWhere( const char* columnName, double value, const SqlContionEnum condition, const char* compare )
{
	SqlColumn s(columnName, CVT_DOUBLE, value, condition, compare);
	mWhereVec.push_back(std::move(s));
}

void SqlStatement::AddWhere( const char* columnName, const char* value, const SqlContionEnum condition, const char* compare )
{
	SqlColumn s(columnName, value, condition, compare);
	mWhereVec.push_back(std::move(s));
}

void SqlStatement::AddWhere( const char* columnName, const string& value, const SqlContionEnum condition, const char* compare )
{
	SqlColumn s(columnName, value.c_str(), condition, compare);
	mWhereVec.push_back(std::move(s));
}

void SqlStatement::AddWhere( const string& columnName, int32 value, const SqlContionEnum condition, const char* compare )
{
	AddWhere(columnName.c_str(), value, condition, compare);
}

void SqlStatement::AddWhere( const string& columnName, uint32 value, const SqlContionEnum condition, const char* compare )
{
	AddWhere(columnName.c_str(), value, condition, compare);
}

void SqlStatement::AddWhere( const string& columnName, int64 value, const SqlContionEnum condition, const char* compare )
{
	AddWhere(columnName.c_str(), value, condition, compare);
}

void SqlStatement::AddWhere( const string& columnName, uint64 value, const SqlContionEnum condition, const char* compare )
{
	AddWhere(columnName.c_str(), value, condition, compare);
}

void SqlStatement::AddWhere( const string& columnName, float value, const SqlContionEnum condition, const char* compare )
{
	AddWhere(columnName.c_str(), value, condition, compare);
}

void SqlStatement::AddWhere( const string& columnName, double value, const SqlContionEnum condition, const char* compare )
{
	AddWhere(columnName.c_str(), value, condition, compare);
}

void SqlStatement::AddWhere( const string& columnName, const char* value, const SqlContionEnum condition, const char* compare )
{
	AddWhere(columnName.c_str(), value, condition, compare);
}

void SqlStatement::AddWhere( const string& columnName, const string& value, const SqlContionEnum condition, const char* compare )
{
	AddWhere(columnName.c_str(), value, condition, compare);
}


void SqlStatement::AddWhere( const char* columnName, const int32 value, const char* compare )
{
	SqlColumn s(columnName, CVT_INT32, value, SQL_CONTION_ENUM_AND, compare);
	mWhereVec.push_back(std::move(s));
}

void SqlStatement::AddWhere( const char* columnName, const uint32 value, const char* compare )
{
	SqlColumn s(columnName, CVT_UINT32, value, SQL_CONTION_ENUM_AND, compare);
	mWhereVec.push_back(std::move(s));
}

void SqlStatement::AddWhere( const char* columnName, const int64 value, const char* compare )
{
	SqlColumn s(columnName, CVT_INT64, value, SQL_CONTION_ENUM_AND, compare);
	mWhereVec.push_back(std::move(s));
}

void SqlStatement::AddWhere( const char* columnName, const uint64 value,  const char* compare )
{
	SqlColumn s(columnName, CVT_UINT64, value, SQL_CONTION_ENUM_AND, compare);
	mWhereVec.push_back(std::move(s));
}

void SqlStatement::AddWhere( const char* columnName, const float value,  const char* compare )
{
	SqlColumn s(columnName, CVT_FLOAT, value, SQL_CONTION_ENUM_AND, compare);
	mWhereVec.push_back(std::move(s));
}

void SqlStatement::AddWhere( const char* columnName, const double value,  const char* compare )
{
	SqlColumn s(columnName, CVT_DOUBLE, value, SQL_CONTION_ENUM_AND, compare);
	mWhereVec.push_back(std::move(s));
}

void SqlStatement::AddWhere( const char* columnName, const char* value,  const char* compare )
{
	SqlColumn s(columnName, value, SQL_CONTION_ENUM_AND, compare);
	mWhereVec.push_back(std::move(s));
}

void SqlStatement::AddWhere( const char* columnName, const string& value,  const char* compare )
{
	SqlColumn s(columnName, value.c_str(), SQL_CONTION_ENUM_AND, compare);
	mWhereVec.push_back(std::move(s));
}

void SqlStatement::AddWhere( const string& columnName, const int32 value, const char* compare )
{
	AddWhere(columnName.c_str(), value, compare);
}

void SqlStatement::AddWhere( const string& columnName, const uint32 value, const char* compare )
{
	AddWhere(columnName.c_str(), value, compare);
}

void SqlStatement::AddWhere( const string& columnName, const int64 value, const char* compare  )
{
	AddWhere(columnName.c_str(), value, compare);
}

void SqlStatement::AddWhere( const string& columnName, const uint64 value,  const char* compare  )
{
	AddWhere(columnName.c_str(), value, compare);
}

void SqlStatement::AddWhere( const string& columnName, const float value,  const char* compare  )
{
	AddWhere(columnName.c_str(), value, compare);
}

void SqlStatement::AddWhere( const string& columnName, const double value,  const char* compare  )
{
	AddWhere(columnName.c_str(), value, compare);
}

void SqlStatement::AddWhere( const string& columnName, const char* value,  const char* compare  )
{
	AddWhere(columnName.c_str(), value, compare);
}

void SqlStatement::AddWhere( const string& columnName, const string& value,  const char* compare  )
{
	AddWhere(columnName.c_str(), value, compare);
}


//拼装where语句
void SqlStatement::PackWhereSql()
{
	int size = 0;
	mSqlStr.append(" where ");
	for( vector<SqlColumn>::iterator it = mWhereVec.begin(); it != mWhereVec.end(); ++it )
	{
		if( size != 0 )
		{
			mSqlStr.append(" ");
			mSqlStr.append(it->GetSqlConditionStr());
			mSqlStr.append(" ");
		}
		mSqlStr.append(it->mColumnName);
		mSqlStr.append(it->mCompare);
		mSqlStr.append(it->GetValueStr());
		++size;
	}
}

string SqlStatement::GetSelSqlStr()
{
	if( 0 == mColumnVec.size() )
	{
		mSqlStr = "select * from " + mTableName;
	}
	else
	{
		mSqlStr = "select ";
		int size = 0;
		for( vector<SqlColumn>::iterator it = mColumnVec.begin(); it != mColumnVec.end(); ++it )
		{
			++size;
			mSqlStr.append(it->mColumnName);
			if( size != (int)mColumnVec.size() )
			{
				mSqlStr.append(",");
			}
		}
		mSqlStr.append(" from ");
		mSqlStr.append(mTableName);
	}

	if( mWhereVec.size() > 0 )
	{
		PackWhereSql();
	}
	mSqlStr.append(";");
	return mSqlStr;
}

string SqlStatement::GetInsertSqlStr()
{
	mSqlStr = "insert into " + mTableName + "(";
	int size = 0;
	for( vector<SqlColumn>::iterator it = mColumnVec.begin(); it != mColumnVec.end(); ++it )
	{
		++size;
		mSqlStr += (*it).mColumnName;
		if( size != (int)mColumnVec.size() )
		{
			mSqlStr.append(",");
		}
		else
		{
			mSqlStr.append(")");
		}
	}
	mSqlStr.append(" values(");
	size = 0;
	for( vector<SqlColumn>::iterator it = mColumnVec.begin(); it != mColumnVec.end(); ++it )
	{
		++size;
		mSqlStr.append(it->GetValueStr());
		if( size != (int)mColumnVec.size() )
		{
			mSqlStr.append(",");
		}
		else
		{
			mSqlStr.append(")");
		}
	}
	mSqlStr.append(";");
	return mSqlStr;
}

//获得插入简单插入on dupicate update的sql
string SqlStatement::GetInsertDupSqlStr()
{
	mSqlStr = "insert into " + mTableName + "(";
	int size = 0;
	for( vector<SqlColumn>::iterator it = mColumnVec.begin(); it != mColumnVec.end(); ++it )
	{
		++size;
		mSqlStr.append(it->mColumnName);
		if( size != (int)mColumnVec.size() )
		{
			mSqlStr.append(",");
		}
		else
		{
			mSqlStr.append(")");
		}
	}
	mSqlStr.append(" values(");
	size = 0;
	for( vector<SqlColumn>::iterator it = mColumnVec.begin(); it != mColumnVec.end(); ++it )
	{
		++size;
		mSqlStr.append(it->GetValueStr());
		if( size != (int)mColumnVec.size() )
		{
			mSqlStr.append(",");
		}
		else
		{
			mSqlStr.append(")");
		}
	}
	if( mDupVec.size() > 0 )
	{
		int size = 0;
		mSqlStr += " ON DUPLICATE KEY UPDATE ";
		for( vector<SqlColumn>::iterator it = mDupVec.begin(); it != mDupVec.end(); ++it )
		{
			++size;
			mSqlStr.append(it->mColumnName);
			mSqlStr.append("=");
			mSqlStr.append(it->GetValueStr());
			if( size != (int)mDupVec.size() )
			{
				mSqlStr.append(",");
			}

		}
	}
	mSqlStr.append(";");
	return mSqlStr;
}

string SqlStatement::GetUpdateSqlStr()
{
	mSqlStr = "update " + mTableName + " set";

	int size = 0;
	for( vector<SqlColumn>::iterator it = mColumnVec.begin(); it != mColumnVec.end(); ++it )
	{
		++size;
		mSqlStr.append(" ");
		mSqlStr.append(it->mColumnName);
		mSqlStr.append(" = ");
		mSqlStr.append(it->GetValueStr());

		if( size != (int)mColumnVec.size() )
		{
			mSqlStr.append(",");
		}
	}

	if( mWhereVec.size() > 0 )
	{
		PackWhereSql();
	}
	mSqlStr.append(";");
	return mSqlStr;
}

string SqlStatement::GetDelSqlStr()
{
	//为了安全，不处理没有where 语句的删除，防止误操作，删除表内所有的数据
	if( mWhereVec.size() <= 0 )
	{
		mSqlStr = "";
		return mSqlStr;
	}
	mSqlStr = "delete from " + mTableName;
	PackWhereSql();
	mSqlStr.append(";");
	return mSqlStr;
}
