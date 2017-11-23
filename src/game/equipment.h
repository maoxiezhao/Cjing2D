#pragma once

#include"common\common.h"
#include"game\item.h"
#include"game\itemBeg.h"
#include"game\itemAcquired.h"

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

private:
	Savegame& mSavegame;
	std::map<std::string, ItemPtr> mAllItems;	// �������ǰ����ע��itme
	ItemBeg mItemBegs;							// ��ǰ��Ϸ����
	//std::vector<ItemPtr> mItemBegs;				// ��ǰ��Ʒ����
};