#pragma once

#include"common\common.h"
#include"game\item.h"
#include"game\itemBeg.h"
#include"game\itemAcquired.h"
#include"game\weapon.h"

class Savegame;
class Player;

/**
*	\brief equipment ������е�ǰ����Ϸ��ֵ����
*	ͬʱ����ǰ��ӵ�е�item��weapon
*
*	equipment��player����ֵ��ʾ����,���е���ֵͨ
*	�����������savegame�У�ͬʱ��������������
*	�������savegame)
*
*	17.10.25 ��Ʒ�����������ͬ������ͱ����ʱ����Ҫ����
*   18.5.4   ���װ����������Ʒϵͳ
*/
class Equipment
{
public:
	Equipment(Savegame& savegame);

	void Update();
	bool SaveGame();
	bool LoadGame();

	/****** item manager *******/
	void LoadAllItems();
	Item& GetItem(const std::string& itemName);
	const Item& GetItem(const std::string& itemName)const;
	bool PushItemIntoBeg(ItemAcquired& itemAccquired);

	/** Setter/Getter */
	int GetLife()const;
	void SetLife(int life);
	int GetMaxLife()const;
	void SetMaxLife(int maxLife);
	Savegame& GetSavegame();

	/******* Weapon Manager ******/
	struct WeaponConfig	// ����װ��������Ʒͬ��������������ݣ�ʵʱ����
	{					// װ��ʵ��(��Ʒʹ�õ������ڵ��뷨��
		WeaponData data;
		int count;
	};
	void LoadAllWeapon();
	bool HasWeapon(const std::string& weaponID)const;
	Weapon& GetWeapon(const std::string& weaponID);

	/** equip operation */
	bool EquipWeaponFromSlots(const std::string& name);
	bool EquipWeaponFromSlots(int slot);
	bool EquipWeaponFromSlots(Weapon& weapon);
	bool UnEquipWeaponFromSlots(const std::string& name);
	bool UnEquipWeaponFromSlots(int slot);
	bool UnEquipWeaponFromSlots(Weapon& weapon);

	bool AddWeaponToSlot(const std::string& weaponID, bool equiped = false, int slot = -1);
	int GetCurEquipSlot()const;
	bool HasCurWeapon()const;
	bool HasWeaponBySlot(int curSlot)const;

	WeaponPtr GetCurWeapon();
	WeaponPtr GetWeaponFromSlot(int curSlot);
	WeaponPtr GetWeaponFromSlot(const std::string& name);

private:
	int FindEmptyWeaponSlot()const;
	void SetCurWeaponSlot(Weapon& weapon, int slot);
	Player& GetCurPlayer();

private:
	Savegame& mSavegame;
	std::map<std::string, ItemPtr> mAllItems;	// �������ǰ����ע��item
	ItemBeg mItemBegs;							// ��ǰ��Ϸ����
	//std::vector<ItemPtr> mItemBegs;			// ��ǰ��Ʒ����
	
	std::map<std::string, WeaponConfig> mAllWeapons;
	int mCurEquipSlot;
	int mMaxEquipSlot;
	std::vector<WeaponPtr> mEquipdWeapons;
};