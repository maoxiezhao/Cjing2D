#pragma once

#include"common\common.h"
#include"game\item.h"

class Savegame;

/**
*	\brief equipment 类包含有当前的游戏数值属性
*	同时管理当前所拥有的item
*
*	equipment是player的数值显示管理,所有的数值通
*	过该类管理在savegame中，同时保存有两份数据
*	（该类和savegame)
*
*	17.10.25 商品的问题的数据同步问题和保存的时机需要考虑
*/
class Equipment
{
public:
	Equipment(Savegame& savegame);

	/** item manager */
	void LoadAllItems();
	Item& GetItem(const std::string& itemName);
	const Item& GetItem(const std::string& itemName)const;

	bool PushItemIntoBeg(Item& item);

	/** Setter/Getter */
	int GetLife()const;
	void SetLife(int life);
	int GetMaxLife()const;
	void SetMaxLife(int maxLife);
	Savegame& GetSavegame();

private:
	Savegame& mSavegame;
	std::map<std::string, ItemPtr> mAllItems;	// 保存管理当前所有注册itme
	std::vector<ItemPtr> mItemBegs;				// 当前物品背包

};