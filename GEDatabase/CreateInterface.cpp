/*
 * CreateInterface.cpp
 *
 *  Created on: 2018年8月3日
 *      Author: carlos
 */


#include "MySQLDatabase.h"
#include <glog/logging.h>

void Database::CleanupLibs()
{
	mysql_library_end();
}


Database * Database::CreateDatabaseInterface(uint32 uType)
{
	switch(uType)
	{
//#if defined(ENABLE_DATABASE_MYSQL)
	case 1:		// MYSQL
		return new MySQLDatabase();
		break;
//#endif

#if defined(ENABLE_DATABASE_POSTGRES)
	case 2:		// POSTGRES
		return new PostgresDatabase();
		break;
#endif

#if defined(ENABLE_DATABASE_SQLITE)
	case 3:		// SQLITE
		return new SQLiteDatabase();
		break;
#endif
	}

	LOG(ERROR) << "You have attempted to connect to a database that is unsupported or nonexistant.\nCheck your config and try again." << endl;
	abort();
	return NULL;
}

