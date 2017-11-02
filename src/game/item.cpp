#include "game\item.h"
#include "game\equipment.h"
#include "game\savegame.h"
#include "lua\luaContext.h"

Item::Item(const std::string& itemName, Equipment& equipment):
	mEquipment(equipment),
	mItemName(itemName),
	mPickedSound(""),
	mHasShadow(true),
	mHasFlowEffect(true),
	mAutoPicked(false),
	mHasLua(true)
{
}

/**
*	\brief ִ����Ʒ��ʼ������
*
*	�ù����������Ʒ�ű�����ִ�нű���onCreate����
*/
void Item::Initialize()
{
	if (IsHasLua())
	{	
		GetLuaContext().RunItem(*this);
	}
}

/**
*	\brief ��ȡitem��ʰȡʱ����Ч��Ĭ��Ϊ��
*/
const std::string & Item::GetPickedSound() const
{
	return mPickedSound;
}

LuaContext& Item::GetLuaContext()
{
	return GetSavegame().GetLuaContext();
}

const string Item::GetLuaObjectName() const
{
	return LuaContext::module_item_name;
}

Equipment & Item::GetEquipment()
{
	return mEquipment;
}

Savegame & Item::GetSavegame()
{
	return mEquipment.GetSavegame();
}