#pragma once

#include"game\item.h"

class Equipment;
class Savegame;

/**
*	\brief ÎïÆ·±³°ü
*/
class ItemBeg
{
public:
	ItemBeg(Equipment& equipment);

private:
	Equipment& mEquipment;

};