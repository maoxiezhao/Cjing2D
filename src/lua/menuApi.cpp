#include"luaContext.h"
#include"luaTools.h"

const string LuaContext::module_menu_name = "Menu";

void LuaContext::RegisterMenuModule()
{
	static const luaL_Reg functions[] = {
		{"Start",menu_api_start},
		{"Stop",menu_api_stop},
		{ nullptr,nullptr }
	};

	RegisterFunction(module_menu_name, functions);
}

/**
*	\brief 添加菜单
*/
void LuaContext::AddMenu(int contextIndex, const LuaRef& menuRef)
{
	const void*context = lua_topointer(l, 1);// 这里暂时假设menu 必定为table
	mMenus.emplace_back(menuRef, context);
	// 执行onStarted
	OnMenuStart(menuRef);
}

/**
*	\brief 更新菜单项，删除为空的菜单
*
*	onUpdate不由次函数调用,且此删除是依据menuRef是否为空
*	所以要删除的menu应在其他函数中执行menuRef.claer()
*/
void LuaContext::UpdateMenus()
{
}

void LuaContext::DestoryMenus()
{
	mMenus.clear();
}

/**
*	\brief 创建菜单menu
*/
int LuaContext::menu_api_start(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		LuaContext& luaContext = GetLuaContext(l);
		// 参数1必须是context table
		if (lua_type(l, 1) != LUA_TUSERDATA && lua_type(l, 1) != LUA_TTABLE)
			LuaTools::Error(l, "wrong type.Only userdata or table");

		// 参数2必须是table
		LuaTools::CheckType(l, 2, LUA_TTABLE);
		LuaRef menuRef = luaContext.CreateRef();
		luaContext.AddMenu(1, menuRef);

		return 0;
	});
}

/**
*	\brief 关闭菜单
*
*	lua: Menu.Stop(self);
*/
int LuaContext::menu_api_stop(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
	
		return 0;
	});
}

void LuaContext::OnMenuStart(const LuaRef&menuRef)
{
	PushRef(l, menuRef);
	OnStart();
	lua_pop(l, 1);
}

/**
*	\brief 更新menu
*
*	在更新完本体后，需要更新子窗体（可能为nil)
*/
void LuaContext::OnMenuUpdate(const LuaRef&menuRef)
{
	PushRef(l, menuRef);
	OnUpdate();
	// OnMenusUpdate(menuRef);
	lua_pop(l, 1);
}

/**
*	\brief 执行menu的onFinished函数
*
*	该函数为menuz终结时调用，需要同时销毁子窗体
*/
void LuaContext::OnMenuFinish(const LuaRef&menuRef)
{
	PushRef(l, menuRef);
	// removeMenus(-1);
	OnFinish();
	lua_pop(l, 1);
}