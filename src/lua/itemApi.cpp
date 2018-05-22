#include"lua\luaContext.h"
#include"lua\luaBinder.h"
#include"entity\player.h"
#include"game\item.h"
#include"game\weapon.h"
#include"game\itemAcquired.h"
#include"game\equipment.h"
#include"game\savegame.h"
#include"game\game.h"

const string LuaContext::module_item_name = "Item";
const string LuaContext::module_weapon_name = "Weapon";

void LuaContext::RegisterItem()
{
	// item class
	LuaBindClass<Item> itemClass(l, module_item_name);
	itemClass.AddDefaultMetaFunction();
	itemClass.AddFunction("GetItem", item_api_get_item);
	itemClass.AddFunction("AddItem", item_api_add_count);
	itemClass.AddFunction("UseItem", item_api_use_item);

	itemClass.AddMethod("GetGame", item_api_get_game);
	itemClass.AddMethod("SetShadow", item_api_set_shadow);
	itemClass.AddMethod("SetFlow", item_api_set_flow);
	itemClass.AddMethod("SetAutoPicked", item_api_set_auto_picked);
	itemClass.AddMethod("GetCount", &Item::GetItemCount);
	itemClass.AddMethod("GetName", &Item::GetItemName);

	// weapon class
	LuaBindClass<Weapon> weaponClass(l, module_weapon_name, module_item_name);
	weaponClass.AddDefaultMetaFunction();
	weaponClass.AddFunction("GetEquip", item_api_get_item);
	weaponClass.AddFunction("AddEquip", weapon_api_add);
	weaponClass.AddFunction("DropEquip", weapon_api_drop);
	weaponClass.AddFunction("Equip", weapon_api_equip);
	weaponClass.AddFunction("UnEquip", weapon_api_unequip);
	weaponClass.AddFunction("GetCurSlot", weapon_api_get_slot);
	weaponClass.AddFunction("GetCurWeapon", weapon_api_get_cur_equip);
	weaponClass.AddFunction("HasWeapon", weapon_api_has_weapon);

	weaponClass.AddMethod("SetAnimation", &Weapon::SetAnimation);
	weaponClass.AddMethod("SetAttackDelta", &Weapon::SetAttackDelta);
	weaponClass.AddMethod("GetEntity", weapon_api_get_entity);
	weaponClass.AddMethod("GetSprite", weapon_api_get_sprite);
	weaponClass.AddMethod("SetPosOffset", &Weapon::SetSpritePosOffset);
	weaponClass.AddMethod("SetRotateOffset", &Weapon::SetSpriteRotateOffset);
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
	const std::string itemFile = std::string("items/items/") + item.GetItemName();
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
bool LuaContext::OnItemObtained(Item & item, ItemAcquired & itemAcquired, Entity& picker)
{									
	if (!IsUserdataHasField(item, "OnObtained"))
	{
		return false;
	}
	PushItem(l, item);
	if (FindMethod("OnObtained"))
	{
		PushUserdata(l, picker);
		lua_pushstring(l, item.GetItemName().c_str());
		lua_pushinteger(l, itemAcquired.GetVariant());
		LuaTools::CallFunction(l, 4, 1, "OnObtained");
	}
	bool result = LuaTools::OptBoolean(l, -1, true);
	lua_pop(l, 2);
	return result;
}

bool LuaContext::OnWeaponObtained(Weapon & weapon, ItemAcquired & itemAccquired, Entity& picker)
{
	if (!IsUserdataHasField(weapon, "OnWeaponObtained"))
	{
		return false;
	}
	PushUserdata(l, weapon);
	if (FindMethod("OnWeaponObtained"))
	{
		PushUserdata(l, picker);
		LuaTools::CallFunction(l, 2, 1, "OnWeaponObtained");
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
*	\brief Item.GetItem(cur_game, id)
*/
int LuaContext::item_api_get_item(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Savegame& savegame = *CheckSavegame(l, 1);	
		Equipment& equip = savegame.GetEquipment();
		const std::string& key = LuaTools::CheckString(l, 2);
		auto& item = equip.GetItem(key);

		PushItem(l, item);
		return 1;
	});
}

/**
*	\brief Item.AddItem(cur_game, id, count)
*/
int LuaContext::item_api_add_count(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Savegame& savegame = *CheckSavegame(l, 1);
		auto game = savegame.GetGame();
		if (game != nullptr)
		{
			Equipment& equip = game->GetEquipment();
			const std::string& key = LuaTools::CheckString(l, 2);
			int count = LuaTools::CheckInt(l, 3);
			auto& item = equip.GetItem(key);
			bool result = item.AddItem(count);

			lua_pushboolean(l, result);
			return 1;
		}
		return 0;
	});
}

