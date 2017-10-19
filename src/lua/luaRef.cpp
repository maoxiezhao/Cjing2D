#include "luaRef.h"
#include "luaTools.h"

LuaRef::LuaRef():
	l(nullptr),
	mRef(LUA_REFNIL)
{
}

LuaRef::LuaRef(lua_State * l, int ref):
	l(l),
	mRef(ref)
{
}

LuaRef::LuaRef(const LuaRef& other):
	l(nullptr),
	mRef(LUA_REFNIL)
{
	*this = other;
}

LuaRef::LuaRef(LuaRef && other):
	l(other.l),
	mRef(other.mRef)
{
	other.l = nullptr;
	other.mRef = LUA_REFNIL;
}

/**
*	\brief 赋值构造函数
*
*	因为luaref索引必须保持唯一性，这里将other应用的Lua值重新压栈，调用lua_ref创建新的索引
*/
LuaRef & LuaRef::operator=(const LuaRef& other)
{
	Clear();
	l = other.l;
	if (l != nullptr)
	{
		if (other.mRef == LUA_REFNIL || other.mRef == LUA_NOREF)
			mRef = other.mRef;
		else
		{
			lua_rawgeti(l, LUA_REGISTRYINDEX, other.mRef);
			mRef = luaL_ref(l, LUA_REGISTRYINDEX);
		}
	}
	return *this;
}

LuaRef & LuaRef::operator=(LuaRef & other)
{
	Clear();
	l = other.l;
	mRef = other.mRef;
	other.l = nullptr;
	other.mRef = LUA_REFNIL;
	return *this;
}

LuaRef::~LuaRef()
{
	Clear();
}


bool LuaRef::IsEmpty() const
{
	return l== nullptr || (mRef == LUA_REFNIL || mRef == LUA_NOREF);
}

int LuaRef::GetRef() const
{
	return mRef;
}

lua_State * LuaRef::GetLuaState() const
{
	return l;
}

/**
*	\brief 删除ref引用
*
*	调用luaL_unref()
*/
void LuaRef::Clear()
{
	if (l != nullptr)
		luaL_unref(l, LUA_REGISTRYINDEX, mRef);
	l = nullptr;
	mRef = LUA_REFNIL;
}

/**
*	\brief 将引用的lua object压栈
*/
void LuaRef::Push()const
{
	if (IsEmpty())
		return;
	lua_rawgeti(l, LUA_REGISTRYINDEX, mRef);
}

/**
*	\brief 调用引用的Lua object
*/
void LuaRef::Call(const std::string & functionName)const
{
	if (IsEmpty())
		return;
	Push();
	LuaTools::CallFunction(l, 0, 0, functionName);
}
