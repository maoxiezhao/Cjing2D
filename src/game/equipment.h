#pragma once

#include"common\common.h"
#include"game\item.h"
#include"game\itemBeg.h"
#include"game\itemAcquired.h"
#include"game\weapon.h"

class Savegame;

/**
*	\brief equipment ������е�ǰ����Ϸ��ֵ����
*	ͬʱ����ǰ��ӵ�е�item
*
*	equipment��player����ֵ��ʾ����,���е���ֵͨ
*	�����������savegame�У�ͬʱ��������������
*	�������savegame)
*
*	17.10.25 ��Ʒ�����������ͬ������ͱ����ʱ����Ҫ����
*/
class Equipment
{
public:
	Equipment(Savegame& savegame);

	void Update();

	/** item manager */
	void LoadAllItems();
	Item& GetItem(const std::string& itemName);
	const Item& GetItem(const std::string& itemName)const;
	bool PushItemIntoBeg(ItemAcquired& item);

	/** Setter/Getter */
	int GetLife()const;
	void SetLife(int life);
	int GetMaxLife()const;
	void SetMaxLife(int maxLife);
	Savegame& GetSavegame();

	/** Weapon Manager */
	struct WeaponConfig	// ����װ��������Ʒͬ��������������ݣ�ʵʱ����
	{					// װ��ʵ��(��Ʒʹ�õ������ڵ��뷨��
		WeaponData data;
		int count;
	};
	void LoadAllWeapon();
	void EquipWeapon(const std::string& name);
	void EquipWeapon(Weapon& weapon);
	int GetCurEquipSlot()const;
	bool HasCurWeapon()const;
	bool HasWeaponBySlot(int curSlot)const;
	Weapon& GetWeaponBySlot(int curSlot);
	Weapon& GetCurWeapon();

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