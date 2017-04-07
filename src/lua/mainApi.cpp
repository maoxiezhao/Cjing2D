#include"luaContext.h"

const string LuaContext::module_main_name = "Main";

void LuaContext::RegisterMainModule()
{
	static const luaL_Reg functions[] = {
		{"sayHello",main_api_hello},
		{nullptr,nullptr }
	};

	RegisterFunction(module_main_name, functions);
}

/**
*	\brief Ö´ÐÐlua½Å±¾main:on_start
*/
void LuaContext::OnMainStart()
{
	PushMain(l);
	OnStart();
	lua_pop(l, 1);
}

int LuaContext::main_api_hello(lua_State* l)
{
	cout << "hello world." << endl;
	return 0;
}

void LuaContext::PushMain(lua_State*l)
{
	//lua_getfield(l, LUA_REGISTRYINDEX, module_main_name.c_str());
	lua_getglobal(l, module_main_name.c_str());
}