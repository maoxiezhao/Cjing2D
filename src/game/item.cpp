#include "item.h"

Item::Item(const std::string& itemName):
	mItemName(itemName),
	mPickedSound("")
{
}

/**
*	\brief ��ȡitem��ʰȡʱ����Ч��Ĭ��Ϊ��
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
