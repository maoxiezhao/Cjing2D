#pragma once

#include"game\item.h"

class Equipment;
class Savegame;

/**
*	\brief ��Ʒ����
*/
class ItemBeg
{
public:
	ItemBeg(Equipment& equipment);

private:
	Equipment& mEquipment;

};