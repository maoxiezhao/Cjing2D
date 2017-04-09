#ifndef _LUA_EXCEPTION_H_
#define _LUA_EXCEPTION_H_

#include<exception>
#include<string>

struct lua_State;

class LuaException : public std::exception
{
public:
	LuaException(lua_State*l, const std::string& errmsg);

	lua_State* GetLuaState()const;
	virtual const char* what()const throw() override;

private:
	std::string mErrMsg;
	lua_State* l;
};


#endif