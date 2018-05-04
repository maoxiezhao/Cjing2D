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
*	\brief ��������item
*/
void Equipment::LoadAllItems()
{
	// ��������item����
	const auto& itemsInfo = GameResource::GetGameResource().GetGameResourcesInfos(GameResourceType::ITEM);
	for (const auto& itemId : itemsInfo)
	{
		std::shared_ptr<Item> item = std::make_shared<Item>(itemId, *this);
		mAllItems[itemId] = item;
	}
	// ��������weapon����,��ʱ��ͬ��Ʒһ��Դ�
	const auto& weaponInfos = GameResource::GetGameResource().GetGameResourcesInfos(GameResourceType::WEAPON);
	for (const auto& weaponID : weaponInfos)
	{
		std::shared_ptr<Weapon> item = std::make_shared<Weapon>(weaponID, *this);
		mAllItems[weaponID] = item;
	}

	// ��ʼ�����нű�
	for (auto& kvp : mAllItems)
	{
		Item& item = *kvp.second;
		item.Initialize();
	}
}

/**
*	\brief ��ȡ�ƶ����ֵ�item
*/
Item & Equipment::GetItem(const std::string & itemName)
{
	auto it = mAllItems.find(itemName);
	if (it == mAllItems.end() || it->second->IsWeapon())
		Debug::Error("Get the non-exists item:" + itemName);

	return *it->second;
}

/**
*	\brief ��ȡ�ƶ����ֵ�item
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
*	\brief ��ȡ��ǰ����ֵ
*/
int Equipment::GetLife() const
{
	return mSavegame.GetInteger(Savegame::KEYWORD_CURRENT_LIFE);
}

/**
*	\brief ���õ�ǰ����ֵ
*/
void Equipment::SetLife(int life)
{
	mSavegame.SetInteger(Savegame::KEYWORD_CURRENT_LIFE, life);
}

/**
*	\brief ��ȡ��ǰ�������ֵ
*/
int Equipment::GetMaxLife() const
{
	return mSavegame.GetInteger(Savegame::KEYWORD_CURRENT_MAX_LIFE);
}

/**
*	\brief ���õ�ǰ�������ֵ
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
	// ��������item����
	const auto& itemsInfo = GameResource::GetGameResource().GetGameResourcesInfos(GameResourceType::ITEM);
	for (const auto& itemId : itemsInfo)
	{
		std::shared_ptr<Item> item = std::make_shared<Item>(itemId, *this);
		mAllItems[itemId] = item;
	}
	// ��ʼ�����нű�
	for (auto& kvp : mAllItems)
	{
		Item& item = *kvp.second;
		item.Initialize();
	}
}

/**
*	\brief �ж��Ƿ���ڸ�����ID
*/
bool Equipment::HasWeapon(const std::string & weaponID) const
{
	auto it = mAllItems.find(weaponID);
	return (it != mAllItems.end() && it->second->IsWeapon());
}

/**
*	\brief ��ȡ��ӦWeaponID��Ӧ��weaponʵ��
*/
Weapon & Equipment::GetWeapon(const std::string & weaponID)
{
	auto it = mAllItems.find(weaponID);
	if (it == mAllItems.end() || !it->second->IsWeapon())
		Debug::Error("Get the non-exists weapon:" + weaponID);
	
	return dynamic_cast<Weapon&>(*it->second);
}

/**
*	\brief �������������װ��
*	\param weaponID װ����ID
*	\param equiped �Ƿ�ֱ��װ��
*	\param slot װ���Ĳ�(0-2),�����-1���ǰ�����ҵ���һ����Ч��
*/
bool Equipment::AddWeaponToSlot(const std::string & weaponID, bool equiped, int slot)
{
	// ���slotΪ-1�����Բ���һ����Ч��λ
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
	// ��װ�����߼��ǣ�װ��������slot�У��ȴ�slot��ȡ��
	// װ������װ����װ����
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
*	\brief װ��ָ����λ��װ��
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
*	\brief ���ص�ǰ���Ͽ�װ������Ч��λ��Ĭ�ϴ�1��ʼ��
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
