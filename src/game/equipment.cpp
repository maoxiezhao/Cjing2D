#include "game\equipment.h"
#include "game\savegame.h"
#include "game\gameResources.h"
#include "game\item.h"

Equipment::Equipment(Savegame & savegame):
	mSavegame(savegame),
	mItemBegs(*this)
{
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
