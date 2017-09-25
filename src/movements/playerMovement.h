#pragma once

#include"movements\straightMovement.h"
#include"game\gameCommands.h"

/**
*	\brief 角色的移动类
*
*	角色的移动类，获取gameCommand的移动方向和角色的移动速度
*	设置移动或者停止
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
