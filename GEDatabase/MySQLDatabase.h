/*
 * MySQLDatabase.h
 *
 *  Created on: 2018年8月3日
 *      Author: carlos
 */

#ifndef SRC_MODULE_GEDATABASE_MYSQLDATABASE_H_
#define SRC_MODULE_GEDATABASE_MYSQLDATABASE_H_

#include <mysql/mysql.h>
#include "Database.h"

#ifdef WIN32
#pragma comment(lib, "libmysql.lib")
#endif

class MySQLDatabaseConnection : public DatabaseConnection
{
public:
	MySQLDatabaseConnection(int id):DatabaseConnection(id),MySql(NULL) {}
	MYSQL * MySql;
};

class SERVER_DECL MySQLDatabase : public Database
{
public:
	MySQLDatabase();
	~MySQLDatabase();

	bool Initialize(const char* Hostname, unsigned int port,
		const char* Username, const char* Password, const char* DatabaseName,
		uint32 ConnectionCount, uint32 BufferSize);

	void OnShutdown();

	string EscapeString(string Escape);
	void EscapeLongString(const char * str, uint32 len, stringstream& out);
	string EscapeString(const char * esc, DatabaseConnection * con);

protected:

	bool _HandleError(MySQLDatabaseConnection*, uint32 ErrorNumber);
	bool _SendQuery(DatabaseConnection *con, const char* Sql, bool Self = false);

	void _BeginTransaction(DatabaseConnection * conn);
	void _EndTransaction(DatabaseConnection * conn);
	bool _Reconnect(MySQLDatabaseConnection * conn);

	QueryResult * _StoreQueryResult(DatabaseConnection * con);
};

class SERVER_DECL MySQLQueryResult : public QueryResult
{
public:
	MySQLQueryResult(MYSQL_RES* res, uint32 FieldCount, uint32 RowCount);
	~MySQLQueryResult();

	bool NextRow();

protected:
	Field *mTotalField;
	uint32 mNowRowIndex;
};

#endif /* SRC_MODULE_GEDATABASE_MYSQLDATABASE_H_ */
