#pragma once

#include"lua\luaObject.h"
#include<memory>

/**
*	\brief ��Ʒ��
*/
class Item : public LuaObject
{
public:
	Item(const std::string& itemName);

	/** status */
	const std::string& GetPickedSound()const;

	/** ��Ʒ��Ч */
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