#include "itemAcquired.h"
#include "game\game.h"
#include "game\equipment.h"
#include "lua\luaContext.h"

ItemAcquired::ItemAcquired(Game& game, const std::string & itemName, int variant):
	mGame(game),
	mItemName(itemName),
	mVariant(variant)
{
}

/**
*	\brief ��ȡitem
*/
Item& ItemAcquired::GetItem()
{
	return mGame.GetEquipment().GetItem(mItemName);
}

/**
*	\brief �����彻��player
*
*	�ò����Ὣ��item��ӵ�equipment begs
*/
bool ItemAcquired::GiveItemToPlayer()
{
	Item& item = GetItem();
	auto& equipment = mGame.GetEquipment();

	// �������lua�ű�������Ӧlua onObtaining����
	// ����itemֱ�Ӽ��뱳��
	bool giveSuccessed = false;
	if (!item.IsHasLua())
	{
		giveSuccessed = equipment.PushItemIntoBeg(item);
	}
	else
	{
		giveSuccessed = mGame.GetLuaContext().OnItemObtained(item, *this);
	}
	return giveSuccessed;
}
