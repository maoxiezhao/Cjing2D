#pragma once

#include"lua\luaObject.h"
#include<memory>

/**
*	\brief 物品类
*/
class Item : public LuaObject
{
public:
	Item(const std::string& itemName);

	/** status */
	const std::string& GetPickedSound()const;

	/** 物品特效 */
	bool HasShadow();
	bool HasFlowEffect();

	virtual const string GetLuaObjectName()const;

private:
	std::string mItemName;
	std::string mSavegameVariable;
	std::string mSavegameAmount;
	std::string mPickedSound;

	bool mHasShadow;
	bool mHasFlowEffect;

	int mMaxAmount;
 
};

using ItemPtr = std::shared_ptr<Item>;