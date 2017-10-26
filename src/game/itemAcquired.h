#pragma once

#include"common\common.h"
#include"game\item.h"

class Game;

/**
*	\brief ItemAcquired ��Ʒ��ȡ����
*
*	��Ʒ��ȡ���ݣ������˻�ȡ��Ʒ�����ƺͶ�Ӧ��
*	��Ʒ�������Լ���ȡ����Ʒʱ����Ϊ������һ��
*	�����Pickable�У�����pickʱ����
*/

class ItemAcquired
{
public:
	ItemAcquired(Game& game, const std::string& itemName, int variant);

	void GiveItemToPlayer();
	const string& GetItemName()const;
	void SetItemName(const string& name);
	int GetVariant()const;
	void SetVariant(int variant);
	Item& GetItem();

private:
	std::string mItemName;
	int mVariant;
	Game& mGame;
};

inline const string & ItemAcquired::GetItemName() const
{
	return mItemName;
}

inline void ItemAcquired::SetItemName(const string & name)
{
	mItemName = name;
}

inline int ItemAcquired::GetVariant() const
{
	return mVariant;
}

inline void ItemAcquired::SetVariant(int variant)
{
	mVariant = variant;
}
