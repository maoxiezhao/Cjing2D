#pragma once

#include"game\item.h"

class Equipment;
class Savegame;

/**
*	\brief ��Ʒ����
*
*	���߱���������ǰ���еĵ���,����ֻ�й̶��ı����ۣ�
*	�����Ե��߲�ֵ���ʽ����ڸ��������ṩ���ߵĲ��
*	�ϲ�������λ�á����ҵ��ߵȲ���
*
*	�����ڱ�������˳���ķ�ʽ�洢��ÿ�����߸���index
*	�洢��begItems��
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
	*	\brief ��������ߵĻ��������
	*/
	class BegItem
	{
	public:
		BegItem();

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

	BegItem& GetBegItem(int index);
	std::vector<BegItem&> GetBegItem(const std::string& itemName);

private:
	Equipment& mEquipment;

	int mMaxBegItemCount;			/** ��ǰ������������*/
	std::vector<BegItem> mBegItems;	/** ��ǰ�������߲� */

};