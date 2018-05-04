#include "game\equipment.h"
#include "game\savegame.h"
#include "game\gameResources.h"
#include "game\item.h"
#include "entity\player.h"

Equipment::Equipment(Savegame & savegame):
	mSavegame(savegame),
	mItemBegs(*this),
	mCurEquipSlot(0),
	mMaxEquipSlot(3)
{
	mEquipdWeapons.resize(mMaxEquipSlot, nullptr);
}

void Equipment::Update()
{
	if (HasCurWeapon())
	{
		auto& weapon = GetCurWeapon();
		if (weapon->IsEquiped())
			weapon->Update();
	}
}

/**
*	\brief 加载所有item
*/
void Equipment::LoadAllItems()
{
	// 创建所有item对象
	const auto& itemsInfo = GameResource::GetGameResource().GetGameResourcesInfos(GameResourceType::ITEM);
	for (const auto& itemId : itemsInfo)
	{
		std::shared_ptr<Item> item = std::make_shared<Item>(itemId, *this);
		mAllItems[itemId] = item;
	}
	// 创建所有weapon对象,暂时如同物品一般对待
	const auto& weaponInfos = GameResource::GetGameResource().GetGameResourcesInfos(GameResourceType::WEAPON);
	for (const auto& weaponID : weaponInfos)
	{
		std::shared_ptr<Weapon> item = std::make_shared<Weapon>(weaponID, *this);
		mAllItems[weaponID] = item;
	}

	// 初始化所有脚本
	for (auto& kvp : mAllItems)
	{
		Item& item = *kvp.second;
		item.Initialize();
	}
}

/**
*	\brief 获取制定名字的item
*/
Item & Equipment::GetItem(const std::string & itemName)
{
	auto it = mAllItems.find(itemName);
	if (it == mAllItems.end() || it->second->IsWeapon())
		Debug::Error("Get the non-exists item:" + itemName);

	return *it->second;
}

/**
*	\brief 获取制定名字的item
*/
const Item & Equipment::GetItem(const std::string & itemName) const
{
	auto itor = mAllItems.find(itemName);
	if (itor == mAllItems.end())
	{
		Debug::Error("Get the non-exists item:" + itemName);
	}
	return *itor->second;
}

bool Equipment::PushItemIntoBeg(ItemAcquired & item)
{
	return true;
}

/**
*	\brief 获取当前生命值
*/
int Equipment::GetLife() const
{
	return mSavegame.GetInteger(Savegame::KEYWORD_CURRENT_LIFE);
}

/**
*	\brief 设置当前生命值
*/
void Equipment::SetLife(int life)
{
	mSavegame.SetInteger(Savegame::KEYWORD_CURRENT_LIFE, life);
}

/**
*	\brief 获取当前最大生命值
*/
int Equipment::GetMaxLife() const
{
	return mSavegame.GetInteger(Savegame::KEYWORD_CURRENT_MAX_LIFE);
}

/**
*	\brief 设置当前最大生命值
*/
void Equipment::SetMaxLife(int maxLife)
{
	mSavegame.SetInteger(Savegame::KEYWORD_CURRENT_MAX_LIFE, maxLife);
}

Savegame & Equipment::GetSavegame()
{
	return mSavegame;
}

void Equipment::LoadAllWeapon()
{
	// 创建所有item对象
	const auto& itemsInfo = GameResource::GetGameResource().GetGameResourcesInfos(GameResourceType::ITEM);
	for (const auto& itemId : itemsInfo)
	{
		std::shared_ptr<Item> item = std::make_shared<Item>(itemId, *this);
		mAllItems[itemId] = item;
	}
	// 初始化所有脚本
	for (auto& kvp : mAllItems)
	{
		Item& item = *kvp.second;
		item.Initialize();
	}
}

/**
*	\brief 判断是否存在该武器ID
*/
bool Equipment::HasWeapon(const std::string & weaponID) const
{
	auto it = mAllItems.find(weaponID);
	return (it != mAllItems.end() && it->second->IsWeapon());
}

