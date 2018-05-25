#include "movements\playerMovement.h"
#include "entity\entity.h"
#include "entity\player.h"
#include "game\game.h"
#include "game\gameCommands.h"
#include "game\timer.h"
#include "utils\geometry.h"

PlayerMovement::PlayerMovement(int speed):
	StraightMovement(false, true),
	mMovingSpeed(speed),
	mDirection(Direction8::DIRECTION8_NONE),
	mIsShift(false)
{
}

/**
*	\brief playerMovementˢ��
*
*	playerMovementˢ�£�����ݵ�ǰ�������λ��
*/
void PlayerMovement::Update()
{
	StraightMovement::Update();

	// λ�����򲻴���
	if (mIsShift)
	{
		mShiftTimer.Update();
		if (mShiftTimer.IsFinished())
			StopShift();
		return;
	}

	// ���ǿ�ƽ�������ֱ�ӽ���
	if (IsStop() && mDirection != Direction8::DIRECTION8_NONE)
	{
		mDirection = Direction8::DIRECTION8_NONE;
		ComputeMovement();
	}
	else
	{
		// ���ݵ�ǰ���������ٶ�
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

int PlayerMovement::GetDirection() const
{
	return static_cast<Direction4>(mDirection / 2);
}

/**
*	\brief ���㵱ǰmovment
*
*	��direction�ı�ʱ��������Ϊ����stopʱ���øú���
*	����speed����angle���ᴥ��notifyMovementChanged
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

void PlayerMovement::StartShift(int shiftSpeed)
{
	if (!mIsShift)
	{
		mIsShift = true;

		auto direction = mDirection;
		if (direction == Direction8::DIRECTION8_NONE)
		{
			auto& player = dynamic_cast<Player&>(*GetEntity());
			direction = player.GetDirection8();
		}

		double angle = static_cast<double>(direction) * 45;
		SetSpeed((double)shiftSpeed);
		SetAngle((double)Geometry::Radians((float)angle));

		mShiftTimer.SetDuration(250);
	}
}
 
void PlayerMovement::StopShift()
{
	mIsShift = false;
	Stop();
}

bool PlayerMovement::IsShifting() const
{
	return mIsShift;
}

void PlayerMovement::NotifyObstacleReached()
{
	//StopShift();
}