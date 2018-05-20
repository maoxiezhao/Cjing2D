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
	mHasLua(true),
	mCurCount(0),
	mMaxAmount(99)
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
		GetLuaContext().RunItem(*this);
}

void Item::Update()
{
}

void Item::Uninitialize()
{
}

/**
*	\brief 使用道具
*	\param count 使用道具的数量
*	\param usedEntity 使用的entity
*/
bool Item::UseItem(int count, Entity* usedEntity)
{
	if (mCurCount - count < 0)
	{
		std::stringstream oss;
		oss << "The current item count is " << mCurCount;
		oss << ", but try to use " << count;
		Debug::Warning(oss.str());
		return false;
	}

	// 依次使用物品
	for (int i = 0; i < count; i++)
	{
		if (!ItemUsing(usedEntity))
			return false;
	}
	return true;
}

bool Item::AddItem(int count)
{
	int newCount = std::min(mCurCount + count, mMaxAmount);
	return SetItemCount(newCount, true);
}

/**
*	\brief 获取item被拾取时的音效，默认为空
*/
std::string Item::GetPickedSound() const
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

/**
*	\brief 设置当前物品个数，仅能在AddItem和UsingItem中访问到
*/
bool Item::SetItemCount(int newCount, bool notify)
{
	int oldCount = mCurCount;
	if (oldCount == newCount)
		return false;

	mCurCount = newCount;

	if ( notify && IsHasLua())
		GetLuaContext().CallFunctionWithUserdata(*this, "OnCountChange",
			[&](lua_State*l) {
		lua_pushinteger(l, oldCount);
		lua_pushinteger(l, newCount);
		return 2;
	});

	GetLuaContext().DoFireLuaSystemEvent(
		LuaContext::SystemLuaEvent::EVENT_ITEM_COUNT_CHANGE,
		[&](lua_State*l) {
		GetLuaContext().PushItem(l, *this);
		lua_pushinteger(l, oldCount);
		lua_pushinteger(l, newCount);
		return 3;
	});
	return true;
}

bool Item::ItemUsing(Entity* usedEntity)
{
	if (IsHasLua()) {	
		auto& luaContext = GetLuaContext();
		static auto pushItemFunc = [&](lua_State*l) {
			if (usedEntity != nullptr) {
				LuaContext::PushUserdata(l, *usedEntity);
				return 1;
			}
			return 0;
		};

		// 物品使用前，可以在这里判断物品是否可以使用
		luaContext.CallFunctionWithUserdata(*this, "OnBeforeUsingItem", pushItemFunc);
	
		// 物品使用
		luaContext.CallFunctionWithUserdata(*this, "OnUsingItem", pushItemFunc);

		// 物品使用后
		int newCount = std::max(mCurCount - 1, 0);
		if (!SetItemCount(newCount))
			return false;
		luaContext.CallFunctionWithUserdata(*this, "OnAfterUsingItem", pushItemFunc);
	}
	return true;
}

Equipment & Item::GetEquipment()
{
	return mEquipment;
}

Savegame & Item::GetSavegame()
{
	return mEquipment.GetSavegame();
}