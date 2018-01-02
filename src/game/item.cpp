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
*	\brief ʹ�õ���
*	\param count ʹ�õ��ߵ�����
*	\param usedEntity ʹ�õ�entity
*/
bool Item::UseItem(size_t count, Entity & usedEntity)
{
	return false;
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