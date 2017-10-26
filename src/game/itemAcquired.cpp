#include "itemAcquired.h"
#include "game\game.h"
#include "game\equipment.h"

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
void ItemAcquired::GiveItemToPlayer()
{
	Item& item = GetItem();
	auto& equipment = mGame.GetEquipment();

	equipment.PushItemIntoBeg(item);

	// ��Ӧlua onObtaining����
}
