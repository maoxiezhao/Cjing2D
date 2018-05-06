#pragma once

#include"game\item.h"

class Equipment;
class Savegame;

/**
*	\brief 物品背包
*
*	道具背包包含当前已有的道具,背包只有固定的背包槽，
*	道具以道具拆分的形式存放在各个槽中提供道具的拆分
*	合并、交换位置、查找道具等操作
*
*	道具在背包中以顺序表的方式存储，每个道具根据index
*	存储在begItems中
*/
class ItemBeg
{
public:
	ItemBeg(Equipment& equipment);

	void Initialize();
	void Sort();

	Savegame& GetSavegame();
	Equipment& GetEquipment();

public:
	/**
	*	\brief 背包项，道具的基本拆分项
	*/
	class SubBegItem
	{
	public:
		SubBegItem();

		Item* GetItem();
		int GetVariant();
		void SetItem(Item* item);
		void SetVaritant(int varitant);

	private:
		Item* mItem;
		int mVariant;
	};

	void SetItem(Item* item, int variant, int index);
	void RemoveItem(int index, int variant);
	void RemoveItem(const std::string& itemName, int variant);
	void SplitItem(int itemIndem, int variant, int splitIndex);
	void SplitItem(const std::string& item, int variant, int toIndex);
	void SwapItem();

	SubBegItem& GetBegItem(int index);
	std::vector<SubBegItem&> GetBegItem(const std::string& itemName);

private:
	Equipment& mEquipment;

	int mMaxBegItemCount;				/** 当前背包的最大槽数*/
	std::vector<SubBegItem> mBegItems;	/** 当前背包道具槽 */
	std::map<Item*, int> mItemsCount;	
};