/**
*	\brief Item.UseItem(cur_game, id, count, useEntity)
*/
int LuaContext::item_api_use_item(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Savegame& savegame = *CheckSavegame(l, 1);
		auto game = savegame.GetGame();
		if (game != nullptr)
		{
			Equipment& equip = game->GetEquipment();
			const std::string& key = LuaTools::CheckString(l, 2);
			int count = LuaTools::CheckInt(l, 3);
			auto& item = equip.GetItem(key);
			Entity* useEntity = nullptr;
			if (lua_isnil(l, 4))
				useEntity = CheckEntity(l, 4).get();

			bool result = item.UseItem(count, useEntity);
			lua_pushboolean(l, result);
			return 1;
		}
		return 0;
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

/**
*	\brief ʵ��Weapon.AddEquip(player, equipID)
*	\return bool �Ƿ���ӳɹ�
*/
int LuaContext::weapon_api_add(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		auto& player = *CheckPlayer(l, 1);
		const std::string& weaponId = LuaTools::CheckString(l, 2);
		bool equiped = LuaTools::OptBoolean(l, 3, false);
		int slot = LuaTools::CheckIntByDefault(l, 4, -1);

		auto& equipment = player.GetEquipment();
		bool result = equipment.AddWeaponToSlot(weaponId, equiped, slot);
		lua_pushboolean(l, result);
		return 1;
	});
}

/**
*	\brief ʵ��Weapon.DropEquip(player, equipID)
*	\return bool �Ƿ�ɾ���ɹ�
*/
int LuaContext::weapon_api_drop(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		auto& player = *CheckPlayer(l, 1);
		auto& equipment = player.GetEquipment();


		bool result = false;
		if (lua_isinteger(l, 2))
		{
			int slot = LuaTools::CheckInt(l, 2);
			result = equipment.RemoveWeaponBySlot(slot);
		}
		else if (lua_isstring(l, 2))
		{
			const std::string& weaponID = LuaTools::CheckString(l, 2);
			result = equipment.RemoveWeaponByID(weaponID);
		}

		lua_pushboolean(l, result);
		return 1;
	});
}

/**
*	\brief ʵ��Weapon.Equip(player, equipID)
*	\return bool �Ƿ���ӳɹ�
*/
int LuaContext::weapon_api_equip(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		auto& player = *CheckPlayer(l, 1);
		auto& equipment = player.GetEquipment();

		bool result = false;
		if (lua_isinteger(l, 2))
		{
			int slot = LuaTools::CheckInt(l, 2);
			result = equipment.EquipWeaponFromSlots(slot);
		}
		else if (lua_isstring(l, 2))
		{
			const std::string& weaponID = LuaTools::CheckString(l, 2);
			result = equipment.EquipWeaponFromSlots(weaponID);
		}
		lua_pushboolean(l, result);
		return 1;
	});
}

/**
*	\brief ʵ��Weapon.UnEquip(player, equipID)
*	\return bool �Ƿ���ӳɹ�
*/
int LuaContext::weapon_api_unequip(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		auto& player = *CheckPlayer(l, 1);
		auto& equipment = player.GetEquipment();

		bool result = false;
		if (lua_isinteger(l, 2))
		{
			int slot = LuaTools::CheckInt(l, 2);
			result = equipment.UnEquipWeaponFromSlots(slot);
		}
		else if (lua_isstring(l, 2))
		{
			const std::string& weaponID = LuaTools::CheckString(l, 2);
			result = equipment.UnEquipWeaponFromSlots(weaponID);
		}
		lua_pushboolean(l, result);
		return 1;
	});
}


/**
*	\brief ʵ��Weapon.GetCurSlot(player)
*	\return int ��ǰװ����λ
*/
int LuaContext::weapon_api_get_slot(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		auto& player = *CheckPlayer(l, 1);
		auto& equipment = player.GetEquipment();
		int slot = equipment.GetCurEquipSlot();
	
		lua_pushinteger(l, slot);
		return 1;
	});
}

/**
*	\brief �ǰ�weapon:GetEntity()
*	\return ���ص�ǰ�����ĳ���Entity,���Ϊ�շ���nil
*/
int LuaContext::weapon_api_get_entity(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Weapon& weapon = *std::static_pointer_cast<Weapon>(
			CheckUserdata(l, 1, module_weapon_name));
		
		Entity* entity = weapon.GetEntity();
		if (entity != nullptr)
		{
			GetLuaContext(l).PushUserdata(l, *entity);
			return 1;
		}
		return 0;
	});
}

/**
*	\brief �ǰ�weapon:GetSprite()
*/
int LuaContext::weapon_api_get_sprite(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		Weapon& weapon = *std::static_pointer_cast<Weapon>(
			CheckUserdata(l, 1, module_weapon_name));

		auto sprite = weapon.GetWeaponSprite();
		if (sprite != nullptr)
		{
			PushUserdata(l, *sprite);
			return 1;
		}
		return 0;
	});
}

/**
*	\brief �ǰ�weapon:GetEntity()
*	\return ���ص�ǰ�����ĳ���Entity,���Ϊ�շ���nil
*/
int LuaContext::weapon_api_get_cur_equip(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		auto& player = *CheckPlayer(l, 1);
		auto& equipment = player.GetEquipment();
		
		auto curEquip = equipment.GetCurWeapon();
		if (curEquip != nullptr)
		{
			PushUserdata(l, *curEquip);
			return 1;
		}
		
		return 0;
	});
}

/**
*	\brief �ǰ�weapon:HasWeapon(slot)
*	\return ���ص�ǰ�����ĳ���Entity,���Ϊ�շ���nil
*/
int LuaContext::weapon_api_has_weapon(lua_State*l)
{
	return LuaTools::ExceptionBoundary(l, [&] {
		auto& player = *CheckPlayer(l, 1);
		auto& equipment = player.GetEquipment();
		int slot = LuaTools::CheckInt(l, 2);
		bool result = equipment.GetWeaponFromSlot(slot) != nullptr;
	
		lua_pushboolean(l, result);
		return 1;
	});
};
