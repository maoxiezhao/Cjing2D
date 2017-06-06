#include "luaObject.h"

LuaObject::LuaObject():
	mKnowToLua(false),
	mLuaContext(nullptr)
{
}

LuaObject::~LuaObject()
{
	if (mLuaContext != nullptr)
	{

	}
}

LuaContext* LuaObject::GetLuaContext() const
{
	return mLuaContext;
}

void LuaObject::SetLuaContext(LuaContext * luaContext)
{
	mLuaContext = luaContext;
}

bool LuaObject::IsKnowToLua() const
{
	return mKnowToLua;
}

void LuaObject::SetKnowToLua(bool knowToLua)
{
	mKnowToLua = knowToLua;
}
