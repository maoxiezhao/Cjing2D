#pragma once

#include"movements\straightMovement.h"
#include"game\gameCommands.h"

/**
*	\brief ��ɫ���ƶ���
*
*	��ɫ���ƶ��࣬��ȡgameCommand���ƶ�����ͽ�ɫ���ƶ��ٶ�
*	�����ƶ�����ֹͣ
*/

class PlayerMovement : public StraightMovement
{
public:
	PlayerMovement(int speed);

	virtual void Update();
	virtual const string GetLuaObjectName()const;

	void ComputeMovement();
	int GetMovingSpeed()const;
	Direction8 GetDirection8()const;

private:
	int mMovingSpeed;
	Direction8 mDirection;

};

inline int PlayerMovement::GetMovingSpeed()const
{
	return mMovingSpeed;
}

inline Direction8 PlayerMovement::GetDirection8() const
{
	return mDirection;
}
