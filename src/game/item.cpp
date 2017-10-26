#include "item.h"

Item::Item(const std::string& itemName):
	mItemName(itemName),
	mPickedSound("")
{
}

/**
*	\brief 获取item被拾取时的音效，默认为空
*/
const std::string & Item::GetPickedSound() const
{
	return mPickedSound;
}

bool Item::HasShadow()
{
	return false;
}

bool Item::HasFlowEffect()
{
	return false;
}

const string Item::GetLuaObjectName() const
{
	return string();
}
