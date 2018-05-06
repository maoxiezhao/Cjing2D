#pragma once

#include"common\common.h"
#include"game\item.h"
#include"game\itemBeg.h"
#include"game\itemAcquired.h"
#include"game\weapon.h"

class Savegame;
class Player;

/**
*	\brief equipment 类包含有当前的游戏数值属性
*	同时管理当前所拥有的item，weapon
*
*	equipment是player的数值显示管理,所有的数值通
*	过该类管理在savegame中，同时保存有两份数据
*	（该类和savegame)
*
*	17.10.25 商品的问题的数据同步问题和保存的时机需要考虑
*   18.5.4   添加装备，完善物品系统
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
	struct WeaponConfig	// 这里装备不和物品同步，保存的是数据，实时创建
	{					// 装备实体(物品使用的是早期的想法）
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
	std::map<std::string, ItemPtr> mAllItems;	// 保存管理当前所有注册item
	ItemBeg mItemBegs;							// 当前游戏背包
	//std::vector<ItemPtr> mItemBegs;			// 当前物品背包
	
	std::map<std::string, WeaponConfig> mAllWeapons;
	int mCurEquipSlot;
	int mMaxEquipSlot;
	std::vector<WeaponPtr> mEquipdWeapons;
};