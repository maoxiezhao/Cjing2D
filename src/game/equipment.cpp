#include "game\equipment.h"
#include "game\savegame.h"
#include "game\gameResources.h"
#include "game\item.h"

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
		if (weapon.IsEquiped())
			weapon.Update();
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
	// ��ʼ�����нű�
	for (auto& kvp : mAllItems)
	{
		Item& item = *kvp.second;
		item.Initialize();
	}
}

/**
*	\brief ��ȡ�ƶ����ֵ�item
*
*	��item�����Ѿ�ע��
*/
Item & Equipment::GetItem(const std::string & itemName)
{
	auto itor = mAllItems.find(itemName);
	if (itor == mAllItems.end())
	{
		Debug::Error("Get the non-exists item:" + itemName);
	}
	return *itor->second;
}

/**
*	\brief ��ȡ�ƶ����ֵ�item
*
*	��item�����Ѿ�ע��
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

void Equipment::EquipWeapon(const std::string & name)
{
	// ��װ�����߼��ǣ�װ��������slot�У��ȴ�slot��ȡ��
	// װ������װ����װ����
	

}

void Equipment::EquipWeapon(Weapon & weapon)
{
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

Weapon & Equipment::GetWeaponBySlot(int curSlot)
{
	Debug::CheckAssertion(curSlot >= 0 && curSlot < mMaxEquipSlot, 
		"Invalid equipment slot:" + std::to_string(curSlot));
	return *mEquipdWeapons[curSlot].get();
}

Weapon & Equipment::GetCurWeapon()
{
	return GetWeaponBySlot(mCurEquipSlot);
}
