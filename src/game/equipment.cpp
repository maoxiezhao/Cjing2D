#include "game\equipment.h"
#include "game\savegame.h"

Equipment::Equipment(Savegame & savegame):
	mSavegame(savegame)
{
	LoadAllItems();
}

/**
*	\brief 加载所有item
*/
void Equipment::LoadAllItems()
{
}

/**
*	\brief 获取制定名字的item
*
*	该item必须已经注册
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
*	\brief 获取制定名字的item
*
*	该item必须已经注册
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

void Equipment::PushItemIntoBeg(Item & item)
{
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
