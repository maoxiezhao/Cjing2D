#pragma once

#include"common\common.h"
#include"game\item.h"

class Game;

/**
*	\brief ItemAcquired 物品获取数据
*
*	物品获取数据，保存了获取物品的名称和对应的
*	物品数量，以及获取该物品时的行为，该类一般
*	组合在Pickable中，用以pick时触发
*/

class ItemAcquired
{
public:
	ItemAcquired(Game& game, const std::string& itemName, int variant);

	void GiveItemToPlayer();
	const string& GetItemName()const;
	void SetItemName(const string& name);
	int GetVariant()const;
	void SetVariant(int variant);
	Item& GetItem();

private:
	std::string mItemName;
	int mVariant;
	Game& mGame;
};

inline const string & ItemAcquired::GetItemName() const
{
	return mItemName;
}

inline void ItemAcquired::SetItemName(const string & name)
{
	mItemName = name;
}

inline int ItemAcquired::GetVariant() const
{
	return mVariant;
}

inline void ItemAcquired::SetVariant(int variant)
{
	mVariant = variant;
}
