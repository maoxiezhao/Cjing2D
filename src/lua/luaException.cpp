#include "luaException.h"
#include "lua.hpp"

LuaException::LuaException(lua_State * l, const std::string & errmsg):
	l(l),
	mErrMsg(errmsg)
{
}

lua_State * LuaException::GetLuaState() const
{
	return l;
}

const char * LuaException::what() const throw()
{
	return mErrMsg.c_str();
}
