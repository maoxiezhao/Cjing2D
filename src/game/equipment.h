#pragma once

#include"common\common.h"

class Savegame;

/**
*	\brief equipment ������е�ǰ����Ϸ��ֵ����
*	ͬʱ����ǰ��ӵ�е�item
*
*	equipment��player����ֵ��ʾ����,���е���ֵͨ
*	�����������savegame�У�ͬʱ��������������
*	�������savegame)
*/
class Equipment
{
public:
	Equipment(Savegame& savegame);

	/**** ***** Setter/Getter **** *****/
	int GetLife()const;
	void SetLife(int life);

private:
	Savegame& mSavegame;
};