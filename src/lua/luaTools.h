#ifndef _LUATOOLS_H_
#define _LUATOOLS_H_

#include"common\common.h"
#include<lua.hpp>

/**
*	\brief lua¹¤¾ßº¯Êý
*/
namespace LuaTools
{
	bool CallFunction(lua_State*l,int arguments,int results,const string& functionName);
	int  GetPositiveIndex(lua_State*l, int index);
}

#endif