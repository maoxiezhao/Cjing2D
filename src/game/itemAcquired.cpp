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
bool ItemAcquired::GiveItemToPlayer()
{
	Item& item = GetItem();
	auto& equipment = mGame.GetEquipment();

	// 如果存在lua脚本，则响应lua onObtaining函数
	// 否则将item直接加入背包
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
