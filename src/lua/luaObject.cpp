#include "luaObject.h"
#include "luaContext.h"

LuaObject::LuaObject():
	mKnowToLua(false),
	mWithLuaTable(false),
	mLuaContext(nullptr)
{
}

LuaObject::~LuaObject()
{
	if (mLuaContext != nullptr)
	{
		mLuaContext->NotifyUserdataDestoryed(*this);
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

bool LuaObject::IsWithLuaTable() const
{
	return mWithLuaTable;
}

void LuaObject::SetWithLuaTable(bool withLuaTable)
{
	mWithLuaTable = withLuaTable;
}
