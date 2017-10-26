#include "game\equipment.h"
#include "game\savegame.h"

Equipment::Equipment(Savegame & savegame):
	mSavegame(savegame)
{
	LoadAllItems();
}

/**
*	\brief ��������item
*/
void Equipment::LoadAllItems()
{
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

void Equipment::PushItemIntoBeg(Item & item)
{
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
