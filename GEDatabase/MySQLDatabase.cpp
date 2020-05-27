/*
 * MySQLDatabase.cpp
 *
 *  Created on: 2018年8月3日
 *      Author: carlos
 */

#include "MySQLDatabase.h"
#include <glog/logging.h>
#include <mysql/mysqld_error.h>
#include <mysql/errmsg.h>
#include "GETime/TimeMgr.h"

MySQLQueryResult::MySQLQueryResult(MYSQL_RES* res, uint32 FieldCount, uint32 RowCount) : QueryResult(FieldCount, RowCount)
{
	mTotalField = new Field[FieldCount * RowCount];
	mCurrentRow = mTotalField;
	mNowRowIndex = -1;

	//init data
	int index = 0;
	MYSQL_ROW nowRow = NULL;
	for(uint32 i=0; i < RowCount; ++i)
	{
		nowRow = mysql_fetch_row(res);
		ASSERT(nowRow);
		for(uint32 j=0; j < FieldCount; ++j)
		{
//			DLOG( INFO ) << nowRow[j] << "  111";

			mTotalField[index++].SetValue(nowRow[j]);
//			DLOG( INFO ) << mTotalField[index-1].GetString() << "  222";
		}
	}

//	exit( 0 );

//	LOG( INFO )<<"### FieldCount:"<<FieldCount<<" RowCount:"<<RowCount<<" index:"<<index<<endl;
}

MySQLQueryResult::~MySQLQueryResult()
{
//	LOG( INFO )<<"### ~MySQLQueryResult():" << endl;
	delete [] mTotalField;
}

bool MySQLQueryResult::NextRow()
{
	mNowRowIndex++;
	if(mNowRowIndex >= mRowCount)
		return false;

	mCurrentRow = mTotalField + mFieldCount * mNowRowIndex;
//	DLOG( INFO )<<"### mysqlresult diff:"<<(mCurrentRow - mTotalField)<<endl;
	return true;
}

MySQLDatabase::~MySQLDatabase()
{
	for(int32 i = 0; i < mConnectionCount; ++i)
	{
		mysql_close(((MySQLDatabaseConnection*)Connections[i])->MySql);
		delete Connections[i];
	}
	delete [] Connections;
}

MySQLDatabase::MySQLDatabase() : Database()
{

}

bool MySQLDatabase::Initialize(const char* Hostname, unsigned int port, const char* Username, const char* Password, const char* DatabaseName, uint32 ConnectionCount, uint32 BufferSize)
{
	uint32 i;
	MYSQL * temp = NULL;
	MYSQL * temp2 = NULL;
	MySQLDatabaseConnection ** conns;
	my_bool my_true = true;

	mHostname = string(Hostname);
	mConnectionCount = ConnectionCount;
	mUsername = string(Username);
	mPassword = string(Password);
	mDatabaseName = string(DatabaseName);

	LOG(INFO) << "Connecting to `" << Hostname << "`, database `" << DatabaseName << "`..." << endl;

	conns = new MySQLDatabaseConnection*[ConnectionCount];
	Connections = ((DatabaseConnection**)conns);
	for( i = 0; i < ConnectionCount; ++i )
	{
		temp = mysql_init( NULL );
		if( temp == NULL )
			continue;

		if(mysql_options(temp, MYSQL_SET_CHARSET_NAME, "utf8"))
			LOG(ERROR) << "Could not set utf8 character set." << endl;

		if (mysql_options(temp, MYSQL_OPT_RECONNECT, &my_true))
			LOG(ERROR) << "MYSQL_OPT_RECONNECT could not be set, connection drops may occur but will be counteracted." << endl;

		int readTimeOut = 5;
		if(mysql_options(temp, MYSQL_OPT_READ_TIMEOUT, &readTimeOut))
					LOG(ERROR) << "Could not set read time out." << endl;

		//temp2 = mysql_real_connect( temp, Hostname, Username, Password, DatabaseName, port, NULL, CLIENT_MULTI_STATEMENTS|CLIENT_MULTI_RESULTS);
		temp2 = mysql_real_connect( temp, Hostname, Username, Password, DatabaseName, port, NULL, 0 );
		if( temp2 == NULL )
		{
			LOG(ERROR) << "Connection failed due to: `" << mysql_error( temp ) << "`" <<endl;
			mysql_close(temp);
			return false;
		}

		mysql_query( temp, "SET NAMES UTF8");
		mysql_set_character_set( temp, "utf8" );

		conns[i] = new MySQLDatabaseConnection(i+1);
		conns[i]->MySql = temp2;
	}

//	Database::_Initialize();
	return true;
}

string MySQLDatabase::EscapeString(string Escape)
{
	char a2[16384] = { 0 };

	DatabaseConnection * con = GetFreeConnection();
	string ret;
	if(mysql_real_escape_string(static_cast<MySQLDatabaseConnection*>(con)->MySql, a2, Escape.c_str(), (unsigned long)Escape.length()) == 0)
		ret = Escape.c_str();
	else
		ret = a2;

//	con->Busy.Release();
	con->Release();

	return string(ret);
}

void MySQLDatabase::EscapeLongString(const char * str, uint32 len, stringstream& out)
{
	char a2[65536*3] = { 0 };

	DatabaseConnection * con = GetFreeConnection();
	const char * ret;
	if(mysql_real_escape_string(static_cast<MySQLDatabaseConnection*>(con)->MySql, a2, str, (unsigned long)len) == 0)
		ret = str;
	else
		ret = a2;

	out.write(ret, (std::streamsize)strlen(ret));
//	con->Busy.Release();
	con->Release();
}

