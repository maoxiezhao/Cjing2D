#include "movements\playerMovement.h"
#include "entity\entity.h"
#include "game\game.h"
#include "game\gameCommands.h"
#include "utils\geometry.h"

PlayerMovement::PlayerMovement(int speed):
	StraightMovement(false),
	mMovingSpeed(speed),
	mDirection(Direction8::DIRECTION8_NONE)
{
}

/**
*	\brief playerMovement刷新
*
*	playerMovement刷新，会根据当前方向计算位移
*/
void PlayerMovement::Update()
{
	StraightMovement::Update();

	// 如果强制结束，则直接结束
	if (IsStop() && mDirection != Direction8::DIRECTION8_NONE)
	{
		mDirection = Direction8::DIRECTION8_NONE;
		ComputeMovement();
	}
	else
	{
		// 根据当前方向设置速度
		GameCommands& gameCommands = GetEntity()->GetGame().GetGameCommands();
		Direction8 wantedDirection = gameCommands.GetWantedDirection8();
		if (wantedDirection != mDirection)
		{
			mDirection = wantedDirection;
			ComputeMovement();
		}
	}
}

const string PlayerMovement::GetLuaObjectName() const
{
	return string();
}

/**
*	\brief 计算当前movment
*
*	当direction改变时，或者人为调用stop时调用该函数
*	设置speed、和angle都会触发notifyMovementChanged
*/
void PlayerMovement::ComputeMovement()
{
	if (mDirection == Direction8::DIRECTION8_NONE)
	{
		Stop();
	}
	else
	{
		double angle = static_cast<double>(mDirection) * 45;
		SetSpeed((double)mMovingSpeed);
		SetAngle((double)Geometry::Radians((float)angle));
	}
}