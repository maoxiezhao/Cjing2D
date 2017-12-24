#include"lua\luaContext.h"
#include"lua\luaBinder.h"
#include"gui\widget\window.h"

const string LuaContext::module_window_name = "Window";

/**
*	\brief  µœ÷cjing.Window.create(x, y, w, h)
*/
//int window_api_create(lua_State* l)
//{
//	return LuaTools::ExceptionBoundary(l, [&] {
//		return 0;
//	});
//}

void window_api_create()
{
}

int RegisterFunction(lua_State* l)
{
	LuaBindClass<gui::Window> bindClass(l, "Window");

	bindClass.AddMetaFunction("__gc", LuaContext::userdata_meta_gc);
	bindClass.AddMetaFunction("__index", LuaContext::userdata_meta_index);
	bindClass.AddMetaFunction("__newindex", LuaContext::userdata_meta_newindex);
	bindClass.AddFunction("create", window_api_create);

	return 0;
}

LUA_FUNCTION_AUTO_BINDER(Window, RegisterFunction)
