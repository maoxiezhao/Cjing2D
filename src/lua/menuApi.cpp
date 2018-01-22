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
	RegisterFunction(l, module_menu_name, functions);
}

/**
*	\brief ��Ӳ˵�
*/
void LuaContext::AddMenu(int contextIndex, const LuaRef& menuRef)
{
	const void*context = lua_topointer(l, contextIndex);// ������ʱ����menu �ض�Ϊtable
	mMenus.emplace_back(menuRef, context);
	// ִ��onStarted
	OnMenuStart(menuRef);
}

/**
*	\brief ���²˵��ɾ��Ϊ�յĲ˵�
*
*	onUpdate���ɴκ�������,�Ҵ�ɾ��������menuRef�Ƿ�Ϊ��
*	����Ҫɾ����menuӦ������������ִ��menuRef.claer()
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
*	\brief ɾ��ָ����������menus
*/
void LuaContext::RemoveMenus(int contextIndex)
{
	LuaTools::CheckType(l, contextIndex, LUA_TTABLE);
	const void* context = lua_topointer(l, contextIndex);

	// menu��ɾ�������п��ܻ�����µĲ˵�����ǰ��
	// ��ȫ����Ϊfalse���������˵�������ɾ��
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
	// menu��ɾ�������п��ܻ�����µĲ˵�����ǰ��
	// ��ȫ����Ϊfalse���������˵�������ɾ��
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
*	\brief �����˵�menu
*/
int LuaContext::menu_api_start(lua_State* l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		LuaContext& luaContext = GetLuaContext(l);
		
		//luaContext.PrintLuaStack(l);

		// ����1������context table
		if (lua_type(l, 1) != LUA_TUSERDATA && lua_type(l, 1) != LUA_TTABLE)
			LuaTools::Error(l, "wrong type.Only userdata or table");

		// ����2������table
		LuaTools::CheckType(l, 2, LUA_TTABLE);
		const LuaRef& menuRef = luaContext.CreateRef(l);
		luaContext.AddMenu(1, menuRef);

		return 0;
	});
}

/**
*	\brief �رղ˵�
*
*	lua: Menu.Stop(self);menu���ͷŹ�����updateMenusִ��
*	����ֻ�ǽ����ͷŵ�menu��luaRef��Ϊclear
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
				menu.menuRef.Clear();	// ��updateʱ�ᱻ�ͷ�
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
*	\brief ����menu
*
*	�ڸ����걾�����Ҫ�����Ӵ��壨����Ϊnil)
*/
void LuaContext::OnMenuUpdate(const LuaRef&menuRef)
{
	PushRef(l, menuRef);
	OnUpdate();
	// OnMenusUpdate(menuRef);
	lua_pop(l, 1);
}

/**
*	\brief ִ��menu��onFinished����
*
*	�ú���Ϊmenuz�ս�ʱ���ã���Ҫͬʱ�����Ӵ���
*/
void LuaContext::OnMenuFinish(const LuaRef&menuRef)
{
	PushRef(l, menuRef);
	RemoveMenus(-1);
	OnFinish();
	lua_pop(l, 1);
}

/**
*	\brief ִ��menu��contextΪָ��contextIndex��������
*/
bool LuaContext::OnMenuInput(int contextIndex,const InputEvent& event)
{
	const void* context = lua_topointer(l, contextIndex);
	bool handle = false;
	// ��ÿ���˵�context���ڸ�context����OnMenuInput
	std::list<LuaContext::MenuData>::reverse_iterator it;
	for (it = mMenus.rbegin(); it != mMenus.rend() && !handle;++it)
	{
		const LuaRef&menuRef = it->menuRef;
		if (it->l == context)
			handle = OnMenuInput(event, menuRef);
	}
	return handle;
}

/**
*	\brief ִ��menu��������
*/
bool LuaContext::OnMenuInput(const InputEvent & event, const LuaRef & menuRef)
{
	bool handle = false;
	PushRef(l, menuRef);
	handle = OnInput(event);
	lua_pop(l, 1);
	return handle;
}

void LuaContext::OnMenuDraw(int contextIndex)
{
	const void* context = lua_topointer(l, contextIndex);
	std::list<LuaContext::MenuData>::reverse_iterator it;
	for (it = mMenus.rbegin(); it != mMenus.rend(); ++it)
	{
		const LuaRef&menuRef = it->menuRef;
		if (it->l == context)
			OnMenuDraw(it->menuRef);
	}
}

void LuaContext::OnMenuDraw(const LuaRef& menuRef)
{
	PushRef(l, menuRef);
	OnMenuDraw(-1);
	OnDraw();
	lua_pop(l, 1);
}