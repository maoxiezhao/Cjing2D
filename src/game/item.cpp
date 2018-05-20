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
*	\brief ִ����Ʒ��ʼ������
*
*	�ù����������Ʒ�ű�����ִ�нű���onCreate����
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
*	\brief ʹ�õ���
*	\param count ʹ�õ��ߵ�����
*	\param usedEntity ʹ�õ�entity
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

	// ����ʹ����Ʒ
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
*	\brief ��ȡitem��ʰȡʱ����Ч��Ĭ��Ϊ��
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
*	\brief ���õ�ǰ��Ʒ������������AddItem��UsingItem�з��ʵ�
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

		// ��Ʒʹ��ǰ�������������ж���Ʒ�Ƿ����ʹ��
		luaContext.CallFunctionWithUserdata(*this, "OnBeforeUsingItem", pushItemFunc);
	
		// ��Ʒʹ��
		luaContext.CallFunctionWithUserdata(*this, "OnUsingItem", pushItemFunc);

		// ��Ʒʹ�ú�
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