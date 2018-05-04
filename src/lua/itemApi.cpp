#include"lua\luaContext.h"
#include"lua\luaBinder.h"
#include"entity\player.h"
#include"game\item.h"
#include"game\weapon.h"
#include"game\itemAcquired.h"
#include"game\equipment.h"

const string LuaContext::module_item_name = "Item";
const string LuaContext::module_weapon_name = "Weapon";

void LuaContext::RegisterItem()
{
	// item class
	LuaBindClass<Item> itemClass(l, module_item_name);
	itemClass.AddDefaultMetaFunction();
	itemClass.AddMethod("GetGame", item_api_get_game);
	itemClass.AddMethod("SetShadow", item_api_set_shadow);
	itemClass.AddMethod("SetFlow", item_api_set_flow);
	itemClass.AddMethod("SetAutoPicked", item_api_set_auto_picked);

	// weapon class
	LuaBindClass<Weapon> weaponClass(l, module_weapon_name, module_item_name);
	weaponClass.AddDefaultMetaFunction();
	weaponClass.AddFunction("AddEquip", weapon_api_add);
	weaponClass.AddFunction("Equip", weapon_api_equip);
	weaponClass.AddFunction("UnEquip", weapon_api_unequip);
}

/**
*	\brief 将item压栈
*/
void LuaContext::PushItem(lua_State*l, Item& item)
{
	PushUserdata(l, item);
}

/**
*	\brief 检查栈上index索引处是否为item userdata
*	\return ItemPtr
*/
std::shared_ptr<Item> LuaContext::CheckItem(lua_State*l, int index)
{
	return std::dynamic_pointer_cast<Item>(CheckUserdata(l, index, module_item_name));
}

/**
*	\brief 返回栈上index索引处是否item
*	\return true 如果是item
*/
bool LuaContext::IsItem(lua_State*l, int index)
{
	return IsUserdata(l, index, module_item_name);
}

/**
*	\brief 启动item的lua脚本
*
*	当item存在lua脚本且加载到游戏中时调用
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
*	\brief 调用item::onCreated方法
*
*	该方法会在item被创建时调用
*/
void LuaContext::OnItemCreated(Item & item)
{
	PushItem(l, item);
	OnCreated();
	lua_pop(l, 1);
}

/**
*	\brief 调用item:onObtained(itemAcquired)
*
*	当item获取物品时调用，同时传入itemAcquired到函数中
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
*	\brief 实现cjing.Item:getGame()
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
*	\brief 实现cjing.Item:setShadow(true/false)
*
*	是否开启道具的影子效果
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
*	\brief 实现cjing.Item:setFlow(true/false)
*
*	是否开启道具的浮动效果
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
*	\brief 实现cjing.Item:setAutoPicked(true/false)
*
*	设置道具是否自动拾取，是否自动拾取关系到道具与entiyt间
*	的交互
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

/**
*	\brief 实现Weapon.AddEquip(player, equipID)
*	\return bool 是否添加成功
*/
int LuaContext::weapon_api_add(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		auto& player = *CheckPlayer(l, 1);
		const std::string& weaponId = LuaTools::CheckString(l, 2);
		bool equiped = LuaTools::OptBoolean(l, 3, false);
		int slot = LuaTools::CheckIntByDefault(l, 4, -1);

		auto& equipment = player.GetEquipment();
		equipment.AddWeaponToSlot(weaponId, equiped, slot);
		return 0;
	});
}

/**
*	\brief 实现Weapon.Equip(player, equipID)
*	\return bool 是否添加成功
*/
int LuaContext::weapon_api_equip(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Item& item = *CheckItem(l, 1);
		bool autoPicked = LuaTools::CheckBoolean(l, 2);
		item.SetAutoPicked(autoPicked);

		return 0;
	});
}

/**
*	\brief 实现Weapon.UnEquip(player, equipID)
*	\return bool 是否添加成功
*/
int LuaContext::weapon_api_unequip(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Item& item = *CheckItem(l, 1);
		bool autoPicked = LuaTools::CheckBoolean(l, 2);
		item.SetAutoPicked(autoPicked);

		return 0;
	});
}

