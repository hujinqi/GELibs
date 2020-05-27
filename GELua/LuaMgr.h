/*
 * LuaMgr.h
 *
 *  Created on: Dec 27, 2018
 *      Author: hjq
 */

#ifndef SRC_MODULE_GELUA_LUAMGR_H_
#define SRC_MODULE_GELUA_LUAMGR_H_

#include "common/defines.h"
#include "common/Singleton.h"
#include "GELua/LuaContext.h"

#define LUA_BEGIN try{
#define LUA_END }catch(const std::runtime_error& e){\
		LOG(ERROR) << "Lua Error: "<< e.what();\
	}

class LuaMgr : public Singleton<LuaMgr>
{
public:
	GE_INLINE LuaContext* GetLuaContext() { return m_lua; }

	typedef void (*RegisterAPI)(LuaContext* lua);
	/**
	 * 加载lua脚本
	 * 	filePath：路径
	 * 	fun: 注册API的接口
	 */
	bool LoadFile(const char* filePath, RegisterAPI fun=NULL);

	/**
	 * 重新加载lua脚本
	 */
	bool ReloadFile();

protected:
	LuaContext* _loadFile();

private:
	LuaContext* m_lua;
	string m_filePath;
	RegisterAPI m_registerAPI_fun;
};

#define sLuaMgr LuaMgr::getSingleton()



#endif /* SRC_MODULE_GELUA_LUAMGR_H_ */
