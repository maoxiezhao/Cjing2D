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
*	\brief playerMovementˢ��
*
*	playerMovementˢ�£�����ݵ�ǰ�������λ��
*/
void PlayerMovement::Update()
{
	StraightMovement::Update();

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