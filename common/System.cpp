/*
 * System.cpp
 *
 *  Created on: 2018年3月21日
 *      Author: carlos Hu
 */

#include <sys/resource.h>
#include <stdio.h>

bool SetCoreLimit()
{
	rlimit rl;
	if(getrlimit(RLIMIT_CORE, &rl) == -1)
	{
		printf("getrlimit failed. This could be problem.");
		return false;
	}
	else
	{
		rl.rlim_cur = rl.rlim_max;
		if(setrlimit(RLIMIT_CORE, &rl) == -1)
		{
			printf("setrlimit failed. Server may not save core.dump files.\n");
			return false;
		}
		return true;
	}
	return false;
}
