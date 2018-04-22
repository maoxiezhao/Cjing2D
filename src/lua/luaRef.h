#ifndef _LUA_REF_H_
#define _LUA_REF_H_

#include"common\common.h"
#include<lua.hpp>
#include<string>

/**
*	\brief 引用lua中的函数作为回调函数
*/
class LuaRef
{
public:
	LuaRef();
	LuaRef(lua_State*l, int ref);
	LuaRef(const LuaRef& other);
	LuaRef(LuaRef&& other);
	LuaRef& operator=(const LuaRef&other);
	LuaRef& operator=(LuaRef& other);
	~LuaRef();

	bool IsEmpty()const;
	int  GetRef()const;

	lua_State* GetLuaState()const;
	void Clear();
	void Push()const;
	void Call(const std::string& functionName )const;

	static LuaRef Nil;
private:
	lua_State*l;
	int mRef;

};


#endif