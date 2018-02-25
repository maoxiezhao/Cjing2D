#ifndef _LUA_DATA_H_
#define _LUA_DATA_H_

#include"lua\luaTools.h"
#include<string>

using std::string;

#define SetterMethod(TYPE, FUNC, MEMBER) void FUNC(const TYPE& v){MEMBER = v;}
#define GetterMethod(TYPE, FUNC, MEMBER) TYPE FUNC()const{return MEMBER;}
#define SetterGetterMethod(TYPE, name) \
	SetterMethod(TYPE, Set##name, m##name ) \
	GetterMethod(TYPE, Get##name, m##name)

/**
*	\brief 打开lua格式文件
*/
class LuaData
{
public:
	LuaData() = default;

	virtual bool ImportFromLua(lua_State*l) = 0;
	bool ImportFromFile(const string& filename);

	std::string GetFileName()const;
	std::string GetFilePath()const;

private:
	std::string mFileName;
	std::string mFilePath;
};

#endif