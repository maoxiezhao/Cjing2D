#include"lua\luaContext.h"
#include"game\item.h"
#include"game\itemAcquired.h"

const string LuaContext::module_item_name = "Item";

void LuaContext::RegisterItem()
{
	static const luaL_Reg methods[] = {
		{ "getGame", item_api_get_game },
		{ "setShadow", item_api_set_shadow },
		{ "setFlow", item_api_set_flow },
		{ "setAutoPicked", item_api_set_auto_picked },
		{ nullptr,nullptr }
	};

	static const luaL_Reg metamethos[] = {
		{ "__newindex", userdata_meta_newindex },
		{ "__index", userdata_meta_index },
		{ "__gc", userdata_meta_gc },
		{ nullptr, nullptr }
	};

	RegisterType(l, module_item_name, nullptr, methods, metamethos);
}

/**
*	\brief ��itemѹջ
*/
void LuaContext::PushItem(lua_State*l, Item& item)
{
	PushUserdata(l, item);
}

/**
*	\brief ���ջ��index�������Ƿ�Ϊitem userdata
*	\return ItemPtr
*/
std::shared_ptr<Item> LuaContext::CheckItem(lua_State*l, int index)
{
	return std::dynamic_pointer_cast<Item>(CheckUserdata(l, index, module_item_name));
}

/**
*	\brief ����ջ��index�������Ƿ�item
*	\return true �����item
*/
bool LuaContext::IsItem(lua_State*l, int index)
{
	return IsUserdata(l, index, module_item_name);
}

/**
*	\brief ����item��lua�ű�
*
*	��item����lua�ű��Ҽ��ص���Ϸ��ʱ����
*/
void LuaContext::RunItem(Item& item)
{
	const std::string itemFile = std::string("items/") + item.GetItemName();
	if (LoadFile(l, itemFile))
	{
		PushItem(l, item);
		LuaTools::CallFunction(l, 1, 0, "itemFunction");

		OnItemCreated(item);
	}
}

/**
*	\brief ����item::onCreated����
*
*	�÷�������item������ʱ����
*/
void LuaContext::OnItemCreated(Item & item)
{
	PushItem(l, item);
	OnCreated();
	lua_pop(l, 1);
}

/**
*	\brief ����item:onObtained(itemAcquired)
*
*	��item��ȡ��Ʒʱ���ã�ͬʱ����itemAcquired��������
*/
bool LuaContext::OnItemObtained(Item & item, ItemAcquired & itemAcquired)
{									
	if (!IsUserdataHasField(item, "onObtained"))
	{
		return false;
	}
	PushItem(l, item);
	if (FindMethod("onObtained"))
	{
		lua_pushstring(l, item.GetItemName().c_str());
		lua_pushinteger(l, itemAcquired.GetVariant());
		LuaTools::CallFunction(l, 3, 1, "onObtained");
	}
	bool result = LuaTools::OptBoolean(l, -1, true);
	lua_pop(l, 2);
	return result;
}

/**
*	\brief ʵ��cjing.Item:getGame()
*/
int LuaContext::item_api_get_game(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Item& item = *CheckItem(l, 1);
		Savegame& game = item.GetSavegame();
		PushGame(l, game);

		return 1;
	});
}


/**
*	\brief ʵ��cjing.Item:setShadow(true/false)
*
*	�Ƿ������ߵ�Ӱ��Ч��
*/
int LuaContext::item_api_set_shadow(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Item& item = *CheckItem(l, 1);
		bool shadowed = LuaTools::CheckBoolean(l, 2);
		item.SetHasShadow(shadowed);

		return 0;
	});
}

/**
*	\brief ʵ��cjing.Item:setFlow(true/false)
*
*	�Ƿ������ߵĸ���Ч��
*/
int LuaContext::item_api_set_flow(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Item& item = *CheckItem(l, 1);
		bool flowed = LuaTools::CheckBoolean(l, 2);
		item.SetHasFlowEffect(flowed);

		return 0;
	});
}

/**
*	\brief ʵ��cjing.Item:setAutoPicked(true/false)
*
*	���õ����Ƿ��Զ�ʰȡ���Ƿ��Զ�ʰȡ��ϵ��������entiyt��
*	�Ľ���
*/
int LuaContext::item_api_set_auto_picked(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Item& item = *CheckItem(l, 1);
		bool autoPicked = LuaTools::CheckBoolean(l, 2);
		item.SetAutoPicked(autoPicked);

		return 0;
	});
}

