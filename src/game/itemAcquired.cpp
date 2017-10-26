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
*	\brief 获取item
*/
Item& ItemAcquired::GetItem()
{
	return mGame.GetEquipment().GetItem(mItemName);
}

/**
*	\brief 将物体交给player
*
*	该操作会将该item添加到equipment begs
*/
void ItemAcquired::GiveItemToPlayer()
{
	Item& item = GetItem();
	auto& equipment = mGame.GetEquipment();

	equipment.PushItemIntoBeg(item);

	// 响应lua onObtaining函数
}
