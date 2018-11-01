#pragma once

#include"movements\straightMovement.h"
#include"game\gameCommands.h"
#include"game\timer.h"

/**
*	\brief ��ɫ���ƶ���
*
*	��ɫ���ƶ��࣬��ȡgameCommand���ƶ�����ͽ�ɫ���ƶ��ٶ�
*	�����ƶ�����ֹͣ, ͬʱPlayer��������һ������λ��
*/

class PlayerMovement : public StraightMovement
{
public:
	PlayerMovement(int speed);

	virtual void Update();
	virtual const string GetLuaObjectName()const;

	virtual int GetDirection()const;
	virtual int GetDirection8()const;
	void ComputeMovement();
	int GetMovingSpeed()const;

	void StartShift(int shiftSpeed);
	void StopShift();
	bool IsShifting()const;

	virtual void NotifyObstacleReached();
private:
	int mMovingSpeed;
	bool mIsShift;
	Direction8 mDirection;
	Timer mShiftTimer;
};

inline int PlayerMovement::GetMovingSpeed()const
{
	return mMovingSpeed;
}