/**
*	\brief 获取对应WeaponID对应的weapon实例
*/
Weapon & Equipment::GetWeapon(const std::string & weaponID)
{
	auto it = mAllItems.find(weaponID);
	if (it == mAllItems.end() || !it->second->IsWeapon())
		Debug::Error("Get the non-exists weapon:" + weaponID);
	
	return dynamic_cast<Weapon&>(*it->second);
}

/**
*	\brief 向武器槽中添加装备
*	\param weaponID 装备的ID
*	\param equiped 是否直接装备
*	\param slot 装备的槽(0-2),如果是-1则从前往后找到第一个有效槽
*/
bool Equipment::AddWeaponToSlot(const std::string & weaponID, bool equiped, int slot)
{
	// 如果slot为-1，则尝试查找一次有效槽位
	if (slot == -1)
		slot = FindEmptyWeaponSlot();

	if (slot == -1 || !HasWeapon(weaponID))
		return false;

	Weapon& weapon = GetWeapon(weaponID);
	SetCurWeaponSlot(weapon, slot);
	bool result = true;
	if (equiped)
		result = EquipWeaponFromSlots(slot);

	return result;
}

bool Equipment::EquipWeaponFromSlots(const std::string & name)
{
	// 穿装备的逻辑是，装备首先在slot中，先从slot中取出
	// 装备，再装备该装备。
	auto& weapon = GetWeaponFromSlot(name);
	if (weapon)
		return EquipWeaponFromSlots(*weapon);
	return false;
}

bool Equipment::EquipWeaponFromSlots(Weapon & weapon)
{
	if (weapon.IsEquiped())
		return true;

	auto& player = GetCurPlayer();
	return weapon.Equiped(player);
}

/**
*	\brief 装备指定槽位的装备
*/
bool Equipment::EquipWeaponFromSlots(int slot)
{
	auto& weapon = GetWeaponFromSlot(slot);
	if(weapon)
		return EquipWeaponFromSlots(*weapon);
	return false;
}

int Equipment::GetCurEquipSlot() const
{
	return mCurEquipSlot;
}

bool Equipment::HasCurWeapon() const
{
	return HasWeaponBySlot(mCurEquipSlot);
}

bool Equipment::HasWeaponBySlot(int curSlot)const
{
	Debug::CheckAssertion(curSlot >= 0 && curSlot < mMaxEquipSlot,
		"Invalid equipment slot:" + std::to_string(curSlot));

	return mEquipdWeapons.size() > curSlot &&
		mEquipdWeapons[curSlot] != nullptr;
}

WeaponPtr Equipment::GetWeaponFromSlot(int curSlot)
{
	Debug::CheckAssertion(curSlot >= 0 && curSlot < mMaxEquipSlot, 
		"Invalid equipment slot:" + std::to_string(curSlot));

	return mEquipdWeapons[curSlot];
}

WeaponPtr Equipment::GetWeaponFromSlot(const std::string & name)
{
	for (auto& weapon : mEquipdWeapons)
	{
		if (weapon->GetItemName() == name)
			return weapon;
	}
	return nullptr;
}

WeaponPtr Equipment::GetCurWeapon()
{
	return GetWeaponFromSlot(mCurEquipSlot);
}

/**
*	\brief 返回当前身上可装备的有效槽位，默认从1开始找
*/
int Equipment::FindEmptyWeaponSlot() const
{
	for (int i = 0; i < mMaxEquipSlot; i++)
	{
		if (!HasWeaponBySlot(i))
			return i;
	}
	return -1;
}

void Equipment::SetCurWeaponSlot(Weapon & weapon, int slot)
{
	mEquipdWeapons[slot] = std::dynamic_pointer_cast<Weapon>(
		weapon.shared_from_this());	// is work ok??
}

Player & Equipment::GetCurPlayer()
{
	return *mSavegame.GetGame()->GetPlayer();
}
