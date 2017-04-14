#ifndef _LUA_DATA_H_
#define _LUA_DATA_H_

#include"lua\luaTools.h"
#include<string>

using std::string;
/**
*	\brief 打开lua格式文件
*/
class LuaData
{
public:
	LuaData() = default;
	virtual bool ImportFromLua(lua_State*l) = 0;
	bool ImportFromFile(const string& filename);
};

#endif