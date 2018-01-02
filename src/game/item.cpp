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
*	\brief 执行物品初始化工作
*
*	该工作会加载物品脚本，并执行脚本的onCreate操作
*/
void Item::Initialize()
{
	if (IsHasLua())
	{	
		GetLuaContext().RunItem(*this);
	}
}

/**
*	\brief 使用道具
*	\param count 使用道具的数量
*	\param usedEntity 使用的entity
*/
bool Item::UseItem(size_t count, Entity & usedEntity)
{
	return false;
}

/**
*	\brief 获取item被拾取时的音效，默认为空
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