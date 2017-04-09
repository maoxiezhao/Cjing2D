#ifndef _LUATOOLS_H_
#define _LUATOOLS_H_

#include"common\common.h"
#include"lua\luaException.h"
#include"lua\luaRef.h"
#include<lua.hpp>

/**
*	\brief lua¹¤¾ßº¯Êý
*/
namespace LuaTools
{
	LuaRef CreateRef(lua_State*l, int index);
	bool CallFunction(lua_State*l,int arguments,int results,const string& functionName);
	int  GetPositiveIndex(lua_State*l, int index);
	void CheckType(lua_State*l, int index, int exceptedType);
	int  CheckInt(lua_State*l, int index);
	string CheckString(lua_State*l,int index);
	LuaRef CheckFunction(lua_State*l, int index);
	void Error(lua_State*l, const string& message);
	void ArgError(lua_State*l, int index, const string&message);

	template<typename Callable>
	int  ExceptionBoundary(lua_State*l, Callable&& func)
	{
		try
		{
			return func();
		}
		catch (const LuaException& ex)
		{
			
			luaL_error(l, ex.what());
		}
		catch (const std::exception&ex)
		{
			luaL_error(l, (string("Error:") + ex.what()).c_str());
		}

		return 0;
	}
}

#endif