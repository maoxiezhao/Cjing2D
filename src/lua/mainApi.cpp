#include"luaContext.h"
#include"game\app.h"

const string LuaContext::module_main_name = "Main";

void LuaContext::RegisterMainModule()
{
	static const luaL_Reg functions[] = {
		{"sayHello",main_api_hello},
		{"Exit",main_api_exit},
		{nullptr,nullptr }
	};

	RegisterFunction(module_main_name, functions);

	lua_getglobal(l, module_name.c_str());
							// cjing
	lua_getfield(l, -1, "Main");
	lua_setfield(l, LUA_REGISTRYINDEX, module_main_name.c_str());
							// cjing
	lua_pop(l, 1);
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

void LuaContext::OnMainUpdate()
{
	PushMain(l);
	OnUpdate();
	lua_pop(l, 1);
}

void LuaContext::OnMainFinish()
{
	PushMain(l);
	OnFinish();
	lua_pop(l, 1);
}

int LuaContext::main_api_hello(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		
		cout << "hello world." << endl;
		return 0;
	});
}

int LuaContext::main_api_exit(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		
		GetLuaContext(l).GetApp()->SetExiting(true);
		return 0;
	});
}

void LuaContext::PushMain(lua_State*l)
{
	lua_getfield(l, LUA_REGISTRYINDEX, module_main_name.c_str());
	//lua_getglobal(l, module_main_name.c_str());
}