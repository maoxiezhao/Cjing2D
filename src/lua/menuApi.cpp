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
	const void*context = lua_topointer(l, contextIndex);// 这里暂时假设menu 必定为table
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
	for (auto&menu = mMenus.begin(); menu != mMenus.end();)
	{
		menu->recentlyAdd = false;
		if (menu->menuRef.IsEmpty())
			menu = mMenus.erase(menu);
		else
			++menu;
	}
}

void LuaContext::DestoryMenus()
{
	mMenus.clear();
}

/**
*	\brief 删除指定索引处的menus
*/
void LuaContext::RemoveMenus(int contextIndex)
{
	LuaTools::CheckType(l, contextIndex, LUA_TTABLE);
	const void* context = lua_topointer(l, contextIndex);

	// menu在删除过程中可能会产生新的菜单，当前菜
	// 单全部设为false，则新增菜单不参与删除
	for (auto& menu : mMenus)
		menu.recentlyAdd = false;
	for (auto& menu : mMenus)
	{
		LuaRef menuRef = menu.menuRef;
		if (menu.l == context && !menu.recentlyAdd )
		{
			menu.menuRef.Clear();
			menu.l = nullptr;
			OnMenuFinish(menuRef);
		}
	}
}

void LuaContext::RemoveMenus()
{
	// menu在删除过程中可能会产生新的菜单，当前菜
	// 单全部设为false，则新增菜单不参与删除
	for (auto& menu : mMenus)
		menu.recentlyAdd = false;
	for (auto& menu : mMenus)
	{
		LuaRef menuRef = menu.menuRef;
		if (!menu.recentlyAdd && !menu.menuRef.IsEmpty()) 
		{
			menu.menuRef.Clear();
			menu.l = nullptr;
			OnMenuFinish(menuRef);
		}
	}
}

/**
*	\brief 创建菜单menu
*/
int LuaContext::menu_api_start(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		LuaContext& luaContext = GetLuaContext(l);
		cout << luaL_typename(l, 0) << endl;

		// 参数1必须是context table
		if (lua_type(l, 1) != LUA_TUSERDATA && lua_type(l, 1) != LUA_TTABLE)
			LuaTools::Error(l, "wrong type.Only userdata or table");

		// 参数2必须是table
		LuaTools::CheckType(l, 2, LUA_TTABLE);
		const LuaRef& menuRef = luaContext.CreateRef();
		luaContext.AddMenu(1, menuRef);

		return 0;
	});
}

/**
*	\brief 关闭菜单
*
*	lua: Menu.Stop(self);menu的释放工作由updateMenus执行
*	这里只是将被释放的menu的luaRef设为clear
*/
int LuaContext::menu_api_stop(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		LuaTools::CheckType(l, 1, LUA_TTABLE);

		auto& menus = GetLuaContext(l).mMenus;
		for (auto&menu : menus)
		{
			menu.menuRef.Push();
			if (lua_rawequal(l, 1, -1) == 1)
			{							// table .... menuTable
				LuaRef menuRef = menu.menuRef;
				menu.menuRef.Clear();	// 在update时会被释放
				menu.l = nullptr;
				GetLuaContext(l).OnMenuFinish(menuRef);
				lua_pop(l, 1);
				break;
			}
			lua_pop(l, 1);
		}
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
	RemoveMenus(-1);
	OnFinish();
	lua_pop(l, 1);
}