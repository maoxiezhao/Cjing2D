#include"luaContext.h"
#include"game\app.h"

const string LuaContext::module_main_name = "Main";

void LuaContext::RegisterMainModule()
{
	static const luaL_Reg functions[] = {
		{"sayHello", main_api_hello},
		{"Exit", main_api_exit}, 
		{"loadFile", main_api_load_file },
		{nullptr,nullptr }
	};

	RegisterFunction(l, module_main_name, functions);

	LuaContext::PushRef(l, LuaContext::mSystemModulesRef);
	lua_getfield(l, -1, "Main");
	lua_setfield(l, LUA_REGISTRYINDEX, module_main_name.c_str());
							// cjing
	lua_pop(l, 1);
}

/**
*	\brief 执行lua脚本main:on_start
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

/**
*	\brief 执行main的绘制操作OnDraw
*/
void LuaContext::OnMainDraw()
{
	PushMain(l);
	OnDraw();
	OnMenuDraw(-1);
	lua_pop(l, 1);
}

/**
*	\brief 输入检测
*
*	输入事件需要传递给当前的menu
*	\return 返回当前是否是有效输入
*/
bool LuaContext::OnMainInput(const InputEvent& event)
{
	PushMain(l);
	bool handle = OnInput(event);
	if (!handle)
		handle = OnMenuInput(-1, event);
	lua_pop(l, 1);
	return handle;
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
		
		GetLuaContext(l).GetApp().SetExiting(true);
		return 0;
	});
}

void LuaContext::PushMain(lua_State*l)
{
	lua_getglobal(l, module_main_name.c_str());
//	lua_getfield(l, LUA_REGISTRYINDEX, module_main_name.c_str());
}

/**
*	\brief 加载lua代码段并压栈,实现cjing.main.loadFile(fileName)
*/
int LuaContext::main_api_load_file(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		LuaContext& luaContext = GetLuaContext(l);
		const string& fileName = LuaTools::CheckString(l, 1);

		if (!LoadFile(l, fileName))
		{
			Debug::Warning( "Failed to load lua file " + fileName);
			lua_pushnil(l);
		}
		return 1;
	});
}