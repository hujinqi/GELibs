/*
 * LuaMgr.cpp
 *
 *  Created on: Dec 27, 2018
 *      Author: hjq
 */

#include "LuaMgr.h"
#include "GETime/TimeMgr.h"

initialiseSingleton(LuaMgr);

bool LuaMgr::LoadFile(const char* filePath, RegisterAPI fun)
{
	m_filePath = filePath;
	m_registerAPI_fun = fun;

	m_lua = _loadFile();
	return (m_lua != NULL);
}

bool LuaMgr::ReloadFile()
{
	LuaContext* lua = _loadFile();
	if(lua){
		m_lua = lua;
		return true;
	}
	return false;
}

LuaContext* LuaMgr::_loadFile()
{
	LuaContext* lua = new LuaContext();

	std::ifstream file(m_filePath);
	if(!file){
		LOG(ERROR)<<"加载Lua脚本失败！ 文件不存在. file:"<<m_filePath<<endl;
		delete lua;
		return NULL;
	}

	//注册API
	if(m_registerAPI_fun){
		m_registerAPI_fun(lua);
	}

	bool succ = true;
	try{
		lua->executeCode(file);
	}catch(const std::runtime_error& e){
		succ = false;
		LOG(ERROR) << "加载Lua脚本失败！ "<< e.what() << std::endl;           // prints an error message
	}

	if(!succ){
		delete lua;
		return NULL;
	}
	return lua;
}
