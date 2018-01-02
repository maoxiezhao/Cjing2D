#pragma once

#include"lua\luaObject.h"
#include"entity\entity.h"
#include<memory>

class Equipment;
class Savegame;

/**
*	\brief 物品类
*/
class Item : public LuaObject
{
public:
	Item(const std::string& itemName, Equipment& equipment);

	void Initialize();
	bool UseItem(size_t count, Entity& usedEntity);

	/** status */
	const std::string& GetPickedSound()const;
	const std::string& GetItemName()const;
	Equipment& GetEquipment();
	Savegame& GetSavegame();

	/** 物品特效 */
	void SetHasShadow(bool hasShadow);
	bool IsHasShadow()const;
	void SetHasFlowEffect(bool hasFlowEffect);
	bool IsHasFlowEffect()const;
	void SetAutoPicked(bool autoPicked);
	bool IsAutoPicked()const;
	bool IsHasLua()const;

	LuaContext& GetLuaContext();
	virtual const string GetLuaObjectName()const;

private:
	Equipment& mEquipment;

	std::string mItemName;
	std::string mSavegameVariable;
	std::string mSavegameAmount;
	std::string mPickedSound;

	bool mHasShadow;		/** 是否开启阴影特效 */
	bool mHasFlowEffect;	/** 是否开启浮动特效 */
	bool mHasLua;			/** 是否存在lua脚本 */
	bool mAutoPicked;		/** 是否自动拾取 */
		
	int mMaxAmount;
 
};

using ItemPtr = std::shared_ptr<Item>;

inline const std::string & Item::GetItemName() const
{
	return mItemName;
}

inline void Item::SetHasShadow(bool hasShadow)
{
	mHasShadow = hasShadow;
}

inline bool Item::IsHasShadow()const
{
	return mHasShadow;
}

inline void Item::SetHasFlowEffect(bool hasFlowEffect)
{
	mHasFlowEffect = hasFlowEffect;
}

inline bool Item::IsHasFlowEffect()const
{
	return mHasFlowEffect;
}

inline void Item::SetAutoPicked(bool autoPicked)
{
	mAutoPicked = autoPicked;
}

inline bool Item::IsHasLua() const
{
	return mHasLua;
}

inline bool Item::IsAutoPicked() const
{
	return mAutoPicked;
}
