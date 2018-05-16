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
	if (mLuaContext != nullptr && mLuaContext->GetLuaState() != nullptr)
	{
		mLuaContext->NotifyUserdataDestoryed(*this);
	}
}

LuaContext* LuaObject::GetLuaContext()
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

int LuaObject::LuaWraper(lua_State * l)
{
	if (mLuaContext != nullptr)
	{
		mLuaContext->PushUserdata(l, *this);
		return 1;
	}
	return 0;
}