string MySQLDatabase::EscapeString(const char * esc, DatabaseConnection * con)
{
	char a2[16384] = { 0 };
	const char * ret;
	if(mysql_real_escape_string(static_cast<MySQLDatabaseConnection*>(con)->MySql, a2, (char*)esc, (unsigned long)strlen(esc)) == 0)
		ret = esc;
	else
		ret = a2;

	return string(ret);
}

void MySQLDatabase::OnShutdown()
{
	// TODO
}

bool MySQLDatabase::_HandleError(MySQLDatabaseConnection * con, uint32 ErrorNumber)
{
	// Handle errors that should cause a reconnect to the Database.
	LOG(WARNING) << "mysql connecting id:"<<con->GetId()<<" error:" << ErrorNumber <<endl;

	switch (ErrorNumber)
	{
	case CR_INVALID_CONN_HANDLE:
		{
			if(con->MySql)
			{
				LOG(ERROR) << "Lost the connection to the MySQL server!" << endl;
				mysql_close(con->MySql);
				con->MySql = NULL;
			}
		}
		/* no break */
	case CR_OUT_OF_MEMORY:	//Client ran out of memory
	case CR_SERVER_LOST:	//Lost connection to sql server during query
	case CR_SERVER_LOST_EXTENDED:	//Lost connection to sql server - system error
	case CR_SERVER_GONE_ERROR:	//Mysql server has gone away
	case CR_CONN_HOST_ERROR:
		{
			LOG(WARNING) << "Attempting to reconnect to the MySQL server..." << endl;
			return _Reconnect( con );
		}
		break;
	case ER_LOCK_DEADLOCK:
		return false;
	case ER_WRONG_VALUE_COUNT:
	case ER_DUP_ENTRY:
		return false;
	}
	return false;
}

bool MySQLDatabase::_SendQuery(DatabaseConnection *con, const char* Sql, bool Self)
{
	//清空之前的结果
	do
	{
		MYSQL_RES* res = mysql_store_result(static_cast<MySQLDatabaseConnection*>(con)->MySql);
		mysql_free_result(res);
	}while ( (0 == mysql_next_result(static_cast<MySQLDatabaseConnection*>(con)->MySql)) );
	//dunno what it does ...leaving untouched
//	DLOG(INFO)<<"mysql_query. sql:"<<Sql<<endl;

	time_t begin = sTime.MSTime();
	int result = mysql_query(static_cast<MySQLDatabaseConnection*>(con)->MySql, Sql);
//	DLOG(INFO)<<"mysql_query ret : "<<result<<" Self:"<<Self<<endl;
	time_t cost = sTime.MSTime() - begin;
	if(cost > 1000){
		LOG(WARNING)<<"数据库花费时间过长 cost:"<<cost<<" ms, result:"<<result<<", sql:["<<Sql<<"]"<<endl;
	}

	if(result > 0)
	{
		if( Self == false && _HandleError(static_cast<MySQLDatabaseConnection*>(con), mysql_errno( static_cast<MySQLDatabaseConnection*>(con)->MySql ) ) )
		{
			// Re-send the query, the connection was successful.
			// The true on the end will prevent an endless loop here, as it will
			// stop after sending the query twice.
			bool succ = _SendQuery(con, Sql, true);
			if(succ)
			{
				result = 0;	//第二次成功了
			}
		}
		else
			LOG(WARNING) << "Sql query failed due to [" << mysql_error( static_cast<MySQLDatabaseConnection*>(con)->MySql ) << "], Query: [" << Sql << " ]" << endl;
	}

//	DLOG(INFO)<<"ret result:"<<result<<" to bool:"<<(result == 0 ? true : false)<<endl;
	return (result == 0 ? true : false);
}

QueryResult * MySQLDatabase::_StoreQueryResult(DatabaseConnection * con)
{
	MySQLQueryResult * res;
	MySQLDatabaseConnection * db = static_cast<MySQLDatabaseConnection*>(con);
	MYSQL_RES * pRes = mysql_store_result( db->MySql );
	uint32 uRows = (uint32)mysql_affected_rows( db->MySql );
	uint32 uFields = (uint32)mysql_field_count( db->MySql );
//	DLOG(INFO)<<"_StoreQueryResult uRows:"<<uRows<<endl;

	if( uRows == 0 || uFields == 0 || pRes == 0 )
	{
		if( pRes != NULL )
		{
			mysql_free_result( pRes );
		}

		return NULL;
	}

	res = new MySQLQueryResult( pRes, uFields, uRows );
	mysql_free_result(pRes);
//	res->NextRow();

	return res;
}

bool MySQLDatabase::_Reconnect(MySQLDatabaseConnection * conn)
{
	MYSQL * temp, *temp2;

	temp = mysql_init( NULL );
	temp2 = mysql_real_connect( temp, mHostname.c_str(), mUsername.c_str(), mPassword.c_str(), mDatabaseName.c_str(), mPort, NULL , 0 );
	if( temp2 == NULL )
	{
		LOG(ERROR) << "Could not reconnect to database because of `" << mysql_error( temp ) << "`" << endl;
		mysql_close( temp );
		return false;
	}

	if( conn->MySql != NULL )
		mysql_close( conn->MySql );

	conn->MySql = temp;
	return true;
}

void MySQLDatabase::_BeginTransaction(DatabaseConnection * conn)
{
	_SendQuery( conn, "START TRANSACTION", false );
}

void MySQLDatabase::_EndTransaction(DatabaseConnection * conn)
{
	_SendQuery( conn, "COMMIT", false );
}
